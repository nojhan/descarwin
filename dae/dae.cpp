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
//#include <do/make_checkpoint.h>
#include <ga.h>
#include <utils/eoFeasibleRatioStat.h>

#include "daex.h"
#include "evaluation/cpt-yahsp.h"
#include "evaluation/yahsp.h"

#define LOG_FILL ' '
#define FORMAT_LEFT_FILL_WIDTH(width) "\t" << std::left << std::setfill(LOG_FILL) << std::setw(width) 
#define FORMAT_LEFT_FILL_W_PARAM FORMAT_LEFT_FILL_WIDTH(22)

// MODIFS MS START
// at the moment, in utils/make_help.cpp
// this should become some eoUtils.cpp with corresponding eoUtils.h
bool testDirRes(std::string _dirName, bool _erase);
// MODIFS MS END

/*
#ifndef NDEBUG
inline void LOG_LOCATION( eo::Levels level )
{
    eo::log << level << "in " << __FILE__ << ":" << __LINE__ << " " << __PRETTY_FUNCTION__ << std::endl; 
}
#endif
*/

void print_results( eoPop<daex::Decomposition> pop, time_t time_start, int run )
{
    (void) time_start;
#ifndef NDEBUG
    struct rusage usage;
    getrusage(RUSAGE_SELF,&usage);

    eo::log << eo::warnings << "OS statistics:" << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "user time used"               << usage.ru_utime.tv_sec << "." << usage.ru_utime.tv_usec << " s." << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "system time used"             << usage.ru_stime.tv_sec << "." << usage.ru_stime.tv_usec << " s." << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "page reclaims * page size"   << usage.ru_minflt * getpagesize() << " b." 
                                         << " (~ " << std::setprecision(2) << usage.ru_minflt * getpagesize() / 1048576 << " Mb.)" << std::endl; 
    // 1 giga-byte = 1 073 741 824 bytes
    // 1 mega-byte = 1 048 576 octets 
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "page reclaims"                << usage.ru_minflt               << std::endl;
    eo::log << eo::warnings << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "page faults"                  << usage.ru_majflt               << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "maximum resident set size"    << usage.ru_maxrss               << " b." << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "integral shared memory size"  << usage.ru_ixrss                << " b." << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "integral unshared data size"  << usage.ru_idrss                << " b." << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "integral unshared stack size" << usage.ru_isrss                << " b." << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "swaps"                        << usage.ru_nswap                << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "block input operations"       << usage.ru_inblock              << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "block output operations"      << usage.ru_oublock              << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "messages sent"                << usage.ru_msgsnd               << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "messages received"            << usage.ru_msgrcv               << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "signals received"             << usage.ru_nsignals             << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "voluntary context switches"   << usage.ru_nvcsw                << std::endl;
    eo::log << eo::warnings << FORMAT_LEFT_FILL_WIDTH(30) << "involuntary context switches" << usage.ru_nivcsw               << std::endl;
    
    //double subsolver_time = 0;
    unsigned int subsolver_steps = 0;
    for( unsigned int i=0; i < pop.size(); i++ ) {
        for( unsigned int j=0; j < pop[i].subplans().size(); j++ ) {
             //subsolver_time += pop[i].subplan(j).time_subsolver();
             subsolver_steps += pop[i].subplan(j).search_steps();
        }
    }

    eo::log << eo::progress << "DAEx sub-solver steps " << subsolver_steps << std::endl;
    //eo::log << eo::progress << "; DAEx sub-solver time " << subsolver_time << " s (u-CPU)" << std::endl;
    eo::log << eo::progress << "DAEx user time " << usage.ru_utime.tv_sec << "." << usage.ru_utime.tv_usec << " (seconds of user time in CPU)" << std::endl;
    eo::log << eo::progress << "DAEx wallclock time " << std::difftime( std::time(NULL), time_start )  << " (seconds)" << std::endl;
#endif

    std::cout << "Number of restarts " << run << std::endl;

    // the pop being unsorted, sorting it before getting the first is more efficient
    // than using best_element (that uses std::max_element)
    pop.sort();

#ifndef NDEBUG
    std::cout << IPC_PLAN_COMMENT << pop.front() << std::endl;
#endif
    std::cout << pop.front().plan() << std::endl;
}


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

    /*
    bool is_sequential = parser.createParam( (bool)false, "sequential", "Is the problem a sequential one?", 'q', "Problem", true ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "is_sequential" << is_sequential << std::endl;
    */

    // un total de 20 paramètres

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

    // Selection
    unsigned int toursize = parser.createParam( (unsigned int)5, "tournament", 
            "Size of the deterministic tournament for the selection", 't', "Selection" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "toursize" << toursize << std::endl;

    unsigned int offsprings = parser.createParam( (unsigned int)pop_size*7, "offsprings", 
            "Number of offsprings to produces", 'f', "Selection" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "offsprings" << offsprings << std::endl;


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
    }
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
    unsigned int maxtry_candidate = 0; // deactivated by default: should try every candidates
    unsigned int maxtry_mutex = 0;     // deactivated by default: should try every candidates
    /*
    unsigned int maxtry_candidate = parser.createParam( (unsigned int)11, "maxtry-candidate", 
            "Maximum number of atoms to try when searching for a candidate in the changeAtom mutation", 'y', "Misc" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "maxtry_candidate" << maxtry_candidate << std::endl;


    unsigned int maxtry_mutex = parser.createParam( (unsigned int)11, "maxtry-mutex", 
            "Maximum number of atoms to try when searching for mutexes in the changeAtom mutation", 'z', "Misc" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "maxtry_mutex" << maxtry_mutex << std::endl;
    */

    std::string plan_file = parser.createParam( (std::string)"plan.soln", "plan-file", "Plan file backup", 'F', "Misc" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "plan-file" << plan_file << std::endl;

    // Variation
    unsigned int radius = parser.createParam( (unsigned int)2, "radius", 
            "Number of neighbour goals to consider for the addGoal mutation", 'R', "Variation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "radius" << radius << std::endl;

    double proba_change = parser.createParam( (double)0.8, "proba-change", 
            "Probability to change an atom for the changeAtom mutation", 'c', "Variation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "proba_change" << proba_change << std::endl;

    double proba_del_atom = parser.createParam( (double)0.8, "proba-del-atom", 
            "Average probability to delete an atom for the delAtom mutation", 'd', "Variation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "proba_del_atom" << proba_del_atom << std::endl;

    double w_delgoal = parser.createParam( (double)1, "w-delgoal", 
            "Relative weight defining the probability to call the delGoal mutation", 'a', "Variation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "w_delgoal" << w_delgoal << std::endl;

    double w_addgoal = parser.createParam( (double)3, "w-addgoal", 
            "Relative weight defining the probability to call the addGoal mutation", 'A', "Variation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "w_addgoal" << w_addgoal << std::endl;

    double w_delatom = parser.createParam( (double)1, "w-delatom", 
            "Relative weight defining the probability to call the delAtom mutation", 'g', "Variation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "w_delatom" << w_delatom << std::endl;

    double w_addatom = parser.createParam( (double)1, "w-addatom", 
            "Relative weight defining the probability to call the addAtom mutation", 'G', "Variation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "w_addatom" << w_addatom << std::endl;

    double proba_cross = parser.createParam( (double)0.2, "proba-cross", 
            "Probability to apply a cross-over", 'c', "Variation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "proba_cross" << proba_cross << std::endl;

    double proba_mut = parser.createParam( (double)0.8, "proba-mut", 
            "Probability to apply one of the mutation", 'm', "Variation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "proba_mut" << proba_mut << std::endl;

    // Stopping criterions
#ifndef SINGLE_EVAL_ITER_DUMP
    unsigned int max_seconds = parser.createParam( (unsigned int)0, "max-seconds", 
            "Maximum number of user seconds in CPU for the whole search, set it to 0 to deactivate (1800 = 30 minutes)", 'i', "Stopping criterions" ).value(); // 1800 seconds = 30 minutes
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "max_seconds" << max_seconds << std::endl;
#endif

    unsigned int mingen = parser.createParam( (unsigned int)10, "gen-min", 
            "Minimum number of iterations", 'n', "Stopping criterions" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "mingen" << mingen << std::endl;

    unsigned int steadygen = parser.createParam( (unsigned int)50, "gen-steady", 
            "Number of iterations without improvement", 's', "Stopping criterions" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "steadygen" << steadygen << std::endl;

    unsigned int maxgens = parser.createParam( (unsigned int)1000, "gen-max", 
            "Maximum number of iterations", 'x', "Stopping criterions" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "maxgens" << maxgens << std::endl;

    unsigned int maxruns = parser.createParam( (unsigned int)0, "runs-max", 
            "Maximum number of runs, if x==0: unlimited multi-starts, if x>1: will do <x> multi-start", 'r', "Stopping criterions" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "maxruns" << maxruns << std::endl;

    // MODIFS MS START
#ifndef NDEBUG
    eoValueParam<std::string>& dirNameParam =  parser.createParam(std::string("Res"), "resDir", "Directory to store DISK outputs", '\0', "Output - Disk");

    // shoudl we empty it if exists
    eoValueParam<bool>& eraseParam = parser.createParam(true, "eraseDir", "erase files in dirName if any", '\0', "Output - Disk");

    eoValueParam<unsigned>& saveFrequencyParam = parser.createParam(unsigned(0), "saveFrequency", "Save every F generation (0 = only final state, absent = never)", '\0', "Persistence" );
#endif

    std::string plusOrComma =  parser.createParam(std::string("Comma"), "plusOrComma", "Plus (parents+offspring) or Comma (only offspring) for replacement", '\0', "Evolution Engine").value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "plusOrComma" << plusOrComma << std::endl;

    unsigned replaceTourSize = parser.createParam(unsigned(1), "replaceTourSize", "Size of Replacement Tournament (1->deterministic (hum, pas logique ;-(", '\0', "Evolution Engine" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "replaceTourSize" << replaceTourSize << std::endl;

    bool removeDuplicates = parser.createParam(false, "removeDuplicates", "Does not allow duplicates in replacement (if possible)", '\0', "Evolution Engine").value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "removeDuplicates" << removeDuplicates << std::endl;

    bool weakElitism = parser.createParam(true, "weakElitism", "Weak Elitism in replacement", '\0', "Evolution Engine").value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "weakElitism" << weakElitism << std::endl;
    // MODIFS MS END

    make_help( parser );

    /***********
     * PARSING *
     ***********/

    // PDDL

    // parse les pddl
    // FIXME ATTENTION : il y a une option cachée dans l'init qui précise à yahsp si on est en temporel ou en séquentiel, il faut la régler correctement en fonction du problème visé
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
    
    unsigned int b_max_in=1, b_max_last, goodguys=0, popsize = pop.size();
    
#ifndef NDEBUG
    // used to pass the eval count through the several eoEvalFuncCounter evaluators
    unsigned int eval_count = 0;
#endif
    
    TimeVal best_makespan = INT_MAX; // FIXME UINT_MAX returns -1, why?

#ifndef SINGLE_EVAL_ITER_DUMP
    std::string dump_sep = ".";
    unsigned int dump_file_count = 1;
    std::string metadata = "domain " + domain + "\n" + IPC_PLAN_COMMENT + "instance " + instance;
#endif

    // Preventive direct call to YAHSP
    daex::Decomposition empty_decompo;
    bool found = false;

    if( b_max_fixed == 0 ) {
#ifndef NDEBUG
        eo::log << eo::progress << "Apply an incremental computation strategy to fix bmax:" << std::endl;
#endif
	//	while( (((double)goodguys/(double)popsize) < b_max_ratio) && (!found) && (b_max_in < b_max_init) ) {
	//12.1 while( (goodguys == 0) && (!found) && (b_max_in < b_max_init) ) {
	while( (((double)goodguys/(double)popsize) < b_max_ratio) && (b_max_in < b_max_init) ) {
	    goodguys=0;
            b_max_last = static_cast<unsigned int>( std::floor( b_max_in * b_max_last_weight ) );

            daeYahspEval<daex::Decomposition> eval_yahsp( init.l_max(), b_max_in, b_max_last, fitness_weight, fitness_penalty );

// in non multi-threaded version, use the plan dumper
//#ifndef SINGLE_EVAL_ITER_DUMP
                daex::evalBestMakespanPlanDump eval_bestfile( eval_yahsp, plan_file, best_makespan, false, dump_sep, dump_file_count, metadata );

// if we do not want MT, but we want debug, add more eval wrappers
#ifndef NDEBUG
                    eoEvalFuncCounter<daex::Decomposition> eval_counter( eval_bestfile, "Eval.\t" );
                    eval_counter.value( eval_count );
                    eoPopLoopEval<daex::Decomposition> eval_y( eval_counter );

// else, only the plan dumper
#else // ifdef NDEBUG
                    eoPopLoopEval<daex::Decomposition> eval_y( eval_bestfile );
#endif

// if we want to compile a multi-threaded version with OpenMP, we only want the basic evaluator, not the other wrappers, even the one that dump plans
//#else // ifdef SINGLE_EVAL_ITER_DUMP
//                eoPopLoopEval<daex::Decomposition> eval_y( eval_yahsp );
//#endif
            eval_y( pop, pop );

#ifndef NDEBUG
	    eoBestFitnessStat<daex::Decomposition> best_statTEST("Best");
	    best_statTEST(pop);
	    //	    eo::log << eo::logging << "\tbest_fitness " << timeValToString(best_statTEST.value());
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
//12.1	if (((double)goodguys/(double)popsize) < b_max_ratio) {
//12.1	b_max_fixed= b_max_fixed * 2;
//12.1	}
    } // if b_max_fixed == 0 

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
    
    /*
  ____________________________________________
 | Checkpoint                                 |
 |    __________________________              |
 | <-| Combined Continue        |             |
 |   |    _____________________ |             |
 |   | <-| Steady Fit Continue ||             |
 |   |   |_____________________||             |
 |   |    ______________        |             |
 |   | <-| Gen Continue |       |             |
 |   |   |______________|       |             |
 |   |__________________________|             |
 |                       ___________________  |
 |                   <--| Best fitness stat | |
 |    _________________ |                   | |
 | <-| Stdout monitor  ||                   | |
 |   |               <--|___________________| |
 |   |                 |                      | ______________
 |   |               <-------------------------| Eval Counter |
 |   |                 |                      ||______________|
 |   |_________________|                      |
 |____________________________________________|
    */

    // continuators == stopping criteria
    eoSteadyFitContinue<daex::Decomposition> steadyfit( mingen, steadygen );
    eoGenContinue<daex::Decomposition> maxgen( maxgens );

    // combine the continuators
    eoCombinedContinue<daex::Decomposition> continuator( steadyfit );
    continuator.add(maxgen);

    // attach a continuator to the checkpoint
    // the checkpoint is here to get some stat during the search
    eoCheckPoint<daex::Decomposition> checkpoint( continuator );

#ifdef SINGLE_EVAL_ITER_DUMP
    daex::BestMakespanStat stat_makespan("BestMakespan");
    daex::BestPlanStat stat_plan("BestPlan");
#endif

#ifndef NDEBUG
    // get the best plan only if it improve the fitness
    // note: fitness is different from the makespan!
    //eoBestPlanImprovedStat<daex::Decomposition> best_plan( worst_fitness, "Best improved plan");
    // at each generation
    //checkpoint.add( best_plan );
    
    // display the stats on std::cout
    // ostream & out, bool _verbose=true, std::string _delim = "\t", unsigned int _width=20, char _fill=' ' 
    eoOStreamMonitor clog_monitor( std::clog, "\t", 10, ' '); 


    // get best fitness
    // for us, has the form "fitness feasibility" (e.g. "722367 1")
    eoBestFitnessStat<daex::Decomposition> best_stat("Best");

    //eoInterquartileRangeStat<daex::Decomposition> iqr_stat( std::make_pair(0.0,false), "IQR" );
    eoInterquartileRangeStat<daex::Decomposition> iqr_f( std::make_pair(0.0,false), "IQR_f" );
    eoInterquartileRangeStat<daex::Decomposition> iqr_uf( std::make_pair(0.0,false), "IQR_uf" );
    eoDualStatSwitch<daex::Decomposition,eoInterquartileRangeStat<daex::Decomposition> > dual_iqr( iqr_f, iqr_uf, "\t" );
 
    // TODO implement "better" nth_element stats with different interpolations (linear and second moment?)
    eoNthElementFitnessStat<daex::Decomposition> median_stat( pop.size() / 2, "Median" ); 
    /*
    eoNthElementFitnessStat<daex::Decomposition> median_f( pop.size() / 2, "Median_f" ); 
    eoNthElementFitnessStat<daex::Decomposition> median_uf( pop.size() / 2, "Median_uf" ); 
    eoDualStatSwitch<daex::Decomposition,eoNthElementFitnessStat<daex::Decomposition> > dual_median( median_f, median_uf, "\t/\t" );
    */

    eoFeasibleRatioStat<daex::Decomposition> feasible_stat( "F.Ratio" );

    eoAverageSizeStat<daex::Decomposition> asize_stat( "Av.Size" );

    if( eo::log.getLevelSelected() >= eo::progress ) {

        // compute stas at each generation
        checkpoint.add( best_stat );
        checkpoint.add( feasible_stat );
        checkpoint.add( asize_stat );
        checkpoint.add( median_stat );
        //checkpoint.add( dual_median );
        //checkpoint.add( iqr_stat );
        checkpoint.add( dual_iqr );
#ifdef SINGLE_EVAL_ITER_DUMP
        checkpoint.add( stat_makespan );
        checkpoint.add( stat_plan );
        clog_monitor.add( stat_makespan );
        clog_monitor.add( stat_plan );
#else
        clog_monitor.add( eval_counter );
#endif
        clog_monitor.add( best_stat );
        clog_monitor.add( asize_stat );
        clog_monitor.add( feasible_stat );
        clog_monitor.add( median_stat );
        //clog_monitor.add( dual_median );
        //clog_monitor.add( iqr_stat );
        clog_monitor.add( dual_iqr );
        
        // the checkpoint should call the monitor at every generation
        checkpoint.add( clog_monitor );
    }

#ifdef SINGLE_EVAL_ITER_DUMP
    // Note: commented here because superseeded by the eoEvalBestPlanFileDump
    // append the plans in a file
    // std::string _filename, std::string _delim = " ", bool _keep = false, bool _header=false, bool _overwrite = false
    eoFileMonitor file_monitor( plan_file, "\n", false, false, true);
    file_monitor.add( stat_plan );
    checkpoint.add( file_monitor );
#endif
    

    // MODIFS MS START 
    // pour plus d'output (recopiés de do/make_checkpoint)
    // un state, pour sauver l'état courant
    eoState state;
    state.registerObject(parser);
    state.registerObject(pop);
    state.registerObject(eo::rng);

    bool dirOK = false;
    
    if (parser.isItThere(saveFrequencyParam)) {
      if (! dirOK ) {
	dirOK = testDirRes(dirNameParam.value(), eraseParam.value());
    eo::log << eo::progress << "TESTRESDIR" << std::endl;
      }
    }

    unsigned freq = (saveFrequencyParam.value()>0 ? saveFrequencyParam.value() : UINT_MAX );
    std::string stmp = dirNameParam.value() + "/generations";
    eoCountedStateSaver stateSaver1 (freq, state, stmp);
    checkpoint.add(stateSaver1);
    // MODIFS MS END
    
#endif // NDEBUG

    // SELECTION
    // TODO cet opérateur, fait soit un tri de la pop (true), soit un shuffle (false), idéalement, on ne voudrait ni l'un ni l'autre, car on parcours tout, peu importe l'ordre
    
    // JACK the article indicate that tere is no selection, but the article use a deterministic tournament

    eoSelectOne<daex::Decomposition> * p_selectone;
    if ( toursize == 1 ) {
    // L'article indique qu'il n'y a pas de sélection, on aurait alors ça :
      p_selectone = (eoSelectOne<daex::Decomposition> *) ( new eoSequentialSelect<daex::Decomposition> ( true ) );
    }
    else {
    /// MAIS le code utilise un tournoi déterministe, on a donc ça :
      p_selectone = (eoSelectOne<daex::Decomposition> *) ( new eoDetTournamentSelect<daex::Decomposition> ( toursize ) );
    }
    
    // VARIATION

    // mutations
    daex::MutationDelGoal<daex::Decomposition> delgoal;
//    daex::MutationDelOneAtom delatom;
    daex::MutationDelAtom<daex::Decomposition> delatom( proba_del_atom );
    // partition, radius, l_max
    daex::MutationAddGoal<daex::Decomposition> addgoal( pddl.chronoPartitionAtom(), radius /*, init.l_max()*/ );
    // partition, proba_change, proba_add, maxtry_search_candidate, maxtry_search_mutex 
    // (maxtry à 0 pour essayer tous les atomes)
    //    daex::MutationAddAtom addatom( pddl.chronoPartitionAtom(), 0.8, 0.5, 11, 11 );
    //    daex::MutationAddAtom addatom( pddl.chronoPartitionAtom(), proba_change, maxtry_candidate, maxtry_mutex );
    daex::MutationChangeAddAtom<daex::Decomposition> addatom( pddl.chronoPartitionAtom(), proba_change, maxtry_candidate, maxtry_mutex );

    // call one of operator, chosen randomly
    eoPropCombinedMonOp<daex::Decomposition> mutator( delgoal, w_delgoal );
    mutator.add( delatom, w_delatom );
    mutator.add( addgoal, w_addgoal );
    mutator.add( addatom, w_addatom );

    // crossover
    // JACK in the crossover, filter out the right half of the decomposition from goals that have a greater date than the date at which we cut
    daex::CrossOverTimeFilterHalf<daex::Decomposition> crossover;

    // first call the crossover with the given proba, then call the mutator with the given proba
    // FIXME most of the mutation use the last_reached information, what if they are called after a crossover?
    eoSGAGenOp<daex::Decomposition> variator( crossover, proba_cross, mutator, proba_mut);

    // selector, variator, rate (for selection), interpret_as_rate
    eoGeneralBreeder<daex::Decomposition> breed( *p_selectone, variator, (double)offsprings, false );
    // FIXME tester si on veut 700% ou 700


    // REPLACEMENT
    
    // JACK : L'article indique qu'on fait un remplacement en tournoi déterministe et qu'il n'y a pas d'élistisme, on aurait alors ça :
    //eoSSGADetTournamentReplacement<daex::Decomposition> replace(5);
    
    // MODIFS MS START
    // plus de paramétrage du remplacement
    eoReplacement<daex::Decomposition> * pt_replace;
    // MAIS le code utilise un remplacement intégral des parents par le meilleur des parents+enfants, avec élitisme faible, on a donc 
    // Note MS: le commentaire était incohérent entre Plus et Comma ???
    
    // Use the eoMergeReduce construct
    
    // Merge: either merge (Plus strategy, parents + offspring) or only keep offspring (Comma)
    // eoMerge<daex::Decomposition> * pt_merge;
    // if (plusOrComma == "Comma") {
    //   pt_merge = (eoMerge<daex::Decomposition> *) (new eoNoElitism<daex::Decomposition>);
    // } else { // Plus
    //   pt_merge = (eoMerge<daex::Decomposition> *) (new eoPlus<daex::Decomposition>);
    // }
    // // Reduce: either truncate (deterministic) or tournament
    // eoReduce<daex::Decomposition> * pt_reduce;
    // if (replaceTourSize == 1) {
    //   pt_reduce = (eoReduce<daex::Decomposition> *) (new eoTruncate<daex::Decomposition>);
    // } else {
    //   pt_reduce = (eoReduce<daex::Decomposition> *) (new eoDetTournamentTruncate<daex::Decomposition> ( replaceTourSize ));
    // }
    // // the full MergeReduce object
    // eoMergeReduce<daex::Decomposition> mergeReduce (*pt_merge, *pt_reduce);

    eoEPReplacement<daex::Decomposition> mergeReduce(replaceTourSize);

    // Now the weak elitism
    if (weakElitism) {
	pt_replace = (eoReplacement<daex::Decomposition> *) (new eoWeakElitistReplacement<daex::Decomposition> ( mergeReduce ) );
      } else {
	pt_replace = &mergeReduce;
      }

    // ALGORITHM
    eoEasyEA<daex::Decomposition> dae( checkpoint, *p_eval, breed, (*pt_replace), offsprings );

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

