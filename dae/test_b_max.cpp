#include<iostream>

#include <eo>
#include <do/make_pop.h>

#include "daex.h"
#include "evaluation/yahsp.h"

int main ( int argc, char* argv[] )
{
    eoParser parser(argc, argv);
    make_verbose(parser);

    std::string domain = parser.createParam( (std::string)"domain-zeno-time.pddl", "domain", "PDDL domain file", 'D', "Problem", true ).value();
    std::string instance = parser.createParam( (std::string)"zeno10.pddl", "instance", "PDDL instance file", 'I', "Problem", true ).value();
    //bool is_sequential = parser.createParam( (bool)false, "sequential", "Is the problem a sequential one?", 'q', "Problem", true ).value();
    unsigned int runs = parser.createParam( (unsigned int)1, "runs", 
            "Number of distributions to produce", 'r' ).value();
    unsigned int l_max_init_coef = parser.createParam( (unsigned int)2, "lmax-initcoef", 
            "l_max will be set to the size of the chrono partition * this coefficient", 'C', "Initialization" ).value();
    unsigned int l_min = parser.createParam( (unsigned int)1, "lmin", 
            "Minimum number of goals in a decomposition", 'l', "Initialization" ).value();
    unsigned int b_max_init = parser.createParam( (unsigned int)1e4, "bmax-init", 
            "Number of allowed expanded nodes for the initial computation of b_max", 'B', "Evaluation" ).value();
    unsigned int fitness_weight = parser.createParam( (unsigned int)10, "fitness-weight", 
            "Unknown weight in the feasible fitness computation", 'W', "Evaluation" ).value();

    unsigned int seed = parser.createParam((unsigned int)0, "seed", "Random seed", 'R', "General").value();
    rng.reseed( seed );

    std::vector<std::string> solver_args;
    solver_args.push_back("-verbosity");
    solver_args.push_back("0");

    daex::pddlLoad pddl( domain, instance, SOLVER_YAHSP, HEURISTIC_H1, eo::parallel.nthreads(), solver_args );

    std::clog << "# Domain: " << domain << std::endl;
    std::clog << "# Instance: " << instance << std::endl;
    std::clog << "# Solver: " << SOLVER_YAHSP << std::endl;
    std::clog << "# Heuristic: " << HEURISTIC_H1 << std::endl;
    //std::clog << "# Sequential: " << is_sequential << std::endl;
    std::clog << "# Nodes number distribution during evaluations at init: " << std::endl;
    
    eoState state;
    daex::Init<> init( pddl.chronoPartitionAtom(), l_max_init_coef, l_min );
    eoPop<daex::Decomposition> pop = do_make_pop( parser, state, init );
    make_help( parser );

    // An different evaluator for the first iteration
    // permits to compute the b_max
    // (b_max = 10 000 by default)
    // uses the same value for b_max_in and b_max_last
    daeYahspEvalInit<> eval_yahsp_init( pop.size(), init.l_max(), b_max_init, b_max_init, fitness_weight );

    // start the eval on the first random pop
    eoPopLoopEval<daex::Decomposition> eval_init( eval_yahsp_init );

    // set the log level so as to output the used node numbers
    eo::log << eo::setlevel(eo::logging);

    unsigned int b_max_in;
    for( unsigned int i=0; i < runs; ++i ) {

        // estimate the b_max from the eval results
        std::clog << "#nodes: ";
        eval_init( pop, pop );
        b_max_in = eval_yahsp_init.estimate_b_max();
        std::clog << std::endl;

        std::clog << std::endl << "b_max=" << b_max_in << std::endl;

        // force re-evaluation for several runs 
        for( unsigned int j=0; j < pop.size(); ++j ) {
            //pop[j].invalidate();
            init( pop[j] );
        }
        
    }

}

