#include <sys/time.h>
#include <sys/resource.h>

#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <cfloat>

#include <eo>
//#include <do/make_pop.h>
#include <ga.h>
#include <utils/eoFeasibleRatioStat.h>

#include "daex.h"
#include "evaluation/cpt-yahsp.h"
#include "evaluation/yahsp.h"

#ifdef WITH_MPI
#include <mpi/eoMpi.h>
#include <mpi/eoParallelApply.h>
#include <mpi/eoTerminateJob.h>
#endif

#include <utils/eoTimer.h>

/*
#ifndef NDEBUG
inline void LOG_LOCATION( eo::Levels level )
{
    eo::log << level << "in " << __FILE__ << ":" << __LINE__ << " " << __PRETTY_FUNCTION__ << std::endl; 
}
#endif
*/

#ifdef WITH_MPI
template< class T >
struct HandleResponseBestPlanDump : public eo::mpi::HandleResponseParallelApply< daex::Decomposition >
{
    HandleResponseBestPlanDump(
            std::string afilename,
            T worst,
            bool single_file = false,
            unsigned int file_count = 0,
            std::string sep = ".",
            std::string metadata = ""
        ) :
        _filename( afilename ),
        best( worst ), // The best at initialization is in fact the worst value we can have.
        _single_file( single_file ),
        _file_count( file_count ),
        _sep( sep ),
        _metadata( metadata )
    {
        // empty
    }

    // TODO TODOB copié / collé de evalBestPlanDump.h
    void dump( daex::Decomposition & eo )
    {
        std::ofstream _of;

        if( _single_file ) {
            // explicitely erase the file before writing in it
            _of.open( _filename.c_str(), std::ios_base::out | std::ios_base::trunc );

        } else {
            std::ostringstream afilename;
            afilename << _filename << _sep << _file_count;
            _of.open( afilename.str().c_str() );
        }

#ifndef NDEBUG
        if ( !_of.is_open() ) {
            std::string str = "Error, eoEvalBestFileDump could not open: " + _filename;
            throw std::runtime_error( str );
        }
        _of << IPC_PLAN_COMMENT << _metadata << std::endl;
        _of << IPC_PLAN_COMMENT << eo << std::endl;
#endif
        // here, in release mode, we assume that the file could be opened
        // thus, we avoid a supplementary test in this costly evaluator
        _of << eo.plan() << std::endl;
        _of.close();

        _file_count++;
    }

    void operator()( int wrkRank )
    {
        (*_wrapped)( wrkRank );
        int index = d->assignedTasks[ wrkRank ].index;
        int size = d->assignedTasks[ wrkRank ].size;
        for( int i = 0; i < size; ++i )
        {
            daex::Decomposition & decompo = d->data()[ index + i ];
            if( decompo.is_feasible() && decompo.plan().makespan() < best )
            {
                best = decompo.plan().makespan() ;
                dump( decompo );
            }
        }
    }

    protected:
    std::string _filename;
    T best;
    bool _single_file;
    unsigned int _file_count;
    std::string _sep;
    std::string _metadata;
};

struct IsFinishedBeforeTime : public eo::mpi::IsFinishedParallelApply< daex::Decomposition >
{
    IsFinishedBeforeTime( long maxTime ) : _maxTime( maxTime )
    {
        getrusage( RUSAGE_SELF , &_usage );
        _current = _usage.ru_utime.tv_sec + _usage.ru_stime.tv_sec;
    }

    bool operator()()
    {
        getrusage( RUSAGE_SELF , &_usage );
        _current = _usage.ru_utime.tv_sec + _usage.ru_stime.tv_sec;
        if ( _current > _maxTime )
        {
            throw eoMaxTimeException( _current );
        }
        return (*_wrapped)();
    }

protected:
    const long _maxTime;
    long _current;

    struct rusage _usage;
};

#endif // WITH_MPI

