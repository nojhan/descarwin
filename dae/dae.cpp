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

    // WALLOCK TIME COUNTER
    time_t time_start = std::time(NULL);

    
    // SYSTEM
#ifndef NDEBUG
    struct rlimit limit;
    getrlimit(RLIMIT_AS, &limit);
    eo::log << eo::logging << "Maximum size of the process virtual memory (soft,hard)=" << limit.rlim_cur << ", " << limit.rlim_max << std::endl;
    getrlimit(RLIMIT_DATA, &limit);
    eo::log << eo::logging << "Maximum size of the process   data segment (soft,hard)=" << limit.rlim_cur << ", " << limit.rlim_max << std::endl;

    /*
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


    // EO
    eoParser parser(argc, argv);
    make_verbose(parser);
    make_parallel(parser);
    
    eoState state;
    
    // PARAMETERS
    eo::log << eo::logging << "Parameters:" << std::endl;

    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "verbose" << eo::log.getLevelSelected() << std::endl;

    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "parallelize-loop" << eo::parallel.isEnabled() << std::endl;
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "parallelize-dynamic" << eo::parallel.isDynamic() << std::endl;
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "parallelize-prefix" << eo::parallel.prefix() << std::endl;
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "parallelize-nthreads" << eo::parallel.nthreads() << std::endl;


    // createParam (ValueType _defaultValue, std::string _longName, std::string _description, char _shortHand=0, std::string _section="", bool _required=false)
    std::string domain = parser.createParam( (std::string)"domain-zeno-time.pddl", "domain", "PDDL domain file", 'D', "Problem", true ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "domain" << domain << std::endl;

    std::string instance = parser.createParam( (std::string)"zeno10.pddl", "instance", "PDDL instance file", 'I', "Problem", true ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "instance" << instance << std::endl;


    // Initialization
    unsigned int pop_size = parser.createParam( (unsigned int)100, "popSize", "Population Size", 'P', "Evolution Engine").value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "pop_size" << pop_size << std::endl;

    eoValueParam<unsigned int> & param_seed = parser.createParam( (unsigned int)0, "seed", "Random number seed", 'S' );
    // if one want to initialize on current time
    if ( param_seed.value() == 0) {
        // change the parameter itself, that will be dumped in the status file
      //        param_seed.value( time(0) );
      param_seed.value()=time(0); // EO compatibility fixed by CC on 2010.12.24
    }
    unsigned int seed = param_seed.value();
    rng.reseed( seed );
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "seed" << seed << std::endl;

    unsigned int l_max_init_coef = parser.createParam( (unsigned int)2, "lmax-initcoef", 
            "l_max will be set to the size of the chrono partition * this coefficient", 'C', "Initialization" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "l_max_init_coef" << l_max_init_coef << std::endl;

    unsigned int l_min = parser.createParam( (unsigned int)1, "lmin", 
            "Minimum number of goals in a decomposition", 'l', "Initialization" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "l_min" << l_min << std::endl;

    // Evaluation
    unsigned int fitness_weight = parser.createParam( (unsigned int)10, "fitness-weight", 
            "Unknown weight in the feasible and unfeasible fitness computation", 'W', "Evaluation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "fitness_weight" << fitness_weight << std::endl;

    unsigned int fitness_penalty = parser.createParam( (unsigned int)2, "fitness-penalty", 
            "Penalty in the unfeasible fitnesses computation", 'w', "Evaluation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "fitness_penalty" << fitness_penalty << std::endl;

    unsigned int b_max_init = parser.createParam( (unsigned int)1e4, "bmax-init", "Number of allowed expanded nodes for the initial computation of b_max", 'B', "Evaluation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "b_max_init" << b_max_init << std::endl;

    /* nonsense with the incremental strategy
    double b_max_quantile = parser.createParam( (double)0.5, "bmax-quantile", 
            "Quantile to use for estimating b_max (in [0,1], 0.5=median)", 'Q', "Evaluation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "b_max_quantile" << b_max_quantile << std::endl;
    if( b_max_quantile < 0 || b_max_quantile > 1 ) {
        std::cout << "bmax-quantile must be a double in [0,1] (=" << b_max_quantile << ") type --help for usage." << std::endl;
        exit(1);
    }looser throw specifier for
    */
    unsigned int b_max_fixed = parser.createParam( (unsigned int)0, "bmax-fixed", "Fixed number of allowed expanded nodes. Overrides bmaxinit if != 0", 'b', "Evaluation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "b_max_fixed" << b_max_fixed << std::endl;

    double b_max_last_weight = parser.createParam( (double)3, "bmax-last-weight",
            "Weighting for the b_max used during the last search towards the end goal (must be strictly positive)", 'T', "Evaluation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "b_max_last_weight" << b_max_last_weight << std::endl;

    if( b_max_last_weight <= 0 ) {
        std::cout << "bmax-last-weight must be strictly positive (=" << b_max_last_weight << ") type --help for usage." << std::endl;
        exit(1);
    }

    double b_max_ratio = parser.createParam( 0.01, "bmax-ratio","Satisfying proportion of feasible individuals for the computation of b_max", 'J', "Evaluation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "b_max_ratio" << b_max_ratio << std::endl;

    double b_max_increase_coef = parser.createParam( (double)2, "bmax-increase-coef", "Multiplier increment for the computation of b_max", 'K', "Evaluation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "b_max_increase_coef" << b_max_increase_coef << std::endl;

    // Other

    std::string plan_file = parser.createParam( (std::string)"plan.soln", "plan-file", "Plan file backup", 'F', "Misc" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "plan-file" << plan_file << std::endl;

    // Stopping criterions
#ifndef SINGLE_EVAL_ITER_DUMP
    unsigned int max_seconds = parser.createParam( (unsigned int)0, "max-seconds", 
            "Maximum number of user seconds in CPU for the whole search, set it to 0 to deactivate (1800 = 30 minutes)", 'i', "Stopping criterions" ).value(); // 1800 seconds = 30 minutes
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "max_seconds" << max_seconds << std::endl;
#endif

    daex::do_make_variation_param( parser, pop_size);
    daex::do_make_checkpoint_param( parser );
    daex::do_make_replace_param( parser );

    daex::do_make_continue_param( parser );
    // Those parameters are needed during restarts (see below)
    unsigned int mingen = parser.value<unsigned int>("gen-min");
    unsigned int steadygen = parser.value<unsigned int>("gen-steady");
    unsigned int maxgens = parser.value<unsigned int>("gen-max");

    unsigned int maxruns = parser.createParam( (unsigned int)0, "runs-max", 
            "Maximum number of runs, if x==0: unlimited multi-starts, if x>1: will do <x> multi-start", 'r', "Stopping criterions" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "maxruns" << maxruns << std::endl;


    make_help( parser );

    /***********
     * PARSING *
     ***********/

    // PDDL

    // parse les pddl
#ifndef NDEBUG
    eo::log << eo::progress << "Load the instance..." << std::endl;
    eo::log.flush();
#endif
    
    daex::pddlLoad pddl( domain, instance, SOLVER_YAHSP, HEURISTIC_H1, eo::parallel.nthreads(), std::vector<std::string>());
   
#ifndef NDEBUG
    eo::log << eo::progress << "Load the instance...OK" << std::endl;
    eo::log << eo::progress << "Initialization...";
    eo::log.flush();
#endif

    /******************
     * INITIALIZATION *
     ******************/

    // l'initialisation se fait en fonction de la liste des dates au plus tot possibles (start time set)
    // Note : dans le init, l_max est réglé au double du nombre de dates dans la partition
    daex::Init<daex::Decomposition> init( pddl.chronoPartitionAtom(), l_max_init_coef, l_min );
    
#ifndef NDEBUG
    eo::log << eo::logging << std::endl;
    eo::log << eo::logging << "\tChrono partition size: " << pddl.chronoPartitionAtom().size() << std::endl;
    eo::log << eo::logging << "\tl_max: " << init.l_max() << std::endl;

    eo::log << eo::debug << "\tChrono partition dates(#atoms): ";
    for( daex::ChronoPartition::const_iterator it = pddl.chronoPartitionAtom().begin(), end = pddl.chronoPartitionAtom().end(); it != end; ++it ) {
         eo::log << eo::debug << it->first << "(" << it->second.size() << ") ";
    }
    eo::log << eo::debug << std::endl;
#endif

    // randomly generate the population with the init operator
    eoPop<daex::Decomposition> pop = eoPop<daex::Decomposition>( pop_size, init );

    /************************************
     * Incremental strategy to fix bmax 
     ************************************/
    
    daex::MutationDelGoal<daex::Decomposition> delgoal;
    
    unsigned int b_max_in=1, b_max_last=1;
    
#ifndef NDEBUG
    // used to pass the eval count through the several eoEvalFuncCounter evaluators
    unsigned int eval_count = 0;
#endif
    
    TimeVal best_makespan = MAXTIME;

#ifndef SINGLE_EVAL_ITER_DUMP
    std::string dump_sep = ".";
    unsigned int dump_file_count = 1;
    std::string metadata = "domain " + domain + "\n" + IPC_PLAN_COMMENT + "instance " + instance;
#endif

    // Preventive direct call to YAHSP
    daex::Decomposition empty_decompo;
    bool found = false;

    bool insemination = true;
    if( insemination ) {
       
        eo::log << eo::logging << "Evaluate a firt empty plan" << std::endl; 
        
        unsigned int return_code = cpt_basic_search();
        assert( return_code == PLAN_FOUND );
        eo::log << eo::debug << "Found a plan" << std::endl;

        eo::log << eo::logging << "Build Adam from the flat plan" << std::endl; 
        // get the flat plan from yahsp and build a complete decomposition with it
        Adam yahsp_adam = yahsp_create_adam( solution_plan );
#ifndef NDEBUG
        eo::log << eo::debug << "YAHSP Adam:" << std::endl;
        yahsp_print_adam( yahsp_adam );
#endif
        daex::Decomposition adam;
        for( unsigned int i=0; i < yahsp_adam.states_nb; ++i) {

            daex::Goal goal;
            TimeVal goal_time = 0;
            for( unsigned int j=0; j < yahsp_adam.states[i].fluents_nb; ++j ) {
                unsigned int common_id = yahsp_adam.states[i].fluents[j]->id;
                daex::Atom* atom = pddl.atoms()[common_id];
                goal.push_back( atom );
                goal_time = std::max( goal_time, atom->earliest_start_time() );
            }
            goal.earliest_start_time( goal_time );

            adam.push_back( goal );
        }
        eo::log << eo::debug << "Adam:" << std::endl << adam << std::endl;

        eo::log << eo::logging << "Create a population of Adam" << std::endl; 
        pop.clear(); // FIXME si insémination, ne pas faire l'init plus haut
        
        for( unsigned int i = 0; i < pop_size; ++i ) {
            pop.push_back( adam );
        }

        eo::log << eo::logging << "Try to del goals and to increase b_max" << std::endl; 
        // global parameters
        unsigned int bmax_iters = 10;
        
        daeYahspEval<daex::Decomposition> eval_yahsp( 
                init.l_max(), b_max_in, b_max_last, fitness_weight, fitness_penalty );


        // while the pop is largely feasible, try to del goals
        double feasibility_ratio = 1.0;
        while( feasibility_ratio > b_max_ratio )  {
            unsigned int feasibles = 0;
            for( unsigned int i = 0; i < pop_size; ++i ) {
                delgoal( pop[i] );
                eval_yahsp( pop[i] );
                if( pop[i].is_feasible() ) {
                    feasibles++;
                }
            }
            feasibility_ratio = static_cast<double>(feasibles) / pop_size;
            
            unsigned int iters = 0;
            while( feasibility_ratio < b_max_ratio && iters <= bmax_iters && b_max_in <= b_max_init )  {
                unsigned int feasibles = 0;

                for( unsigned int i = 0; i < pop_size; ++i ) {
                    eval_yahsp( pop[i] );
                    if( pop[i].is_feasible() ) {
                        feasibles++;
                    }
                }

                b_max_in= b_max_in * b_max_increase_coef;
                iters++;
                feasibility_ratio = static_cast<double>(feasibles) / pop_size;
            }
        } // while feasibility_ratio > b_max_ratio

        b_max_fixed = b_max_in;
        eo::log << eo::logging << "After insemination, b_max=" << b_max_in << std::endl;

    } else { // if not insemination
        if( b_max_fixed == 0 ) {
            unsigned int goodguys = 1;
            unsigned int popsize = pop.size();
            //        b_max_fixed = estimate_bmax_incremental( parser, pop );

#ifndef NDEBUG
            eo::log << eo::progress << "Apply an incremental computation strategy to fix bmax:" << std::endl;
#endif
            while( (((double)goodguys/(double)popsize) < b_max_ratio) && (b_max_in < b_max_init) ) {
                goodguys=0;
                b_max_last = static_cast<unsigned int>( std::floor( b_max_in * b_max_last_weight ) );

                daeYahspEval<daex::Decomposition> eval_yahsp( 
                        init.l_max(), b_max_in, b_max_last, fitness_weight, fitness_penalty );

                // in non multi-threaded version, use the plan dumper
                //#ifndef SINGLE_EVAL_ITER_DUMP
                daex::evalBestMakespanPlanDump eval_bestfile( 
                        eval_yahsp, plan_file, best_makespan, false, dump_sep, dump_file_count, metadata );

                // if we do not want MT, but we want debug, add more eval wrappers
#ifndef NDEBUG
                eoEvalFuncCounter<daex::Decomposition> eval_counter( eval_bestfile, "Eval.\t" );
                eval_counter.value( eval_count );
                eoPopLoopEval<daex::Decomposition> eval_y( eval_counter );

                // else, only the plan dumper
#else // ifdef NDEBUG
                eoPopLoopEval<daex::Decomposition> eval_y( eval_bestfile );
#endif

                // if we want to compile a multi-threaded version with OpenMP, 
                // we only want the basic evaluator, not the other wrappers, 
                // even the one that dump plans
                //#else // ifdef SINGLE_EVAL_ITER_DUMP
                //                eoPopLoopEval<daex::Decomposition> eval_y( eval_yahsp );
                //#endif
                eval_y( pop, pop );

#ifndef NDEBUG
                eoBestFitnessStat<daex::Decomposition> best_statTEST("Best");
                best_statTEST(pop);
                //        eo::log << eo::logging << "\tbest_fitness " << timeValToString(best_statTEST.value());
                eo::log << eo::logging << "\tbest_fitness " << best_statTEST.value();
#endif

                for (size_t i = 0; i < popsize; ++i) {
                    // unfeasible individuals are invalidated in order to be re-evaluated 
                    // with a larger bmax at the next iteration but we keep the good guys.
                    if (pop[i].is_feasible()) goodguys++;
                    else pop[i].invalidate();
                }
                // If no individual haven't yet been found, then try a direct call to YAHSP (i.e. the empty decomposition evaluation)
                if ((goodguys == 0) && (!found)) {
                    empty_decompo.invalidate();
                    //#ifdef SINGLE_EVAL_ITER_DUMP
                    //                eval_yahsp( empty_decompo );
                    //#else
#ifndef NDEBUG
                    eval_counter(empty_decompo);
#else
                    eval_bestfile(empty_decompo);
#endif
                    //#endif
                    if (empty_decompo.is_feasible()){
                        found = true;
                        /*
                           std::ofstream of(plan_file.c_str());
                           of << empty_decompo.plan();
                           of.close();
                           */
                    }
                } // if ! goodguys && ! found

                //#ifdef SINGLE_EVAL_ITER_DUMP
                //            best_makespan = pop.best_element().plan_copy().makespan();
                //#else
                best_makespan = eval_bestfile.best();
                dump_file_count = eval_bestfile.file_count();
                //#endif

#ifndef NDEBUG
                eo::log << eo::logging << "\tb_max_in "   << b_max_in << "\tfeasible_ratio " <<  ((double)goodguys/(double)popsize);
                eo::log << "\tbest_makespan " << best_makespan;
                if(found) {
                    eo::log << "\tfeasible empty decomposition";
                }
                eo::log << std::endl;
                eval_count = eval_counter.value();
#endif
                b_max_fixed = b_max_in;
                b_max_in = (unsigned int)ceil(b_max_in*b_max_increase_coef);
            } // while
        } // if b_max_fixed == 0 
    } // if insemination

    b_max_in = b_max_fixed;
    b_max_last = static_cast<unsigned int>( std::floor( b_max_in * b_max_last_weight ) );

    daeYahspEval<daex::Decomposition> eval_yahsp( init.l_max(), b_max_in, b_max_last, fitness_weight, fitness_penalty );
    eoPopLoopEval<daex::Decomposition> eval_y( eval_yahsp );
    eval_y( pop, pop );

#ifndef NDEBUG
    eo::log << eo::logging << std::endl << "\tb_max for intermediate goals, b_max_in: "   << b_max_in   << std::endl;
    eo::log << eo::logging              << "\tb_max for        final goal,  b_max_last: " << b_max_last << std::endl;
    eo::log << eo::progress << "OK" << std::endl;

    eo::log << eo::progress << "Creating evaluators...";
    eo::log.flush();
#endif

    // nested evals:
    eoEvalFunc<daex::Decomposition> * p_eval;

//#ifndef SINGLE_EVAL_ITER_DUMP
    // dump the best solution found so far in a file
    daex::evalBestMakespanPlanDump eval_bestfile( eval_yahsp, plan_file, best_makespan, false, dump_sep, dump_file_count, metadata );

#ifndef NDEBUG
    // counter, for checkpointing
    eoEvalFuncCounter<daex::Decomposition> eval_counter( eval_bestfile, "Eval.\t" );
    eval_counter.value( eval_count );
#endif
//#endif

    // if we do not want to add a time limit, do not add an EvalTime
//#ifdef SINGLE_EVAL_ITER_DUMP
//        p_eval = & eval_yahsp;
//#else // ifndef SINGLE_EVAL_ITER_DUMP
        if( max_seconds == 0 ) {
#ifndef NDEBUG
        p_eval = & eval_counter;
#else
        p_eval = & eval_bestfile;
#endif
    } else {
        // an eval that raises an exception if maxtime is reached
        /* eoEvalTimeThrowException<daex::Decomposition> * p_eval_maxtime 
                = new eoEvalTimeThrowException<daex::Decomposition>( eval_counter, max_seconds ); */
        eoEvalUserTimeThrowException<daex::Decomposition> * p_eval_maxtime
#ifndef NDEBUG
            = new eoEvalUserTimeThrowException<daex::Decomposition>( eval_counter, max_seconds );
#else
            = new eoEvalUserTimeThrowException<daex::Decomposition>( eval_bestfile, max_seconds );
#endif
        p_eval = p_eval_maxtime;
    }
//#endif // SINGLE_EVAL_ITER_DUMP

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
    eoGeneralBreeder<daex::Decomposition> & breed = daex::do_make_variation_op<daex::Decomposition>( parser, state, pddl, &delgoal );
    

    // REPLACEMENT
    eoReplacement<daex::Decomposition> & replacor = daex::do_make_replace_op<daex::Decomposition>( parser, state );
     unsigned int offsprings = parser.value<unsigned int>("offsprings");

    // ALGORITHM
    eoEasyEA<daex::Decomposition> dae( checkpoint, *p_eval, breed, replacor, offsprings );

#ifndef NDEBUG
    eo::log << eo::progress << "OK" << std::endl;
    eo::log << eo::progress << "Note: dual fitness is printed as two numbers: a value followed by a boolean (0=unfeasible, 1=feasible)" << std::endl;
    eo::log.flush();
    eo::log << eo::debug << "Legend: \n\t- already valid, no eval\n\tx plan not found\n\t* plan found\n\ta add atom\n\tA add goal\n\td delete atom\n\tD delete goal\n\tC crossover" << std::endl;
#endif

    // best decomposition of all the runs, in case of multi-start
    // start at the best element of the init
    daex::Decomposition best = pop.best_element();
    unsigned int run = 1;

    try { 

        while( true ) {
#ifndef NDEBUG
            eo::log << eo::progress << "Start the " << run << "th run..." << std::endl;

            // call the checkpoint (log and stats output) on the pop from the init
            checkpoint( pop );
#endif

            // start a search
            dae( pop );

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
                break;
            }

            // Once the bmax is known, there is no need to re-estimate it,
            // thus we re-init ater the first search, because the pop has already been created before,
            // when we were trying to estimate the b_max.
            pop = eoPop<daex::Decomposition>( pop_size, init );
            
            // evaluate
            eoPopLoopEval<daex::Decomposition> pop_eval( *p_eval );
            pop_eval( pop, pop );

            // reset run's continuator counters
            steadyfit.totalGenerations( mingen, steadygen );
            maxgen.totalGenerations( maxgens );
        }


    } catch( std::exception& e ) {
#ifndef NDEBUG
        eo::log << eo::warnings << "STOP: " << e.what() << std::endl;
        eo::log << eo::progress << "... premature end of search, current result:" << std::endl;
#endif


        // push the best result, in case it was not in the last run
        pop.push_back( best );

#ifndef NDEBUG
        // call the checkpoint, as if it was ending a generation
        checkpoint( pop );
#endif

        pop.push_back( empty_decompo );
        print_results( pop, time_start, run );
        return 0;
    }

#ifndef NDEBUG
    eo::log << eo::progress << "... end of search" << std::endl;
#endif


    pop.push_back( empty_decompo );
    // push the best result, in case it was not in the last run
    pop.push_back( best );
    print_results( pop, time_start, run );

    return 0;
}

