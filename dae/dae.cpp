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

#ifdef WITH_MPI
#include <mpi/eoMpi.h>

#include "do/make_parallel_eval_dae.h"
#include "do/make_parallel_multistart_dae.h"
#endif // WITH_MPI

#include <utils/eoTimer.h>

/*
#ifndef NDEBUG
inline void LOG_LOCATION( eo::Levels level )
{
    eo::log << level << "in " << __FILE__ << ":" << __LINE__ << " " << __PRETTY_FUNCTION__ << std::endl; 
}
#endif
*/

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


#ifdef DAE_MO // if multi-objective DAE
    typedef daex::DecompositionMO T;
#else
    typedef daex::Decomposition T;
#endif


    /**************
     * PARAMETERS *
     **************/

    // EO
    eoParser parser(argc, argv);
    make_verbose(parser);

#ifdef WITH_OMP
    make_parallel(parser);
#endif

#ifdef WITH_MPI
    using eo::mpi::timerStat;
    timerStat.start("dae_main");
    eo::mpi::Node::init( argc, argv );

    int rank = eo::mpi::Node::comm().rank();
#endif // WITH_MPI

    eoState state;

    // log some EO parameters
    eo::log << eo::logging << "Parameters:" << std::endl;
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "verbose" << eo::log.getLevelSelected() << std::endl;
#ifdef WITH_OMP
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "parallelize-loop" << eo::parallel.isEnabled() << std::endl;
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "parallelize-dynamic" << eo::parallel.isDynamic() << std::endl;
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "parallelize-prefix" << eo::parallel.prefix() << std::endl;
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "parallelize-nthreads" << eo::parallel.nthreads() << std::endl;
#endif


    // GENERAL PARAMETERS
    // createParam (ValueType _defaultValue, std::string _longName, std::string _description, char _shortHand=0, std::string _section="", bool _required=false)
    std::string domain = parser.createParam( (std::string)"domain-zeno-time.pddl", "domain", "PDDL domain file", 'D', "Problem", true ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "domain" << domain << std::endl;

    std::string instance = parser.createParam( (std::string)"zeno10.pddl", "instance", "PDDL instance file", 'I', "Problem", true ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "instance" << instance << std::endl;

    std::string plan_file = parser.createParam( (std::string)"plan.soln", "plan-file", "Plan file backup", 'F', "Output" ).value();
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

#ifdef DAE_MO
    // multi-objective algorithm
    double kappa = parser.createParam(0.05, "kappa", "Scaling factor kappa for indicator based search", 'k', "Evolution Engine").value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "kappa" << kappa << std::endl;

    double rho = parser.createParam(1.1, "rho", "reference point for the hypervolume indicator", 'R', "Evolution Engine").value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "rho" << rho << std::endl;
#endif

    // seed
    eoValueParam<unsigned int> & param_seed = parser.createParam( (unsigned int)0, "seed", "Random number seed", 'S' );
    // if one want to initialize on current time
    if ( param_seed.value() == 0) {
        // change the parameter itself, that will be dumped in the status file
        //        param_seed.value( time(0) );
        param_seed.value() = time(0); // EO compatibility fixed by CC on 2010.12.24
    }

    unsigned int seed = 0;
# ifdef WITH_MPI
    // Sending the seed to everyone
    if( rank == eo::mpi::DEFAULT_MASTER )
    {
# endif // WITH_MPI
        seed = param_seed.value();
# ifdef WITH_MPI
    }
    bmpi::broadcast( eo::mpi::Node::comm(), seed, eo::mpi::DEFAULT_MASTER );

    // Parallel hierarchy params
    unsigned int multistart_workers = parser.createParam( (unsigned int)0, "multistart-workers", "Number of workers for multi-start", '\0', "Parallelization").value();
    unsigned int eval_workers = parser.createParam( (unsigned int)0, "eval-workers", "Number of evaluators workers for each multi-start worker", '\0', "Parallelization").value();

    // check if we're in multistart mode
    bool with_multistart = parallel::check_roles_multistart( multistart_workers, eval_workers );
# endif // WITH_MPI

    rng.reseed( seed );
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "seed" << seed << std::endl;

    // Parameters makers
    daex::do_make_eval_param( parser );
    daex::do_make_init_param( parser );
    daex::do_make_variation_param( parser );
    daex::do_make_breed_param( parser, pop_size);
    daex::do_make_checkpoint_param( parser );

#ifndef DAE_MO // only in mono-objective
    daex::do_make_replace_param( parser );
#endif

    // special case of stopping criteria parameters
    daex::do_make_continue_param( parser )    ;
    // Those parameters are needed during restarts (see below)
#ifndef DAE_MO
    unsigned int mingen = parser.valueOf<unsigned int>("gen-min");
    unsigned int steadygen = parser.valueOf<unsigned int>("gen-steady");
#endif
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

    daex::Init<T>& init = daex::do_make_init_op<T>( parser, state, pddl );

    // randomly generate the population with the init operator
    eoPop<T> pop = eoPop<T>( pop_size, init ) ;

    // used to pass the eval count through the several eoEvalFuncCounter evaluators
    unsigned int eval_count = 0;

    TimeVal best_makespan = INT_MAX;

#ifndef SINGLE_EVAL_ITER_DUMP
    std::string dump_sep = ".";
    unsigned int dump_file_count = 1;
    std::string metadata = "domain " + domain + "\n" + IPC_PLAN_COMMENT + "instance " + instance;
#endif

#ifndef NDEBUG
    eo::log << eo::progress << "OK" << std::endl;
    //eo::log << eo::progress << "Note: dual fitness is printed as two numbers: a value followed by a boolean (0=unfeasible, 1=feasible)" << std::endl;
    eo::log.flush();
    eo::log << eo::debug << "Legend: \n\t- already valid, no eval\n\tx plan not found\n\t* plan found\n\ta add atom\n\tA add goal\n\td delete atom\n\tD delete goal\n\tC crossover" << std::endl;
#endif

    unsigned int b_max_fixed = parser.valueOf<unsigned int>("bmax-fixed");
    if( b_max_fixed == 0 ) {
        // Heuristics for the estimation of an optimal b_max
        if( insemination ) {
            b_max_fixed = daex::estimate_bmax_insemination( parser, pddl, pop, init.l_max() );
        } else {
            // if not insemination, incremental search strategy
            b_max_fixed  = daex::estimate_bmax_incremental<T>( 
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

# ifdef WITH_MPI
    /***********************
     * PARALLEL MULTISTART *
     **********************/

    int eval_master = eo::mpi::DEFAULT_MASTER;
    std::vector< std::vector<int> > evaluatorsAffectations;
    parallel::compute_roles(
            // in params
            with_multistart, multistart_workers, eval_workers,
            // in out params
            eval_master, evaluatorsAffectations );
# endif // WITH_MPI

    /**************
     * EVALUATION *
     **************/

#ifndef NDEBUG
    eo::log << eo::progress << "Creating evaluators...";
    eo::log.flush();
#endif

    // do_make_eval returns a pair: the evaluator instance
    // and a pointer on a func counter that may be null of we are in release mode
    std::pair< eoEvalFunc<T>&, eoEvalFuncCounter<T>* > eval_pair
        = daex::do_make_eval_op<T>(
                parser, state, init.l_max(), eval_count, b_max_in, b_max_last, plan_file, best_makespan, dump_sep, dump_file_count, metadata
                );
    eoEvalFunc<T>& eval = eval_pair.first;

#ifndef NDEBUG
    // in debug mode, we should have a func counter
    assert( eval_pair.second != NULL );
    eoEvalFuncCounter<T>& eval_counter = * eval_pair.second;

    eo::log << eo::progress << "OK" << std::endl;

    eo::log << eo::progress << "Evaluating the first population...";
    eo::log.flush();
#endif

    // Make pop eval function. Uses the do_make_parallel_eval function, which produces a simple eoPopLoopEval if the option
    // parallelize-loop is not set, and a MPI based parallelized version, otherwise.
#ifdef WITH_MPI
    unsigned int max_seconds = parser.valueOf<unsigned int>("max-seconds");
    eoPopEvalFunc<T>* p_pop_eval = parallel::do_make_parallel_eval(
            eval, plan_file, best_makespan, dump_file_count, dump_sep, metadata, max_seconds,
            /* multistart parameters */
            with_multistart,
            eval_master,
            (with_multistart && rank > 0 ) ? 
                evaluatorsAffectations[ eval_master-1 ]
                : evaluatorsAffectations[ 0 ]
    );
    eoPopEvalFunc<T>& pop_eval = *p_pop_eval;
# else
    eoPopLoopEval<T> pop_eval( eval );
# endif // WITH_MPI

# ifdef WITH_MPI
    /*
     * This affects the eval workers.
     * In single start mode, they are the processes which are other than master.
     * In multi start mode, they are all the processes whose ranks are higher than multistart_workers + 1.
     */
    if( ( ! with_multistart && rank != eo::mpi::DEFAULT_MASTER )
        || ( with_multistart && rank >= (int)multistart_workers + 1) )
    {
        // eval workers just perform evaluation
        eoPop<T> pop;
        pop_eval( pop, pop ); // Just one call is necessary, as ParallelApply is a MultiJob (see eo doc).

        timerStat.stop("dae_main");
        eo::mpi::Node::comm().send( eo::mpi::DEFAULT_MASTER, 0, timerStat );
        return 0;
    }

# endif // WITH_MPI
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
#ifdef DAE_MO
    moeoUnboundedArchive<T> archive;
#endif

    // STOPPING CRITERIA
#ifdef DAE_MO
    eoCombinedContinue<T> continuator = daex::do_make_continue_op<T>( parser, state, archive );
#else
    eoCombinedContinue<T> continuator = daex::do_make_continue_op<T>( parser, state );
#endif

    // Direct access to continuators are needed during restarts (see below)
    eoGenContinue<T> & maxgen
        = *( dynamic_cast< eoGenContinue<T>* >( continuator.at(0) ) );
#ifndef DAE_MO
    eoSteadyFitContinue<T> & steadyfit
        = *( dynamic_cast<eoSteadyFitContinue<T>* >( continuator.at(1) ) );
#endif

    // CHECKPOINTING
    eoCheckPoint<T> & checkpoint = daex::do_make_checkpoint_op( continuator, parser, state, pop
#ifndef NDEBUG
            , eval_counter
#endif
#ifdef DAE_MO
            , archive
#endif
            );

    // VARIATION
    daex::MutationDelGoal<T>* delgoal = new daex::MutationDelGoal<T>;
    eoGenOp<T> & variator = daex::do_make_variation_op<T>( parser, state, pddl, delgoal );

    // ALGORITHM
#ifdef DAE_MO // MULTI-OBJECTIVE
    moeoHypervolumeBinaryMetric<T::ObjectiveVector> metric(rho);
    moeoExpBinaryIndicatorBasedDualFitnessAssignment<T> assignment( metric, kappa );

    moeoIBEA<T> dae( checkpoint, pop_eval, variator, assignment );

#else // MONO-OBJECTIVE
    // Selection
    eoGeneralBreeder<T> & breeder = daex::do_make_breed_op<T>( parser, state, variator );

    // Replacement
    eoReplacement<T> & replacor = daex::do_make_replace_op<T>( parser, state );
    unsigned int offsprings = parser.valueOf<unsigned int>("offsprings");

    eoEasyEA<T> dae( checkpoint, eval, pop_eval, breeder, replacor, offsprings );
#endif

    /********************
     * MULTI-START RUNS *
     ********************/

#ifdef DAE_MO
    // the best solution to a multi-objective problem is a Pareto front (an archive)
    moeoUnboundedArchive<T> best = archive;
#else
    // best decomposition of all the runs, in case of multi-start
    // start at the best element of the init
    T best = pop.best_element();
#endif
    unsigned int run = 0;

    // evaluate an empty decomposition, for comparison with decomposed solutions
    T empty_decompo;
# ifdef WITH_MPI
    {
        eoPop<T> tempPop;
        tempPop.push_back( empty_decompo );
        pop_eval( tempPop, tempPop );
    }
# else // WITH_MPI
    eval( empty_decompo );
# endif // WITH_MPI

    /**
     * @brief Herb Sutter's trick to have a finally block in a try / catch section.
     *
     * By creating a FinallyBlock into the try section, the destructor will be called in every cases: this will be our
     * finally block.
     */
    class FinallyBlock
    {
        public:

        FinallyBlock(
# ifdef WITH_MPI
                eoPopEvalFunc<T> * _p_pop_eval,
# endif // WITH_MPI
                eoPop<T> & _pop,
#ifdef DAE_MO
                moeoUnboundedArchive<T> & _best,
#else
                T & _best,
#endif
                T & _empty_decompo
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
#ifdef DAE_MO
            best( pop ); // update the best archive
#else
            // push the best result, in case it was not in the last run
            // but produced by the init after the last search
            pop.push_back( best );
#endif

#ifndef NDEBUG
            // call the checkpoint, as if it was ending a generation
            // checkpoint( pop );
            eo::log << eo::progress << "... end of search" << std::endl;
#endif
#ifdef WITH_MPI
            // this part is done by both multistart workers and general master
            delete p_pop_eval;
            timerStat.stop("dae_main");

            // Only multistart workers will do this part
            if( eo::mpi::Node::comm().rank() != eo::mpi::DEFAULT_MASTER )
            {
                eo::mpi::Node::comm().send( eo::mpi::DEFAULT_MASTER, 0, timerStat );
                return;
            }

            // Only general master applies this part of catch section, until the end.
            std::ofstream fd("stats.log");
            std::ostream & ss = fd;

            // Computes statistics of self.
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

            // Receives statistics from other nodes.
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

#ifndef NDEBUG

#ifdef DAE_MO
            // ugly fix to output correctly formatted json
            // FIXME backport json serialization everywhere in paradisEO
            // std::cout << best << std::endl;
            std::cout << "[" << std::endl;
            // best is a kind of eoPop containing non-dominated solutions
            moeoArchive<T>::iterator idecompo=best.begin(), end=best.end();
            std::cout << *idecompo;// << std::endl;
            idecompo++;
            for( ; idecompo != end; ++idecompo ) {
                std::cout << "," << *idecompo;// << std::endl;
            }
            std::cout << "]" << std::endl;
#else
            pop.push_back( empty_decompo );
            // push the best result, in case it was not in the last run
            pop.push_back( best );
            //            pop_eval( pop, pop ); // FIXME normalement inutile
            // print_results( pop, time_start, run );
            std::cout << pop.best_element() << std::endl;
#endif
            eo::log << eo::progress << "The end" << std::endl;
#endif
        }

        private:

# ifdef WITH_MPI
        eoPopEvalFunc<T> * p_pop_eval;
# endif // WITH_MPI
        eoPop<T>& pop;
#ifdef DAE_MO
        moeoUnboundedArchive<T> & best;
#else
        T & best;
#endif
        T & empty_decompo;
    };

    try {
        FinallyBlock finallyBlock(
# ifdef WITH_MPI
            p_pop_eval,
# endif
            pop, best, empty_decompo );

# ifdef WITH_MPI
        if( with_multistart )
        {
            parallel::run_multistart(
                    multistart_workers,
                    maxruns,
                    maxgen,
                    steadyfit,
                    pop,
                    pop_eval,
                    dae,
                    plan_file,
                    best_makespan,
                    dump_file_count,
                    dump_sep,
                    metadata,
                    max_seconds);
        } else
# endif // WITH_MPI
        {
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

#ifdef DAE_MO
                archive( pop );
#else
                // remember the best of all runs
                T best_of_run = pop.best_element();

                // note: operator> is overloaded in EO, don't be afraid: we are minimizing
                if( best_of_run.fitness() > best.fitness() ) {
                    best = best_of_run;
                }
#endif

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
                pop = eoPop<T>( pop_size, init );

                eo::log << "[Master] After re init of population, evaluating population." << std::endl;

                // evaluate
                // eoPopLoopEval<T> pop_eval( eval ); // FIXME useful ??
                pop_eval( pop, pop );

                // reset run's continuator counters
                maxgen.totalGenerations( maxgens );
#ifndef DAE_MO
                steadyfit.totalGenerations( mingen, steadygen );
#endif
# ifdef WITH_MPI
                timerStat.stop("main_run");
# endif
            } // while
        } // if with multistart
    // } catch( std::exception const& e ) {
    } catch( eoMaxTimeException const& e ) {
#ifndef NDEBUG
            eo::log << eo::progress << "... premature end of search, current result:" << std::endl;
#endif
            eo::log << eo::progress << "Stop after exception : " << e.what() << std::endl;
        return 0;
    }


    return 0;
}

