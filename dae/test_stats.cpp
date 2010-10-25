#include<iostream>
#include <iomanip>

#include <eo>
#include <utils/eoParserLogger.h>
#include <utils/eoLogger.h>
#include <do/make_pop.h>
#include <utils/eoFeasibleRatioStat.h>

#include "daex.h"
#include "evaluation/yahsp.h"

#define LOG_FILL ' '
#define FORMAT_LEFT_FILL_WIDTH(width) "\t" << std::left << std::setfill(LOG_FILL) << std::setw(width) 
#define FORMAT_LEFT_FILL_W_PARAM FORMAT_LEFT_FILL_WIDTH(20)

int main ( int argc, char* argv[] )
{
    eoParserLogger parser(argc, argv);
    make_verbose(parser);

    std::string domain = parser.createParam( (std::string)"domain-zeno-time.pddl", "domain", "PDDL domain file", 'D', "Problem", true ).value();
    std::string instance = parser.createParam( (std::string)"zeno10.pddl", "instance", "PDDL instance file", 'I', "Problem", true ).value();
    bool is_sequential = parser.createParam( (bool)false, "sequential", "Is the problem a sequential one?", 'q', "Problem", true ).value();
    unsigned int runs = parser.createParam( (unsigned int)1, "runs", 
            "Number of distributions to produce", 'r' ).value();
    unsigned int l_max_init_coef = parser.createParam( (unsigned int)2, "lmax-initcoef", 
            "l_max will be set to the size of the chrono partition * this coefficient", 'C', "Initialization" ).value();
    unsigned int l_min = parser.createParam( (unsigned int)1, "lmin", 
            "Minimum number of goals in a decomposition", 'l', "Initialization" ).value();
    unsigned int b_max_in = parser.createParam( (unsigned int)10, "bmax-in", 
            "Number of allowed expanded nodes for intermediate goals", 'b', "Evaluation" ).value();
    double b_max_last_weight = parser.createParam( (double)3, "bmax-last-weight",
            "Weighting for the b_max used during the last search towards the end goal (must be strictly positive)", 'T', "Evaluation" ).value();
    unsigned int fitness_weight = parser.createParam( (unsigned int)10, "fitness-weight", 
            "Unknown weight in the feasible fitness computation", 'W', "Evaluation" ).value();
    unsigned int fitness_penalty = parser.createParam( (unsigned int)2, "fitness-penalty", 
            "Penalty in the unfeasible fitnesses computation", 'w', "Evaluation" ).value();

    unsigned int seed = parser.createParam((unsigned int)0, "seed", "Random seed", 'R', "General").value();
    rng.reseed( seed );

    std::vector<std::string> solver_args;
    solver_args.push_back("-verbosity");
    solver_args.push_back("0");

    daex::pddlLoad pddl( domain, instance, SOLVER_YAHSP, HEURISTIC_H1, is_sequential, solver_args );
    bindDaeYahsp( pddl );
   
    std::clog << "# Domain: " << domain << std::endl;
    std::clog << "# Instance: " << instance << std::endl;
    std::clog << "# Solver: " << SOLVER_YAHSP << std::endl;
    std::clog << "# Heuristic: " << HEURISTIC_H1 << std::endl;
    std::clog << "# Sequential: " << is_sequential << std::endl;
    
    eoState state;
    daex::Init init( pddl.chronoPartitionAtom(), l_max_init_coef, l_min );
    eoPop<daex::Decomposition> pop = do_make_pop( parser, state, init );
    make_help( parser );

    // An different evaluator for the first iteration
    // permits to compute the b_max
    // (b_max = 10 000 by default)
    // uses the same value for b_max_in and b_max_last
    daeYahspEval eval_yahsp( init.l_max(), b_max_in, static_cast<unsigned int>(b_max_in*b_max_last_weight), fitness_weight, fitness_penalty );

    // start the eval on the first random pop
    eoPopLoopEval<daex::Decomposition> eval( eval_yahsp );

    eval(pop,pop);

    struct rusage usage;
    getrusage(RUSAGE_SELF,&usage);

    std::clog << std::endl;
    std::clog << "OS statistics:" << std::endl;
    std::cout << FORMAT_LEFT_FILL_WIDTH(30) << "user time used"               << usage.ru_utime.tv_sec << "." << usage.ru_utime.tv_usec << std::endl;
    std::cout << FORMAT_LEFT_FILL_WIDTH(30) << "system time used"             << usage.ru_stime.tv_sec << "." << usage.ru_stime.tv_usec << std::endl;
    std::cout << FORMAT_LEFT_FILL_WIDTH(30) << "page reclaims * page size"   << usage.ru_minflt * getpagesize() << std::endl;
    
    
    std::clog << "Statistics on the populaion after an init: " << std::endl;

    eoBestFitnessStat<daex::Decomposition> best_stat;
    eoNthElementFitnessStat<daex::Decomposition> median_stat( pop.size() / 2, "Median" ); 
    eoInterquartileRangeStat<daex::Decomposition> iqr_stat( std::make_pair(0.0,false) );
    eoFeasibleRatioStat<daex::Decomposition> feasible_stat( "F.Ratio" );


    best_stat(pop);
    std::cout <<  FORMAT_LEFT_FILL_WIDTH(30) << "Best" << best_stat.value() << std::endl;
    
    std::vector<const daex::Decomposition* > _pop;
    _pop.reserve(pop.size());
    for( unsigned int i = 0, e = pop.size(); i<e; ++i ) {
        _pop.push_back( & pop[i] );
    }
    median_stat(_pop);
    std::cout <<  FORMAT_LEFT_FILL_WIDTH(30) << "Median" << median_stat.value() << std::endl;
    
    iqr_stat(pop);
    std::cout <<  FORMAT_LEFT_FILL_WIDTH(30) << "IQR" << iqr_stat.value() << std::endl;
    
    feasible_stat(pop);
    std::cout <<  FORMAT_LEFT_FILL_WIDTH(30) << "Feasible ratio" << feasible_stat.value() << std::endl;
}