int main ( int argc, char* argv[] )
{
    // SYSTEM
#ifndef NDEBUG
    struct rlimit limit;
    getrlimit(RLIMIT_AS, &limit);
    eo::log << eo::logging << "Maximum size of the process virtual memory (soft,hard)=" << limit.rlim_cur << ", " << limit.rlim_max << std::endl;
    getrlimit(RLIMIT_DATA, &limit);
    eo::log << eo::logging << "Maximum size of the process   data segment (soft,hard)=" << limit.rlim_cur << ", " << limit.rlim_max << std::endl;

    /*
    // Hard coded memory usage limits
    limit.rlim_cur=100000000;
    limit.rlim_max=100000000;
    setrlimit(RLIMIT_AS, &limit);
    setrlimit(RLIMIT_DATA, &limit);

    getrlimit(RLIMIT_AS, &limit);
    std::cout << "Maximum size of the process virtual memory (soft,hard)=" << limit.rlim_cur << ", " << limit.rlim_max << std::endl;
    getrlimit(RLIMIT_DATA, &limit);
    std::cout << "Maximum size of the process   data segment (soft,hard)=" << limit.rlim_cur << ", " << limit.rlim_max << std::endl;
    */
#endif

    /**************
     * PARAMETERS *
     **************/

    // EO
    eoParser parser(argc, argv);
    make_verbose(parser);
    make_parallel(parser);

#ifdef WITH_MPI
    using eo::mpi::timerStat;
    timerStat.start("dae_main");
    eo::mpi::Node::init( argc, argv );

    int rank = eo::mpi::Node::comm().rank();
#endif

    eoState state;

    // log some EO parameters
    eo::log << eo::logging << "Parameters:" << std::endl;
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "verbose" << eo::log.getLevelSelected() << std::endl;
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "parallelize-loop" << eo::parallel.isEnabled() << std::endl;
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "parallelize-dynamic" << eo::parallel.isDynamic() << std::endl;
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "parallelize-prefix" << eo::parallel.prefix() << std::endl;
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "parallelize-nthreads" << eo::parallel.nthreads() << std::endl;


    // GENERAL PARAMETERS
    // createParam (ValueType _defaultValue, std::string _longName, std::string _description, char _shortHand=0, std::string _section="", bool _required=false)
    std::string domain = parser.createParam( (std::string)"domain-zeno-time.pddl", "domain", "PDDL domain file", 'D', "Problem", true ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "domain" << domain << std::endl;

    std::string instance = parser.createParam( (std::string)"zeno10.pddl", "instance", "PDDL instance file", 'I', "Problem", true ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "instance" << instance << std::endl;

    std::string plan_file = parser.createParam( (std::string)"plan.soln", "plan-file", "Plan file backup", 'F', "Misc" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "plan-file" << plan_file << std::endl;

    // pop size
    unsigned int pop_size = parser.createParam( (unsigned int)100, "popSize", "Population Size", 'P', "Evolution Engine").value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "pop_size" << pop_size << std::endl;

    // multi-start
    unsigned int maxruns = parser.createParam( (unsigned int)0, "runs-max", 
            "Maximum number of runs, if x==0: unlimited multi-starts, if x>1: will do <x> multi-start", 'r', "Stopping criterions" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "maxruns" << maxruns << std::endl;

    // b_max estimation
    bool insemination = parser.createParam(false, "insemination", "Use the insemination heuristic to estimate b_max at init", '\0', "Initialization").value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "insemination" << insemination << std::endl;

    // seed
    eoValueParam<unsigned int> & param_seed = parser.createParam( (unsigned int)0, "seed", "Random number seed", 'S' );
    // if one want to initialize on current time
    if ( param_seed.value() == 0) {
        // change the parameter itself, that will be dumped in the status file
        //        param_seed.value( time(0) );
        param_seed.value() = time(0); // EO compatibility fixed by CC on 2010.12.24
    }

    unsigned int seed;
# ifdef WITH_MPI
    // Sending the seed to everyone
    if( rank == eo::mpi::DEFAULT_MASTER )
    {
# endif // WITH_MPI
        seed = param_seed.value();
    }
# ifdef WITH_MPI
    bmpi::broadcast( eo::mpi::Node::comm(), seed, eo::mpi::DEFAULT_MASTER );
# endif // WITH_MPI

    rng.reseed( seed );
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "seed" << seed << std::endl;

    // Parameters makers
    daex::do_make_eval_param( parser );
    daex::do_make_init_param( parser );
    daex::do_make_variation_param( parser, pop_size);
    daex::do_make_checkpoint_param( parser );
    daex::do_make_replace_param( parser );

    // special case of stopping criteria parameters
    daex::do_make_continue_param( parser )    ;
    // Those parameters are needed during restarts (see below)
    unsigned int mingen = parser.valueOf<unsigned int>("gen-min");
    unsigned int steadygen = parser.valueOf<unsigned int>("gen-steady");
    unsigned int maxgens = parser.valueOf<unsigned int>("gen-max");

    make_help( parser );


    /***********
     * PARSING *
     ***********/

    // PDDL
#ifndef NDEBUG
    eo::log << eo::progress << "Load the instance..." << std::endl;
    eo::log.flush();
#endif

    daex::pddlLoad pddl( domain, instance, SOLVER_YAHSP, HEURISTIC_H1, eo::parallel.nthreads(), std::vector<std::string>());
    daex::Goal::atoms( & pddl.atoms() );

#ifndef NDEBUG
    eo::log << eo::progress << "Load the instance...OK" << std::endl;
    eo::log << eo::progress << "Initialization...";
    eo::log.flush();
#endif

    /******************
     * INITIALIZATION *
     ******************/

    daex::Init<daex::Decomposition>& init = daex::do_make_init_op<daex::Decomposition>( parser, state, pddl );

    // randomly generate the population with the init operator
    eoPop<daex::Decomposition> pop = eoPop<daex::Decomposition>( pop_size, init ) ;

    // used to pass the eval count through the several eoEvalFuncCounter evaluators
    unsigned int eval_count = 0;

    TimeVal best_makespan = INT_MAX;

#ifndef SINGLE_EVAL_ITER_DUMP
    std::string dump_sep = ".";
    unsigned int dump_file_count = 1;
    std::string metadata = "domain " + domain + "\n" + IPC_PLAN_COMMENT + "instance " + instance;
#endif

    unsigned int b_max_fixed = parser.valueOf<unsigned int>("bmax-fixed");
    if( b_max_fixed == 0 ) {
        // Heuristics for the estimation of an optimal b_max
        if( insemination ) {
            b_max_fixed = daex::estimate_bmax_insemination( parser, pddl, pop, init.l_max() );
        } else {
            // if not insemination, incremental search strategy
            b_max_fixed  = daex::estimate_bmax_incremental<daex::Decomposition>( 
                    pop, parser, init.l_max(), eval_count, plan_file, best_makespan, dump_sep, dump_file_count, metadata 
                    );
        }
    }

    unsigned b_max_in = b_max_fixed;
    double b_max_last_weight = parser.valueOf<double>("bmax-last-weight"); 

    unsigned int b_max_last = static_cast<unsigned int>( std::floor( b_max_in * b_max_last_weight ) );
#ifndef NDEBUG
    eo::log << eo::logging << std::endl << "\tb_max for intermediate goals, b_max_in: "   << b_max_in   << std::endl;
    eo::log << eo::logging              << "\tb_max for        final goal,  b_max_last: " << b_max_last << std::endl;
#endif

    /**************
     * EVALUATION *
     **************/

#ifndef NDEBUG
    eo::log << eo::progress << "Creating evaluators...";
    eo::log.flush();
#endif

    // do_make_eval returns a pair: the evaluator instance 
    // and a pointer on a func counter that may be null of we are in release mode
    std::pair< eoEvalFunc<daex::Decomposition>&, eoEvalFuncCounter<daex::Decomposition>* > eval_pair
        = daex::do_make_eval_op<daex::Decomposition>(
                parser, state, init.l_max(), eval_count, b_max_in, b_max_last, plan_file, best_makespan, dump_sep, dump_file_count, metadata
                );
    eoEvalFunc<daex::Decomposition>& eval = eval_pair.first;

#ifndef NDEBUG
    // in debug mode, we should have a func counter
    assert( eval_pair.second != NULL );
    eoEvalFuncCounter<daex::Decomposition>& eval_counter = * eval_pair.second;

    eo::log << eo::progress << "OK" << std::endl;

    eo::log << eo::progress << "Evaluating the first population...";
    eo::log.flush();
#endif

#ifdef WITH_MPI

    // TODO TODOB mettre ça dans un do_make_eval_parallel.h
    eoPopEvalFunc<daex::Decomposition>* p_pop_eval;

    bool parallelLoopEval = eo::parallel.isEnabled();
    if( parallelLoopEval )
    {
        eo::mpi::AssignmentAlgorithm* assign;
        if ( eo::parallel.isDynamic() )
        {
            assign = new eo::mpi::DynamicAssignmentAlgorithm ;
        } else
        {
            assign = new eo::mpi::StaticAssignmentAlgorithm ;
        }

        unsigned int packet_size = eo::parallel.packetSize();

        eo::mpi::ParallelApplyStore<daex::Decomposition>* store
            = new eo::mpi::ParallelApplyStore<daex::Decomposition>( eval, eo::mpi::DEFAULT_MASTER, packet_size );
        store->wrapHandleResponse( new HandleResponseBestPlanDump<TimeVal>(
                    plan_file,
                    best_makespan,
                    false,
                    dump_file_count,
                    dump_sep,
                    metadata
                    )
                );

        unsigned int max_seconds = parser.valueOf<unsigned int>("max-seconds");
        if( max_seconds > 0 )
        {
            store->wrapIsFinished( new IsFinishedBeforeTime( max_seconds ) );
        }

        // Add wrappers
        p_pop_eval = new eoParallelPopLoopEval<daex::Decomposition>(
                *assign,
                eo::mpi::DEFAULT_MASTER /* master rank */,
                store
                );
    } else
    {
        p_pop_eval = new eoPopLoopEval<daex::Decomposition>( eval );
    }
    eoPopEvalFunc<daex::Decomposition>& pop_eval = *p_pop_eval;
# else
    eoPopLoopEval<daex::Decomposition> pop_eval( eval );
# endif // WITH_MPI

# ifdef WITH_MPI
    if( rank != 0 )
    {
        // workers just perform evaluation
        eoPop<daex::Decomposition> pop;
        pop_eval( pop, pop );
        timerStat.stop("dae_main");

        eo::mpi::Node::comm().send( 0, 0, timerStat );

        return 0;
    }
# endif

    // a first evaluation of generated pop
    pop_eval( pop, pop );

#ifndef NDEBUG
    eo::log << eo::progress << "OK" << std::endl;
#endif

    /********************
     * EVOLUTION ENGINE *
     ********************/

#ifndef NDEBUG
    eo::log << eo::progress << "Algorithm instanciation...";
    eo::log.flush();
#endif

    // STOPPING CRITERIA
    eoCombinedContinue<daex::Decomposition> continuator = daex::do_make_continue_op<daex::Decomposition>( parser, state );

    // Direct access to continuators are needed during restarts (see below)
    eoSteadyFitContinue<daex::Decomposition> & steadyfit 
        = *( dynamic_cast<eoSteadyFitContinue<daex::Decomposition>* >( continuator[0] ) );
    eoGenContinue<daex::Decomposition> & maxgen 
        = *( dynamic_cast< eoGenContinue<daex::Decomposition>* >( continuator[1] ) );


    // CHECKPOINTING
    eoCheckPoint<daex::Decomposition> & checkpoint = daex::do_make_checkpoint_op( continuator, parser, state, pop
#ifndef NDEBUG
            , eval_counter
#endif
            );

    // SELECTION AND VARIATION
    // daex::MutationDelGoal<daex::Decomposition> delgoal; // FIXME delgoal devrait être un pointeur alloué sur le tas
    daex::MutationDelGoal<daex::Decomposition>* delgoal = new daex::MutationDelGoal<daex::Decomposition>;
    eoGeneralBreeder<daex::Decomposition> & breed = daex::do_make_variation_op<daex::Decomposition>( parser, state, pddl, delgoal );

    // REPLACEMENT
    eoReplacement<daex::Decomposition> & replacor = daex::do_make_replace_op<daex::Decomposition>( parser, state );
    unsigned int offsprings = parser.valueOf<unsigned int>("offsprings");

    // ALGORITHM
    // eoEasyEA<daex::Decomposition> dae( checkpoint, eval, breed, replacor, offsprings );
    eoEasyEA<daex::Decomposition> dae( checkpoint, eval, pop_eval, breed, replacor, offsprings );

#ifndef NDEBUG
    eo::log << eo::progress << "OK" << std::endl;
    //eo::log << eo::progress << "Note: dual fitness is printed as two numbers: a value followed by a boolean (0=unfeasible, 1=feasible)" << std::endl;
    eo::log.flush();
    eo::log << eo::debug << "Legend: \n\t- already valid, no eval\n\tx plan not found\n\t* plan found\n\ta add atom\n\tA add goal\n\td delete atom\n\tD delete goal\n\tC crossover" << std::endl;
#endif

    /********************
     * MULTI-START RUNS *
     ********************/

    // best decomposition of all the runs, in case of multi-start
    // start at the best element of the init
    daex::Decomposition best = pop.best_element();
    unsigned int run = 0;

    // evaluate an empty decomposition, for comparison with decomposed solutions
    daex::Decomposition empty_decompo;
# ifdef WITH_MPI
    {
        eoPop<daex::Decomposition> tempPop;
        tempPop.push_back( empty_decompo );
        pop_eval( tempPop, tempPop );
    }
# else // WITH_MPI
    eval( empty_decompo );
# endif // WITH_MPI

    class FinallyBlock
    {
        public:

        FinallyBlock(
# ifdef WITH_MPI
                eoPopEvalFunc<daex::Decomposition> * _p_pop_eval,
# endif // WITH_MPI
                eoPop<daex::Decomposition> & _pop,
                daex::Decomposition & _best,
                daex::Decomposition & _empty_decompo
                ) :
# ifdef WITH_MPI
            p_pop_eval( _p_pop_eval ),
# endif // WITH_MPI
            pop( _pop),
            best( _best),
            empty_decompo( _empty_decompo )
        {
            // empty
        }

        ~FinallyBlock()
        {

            // push the best result, in case it was not in the last run
            pop.push_back( best );

#ifndef NDEBUG
            // call the checkpoint, as if it was ending a generation
            // checkpoint( pop );
            eo::log << eo::progress << "... end of search" << std::endl;
#endif

#ifdef WITH_MPI
            delete p_pop_eval;
            timerStat.stop("dae_main");

            std::ostream & ss = std::cout;

            typedef std::map<std::string, eoTimerStat::Stat> statsMap;
            statsMap stats = timerStat.stats();
            for( statsMap::iterator it = stats.begin(), end = stats.end();
                    it != end;
                    ++it)
            {
                ss << "0 " << it->first << " S: ";
                for(unsigned j = 0; j < it->second.stime.size(); ++j)
                {
                    ss << it->second.stime[j] << " ";
                }
                ss << std::endl << "0 " << it->first << " U: ";

                for(unsigned j = 0; j < it->second.utime.size(); ++j)
                {
                    ss << it->second.utime[j] << " ";
                }
                ss << std::endl << "0 " << it->first << " W: ";
                for(unsigned j = 0; j < it->second.wtime.size(); ++j)
                {
                    ss << it->second.wtime[j] << " ";
                }
                ss << std::endl;
            }

            for(int i = 1; i < eo::mpi::Node::comm().size(); ++i)
            {
                eoTimerStat otherTimerStat;
                eo::mpi::Node::comm().recv( i, 0, otherTimerStat );
                statsMap stats = otherTimerStat.stats();
                for( statsMap::iterator it = stats.begin(), end = stats.end();
                        it != end;
                        ++it)
                {
                    ss << i << " " << it->first << " S: ";
                    for(unsigned j = 0; j < it->second.stime.size(); ++j)
                    {
                        ss << it->second.stime[j] << " ";
                    }
                    ss << std::endl << i << " " << it->first << " U: ";

                    for(unsigned j = 0; j < it->second.utime.size(); ++j)
                    {
                        ss << it->second.utime[j] << " ";
                    }
                    ss << std::endl << i << " " << it->first << " W: ";
                    for(unsigned j = 0; j < it->second.wtime.size(); ++j)
                    {
                        ss << it->second.wtime[j] << " ";
                    }
                    ss << std::endl;
                }
            }
#endif

            /*
               pop.push_back( empty_decompo );
            // push the best result, in case it was not in the last run
            pop.push_back( best );
            pop_eval( pop, pop ); // FIXME normalement inutile
            // print_results( pop, time_start, run ); // TODO TODOB temporaire
            //
            */
            std::cout << "End of main!" << std::endl;
        }

        private:

# ifdef WITH_MPI
        eoPopEvalFunc<daex::Decomposition> * p_pop_eval;
# endif // WITH_MPI
        eoPop<daex::Decomposition>& pop;
        daex::Decomposition & best;
        daex::Decomposition & empty_decompo;

    };

    try {
        FinallyBlock finallyBlock(
# ifdef WITH_MPI
                p_pop_eval,
# endif
                pop, best, empty_decompo );
        while( true )
        {
#ifndef NDEBUG
            eo::log << eo::progress << "Start the " << run << "th run..." << std::endl;

            // call the checkpoint (log and stats output) on the pop from the init
            checkpoint( pop );
#endif

# ifdef WITH_MPI
            timerStat.start("main_run");
# endif
            eo::log << eo::progress;
            eo::log << "Starting search..." << std::endl;
            // start a search
            dae( pop );
            eo::log << "After dae search..." << std::endl;

            // remember the best of all runs
            daex::Decomposition best_of_run = pop.best_element();

            // note: operator> is overloaded in EO, don't be afraid: we are minimizing
            if( best_of_run.fitness() > best.fitness() ) {
                best = best_of_run;
            }

            // TODO handle the case when we have several best decomposition with the same fitness but different plans?
            // TODO use previous searches to re-estimate a better b_max?

            // the loop test is here, because if we've reached the number of runs, we do not want to redraw a new pop
            run++;
            if( run >= maxruns && maxruns != 0 ) {
# ifdef WITH_MPI
                timerStat.stop("main_run");
# endif
                break;
            }

            // Once the bmax is known, there is no need to re-estimate it,
            // thus we re-init ater the first search, because the pop has already been created before,
            // when we were trying to estimate the b_max.
            pop = eoPop<daex::Decomposition>( pop_size, init );

            eo::log << "[Master] After re init of population, evaluating population." << std::endl;

            // evaluate
            // eoPopLoopEval<daex::Decomposition> pop_eval( eval ); // FIXME useful ??
            pop_eval( pop, pop );

            // reset run's continuator counters
            steadyfit.totalGenerations( mingen, steadygen );
            maxgen.totalGenerations( maxgens );
# ifdef WITH_MPI
            timerStat.stop("main_run");
# endif
        }
    } catch( std::exception const& e ) {
#ifndef NDEBUG
            eo::log << eo::progress << "... premature end of search, current result:" << std::endl;
#endif
            eo::log << eo::progress << "Leaving (living?) after an exception : " << e.what() << std::endl;
        return 0;
    }


    return 0;
}

