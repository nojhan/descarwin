#include<iostream>

#include <eo>
#include <utils/eoParserLogger.h>
#include <utils/eoLogger.h>

#include "daex.h"
#include "evaluation/yahsp.h"

int main ( int argc, char* argv[] )
{
    eoParserLogger parser(argc, argv);
    make_verbose(parser);

    std::string domain = parser.createParam( (std::string)"domain-zeno-time.pddl", "domain", "PDDL domain file", 'D', "Problem", true ).value();

    std::string instance = parser.createParam( (std::string)"zeno10.pddl", "instance", "PDDL instance file", 'I', "Problem", true ).value();
    
    //bool is_sequential = parser.createParam( (bool)false, "sequential", "Is the problem a sequential one?", 'q', "Problem", true ).value();


    std::vector<std::string> solver_args;
    solver_args.push_back("-verbosity");
    solver_args.push_back("0");

    daex::pddlLoad pddl( domain, instance, SOLVER_YAHSP, HEURISTIC_H1, /*is_sequential,*/ solver_args );

    std::clog << "# Domain: " << domain << std::endl;
    std::clog << "# Instance: " << instance << std::endl;
    std::clog << "# Solver: " << SOLVER_YAHSP << std::endl;
    std::clog << "# Heuristic:" << HEURISTIC_H1 << std::endl;
    //std::clog << "# Sequential: " << is_sequential << std::endl;
    std::clog << "# Chrono partition of " << pddl.chronoPartitionAtom().size() << " dates:" << std::endl;

    for( daex::ChronoPartition::const_iterator it = pddl.chronoPartitionAtom().begin(), end = pddl.chronoPartitionAtom().end(); it != end; ++it ) {
        std::cout << it->first << " ";
        std::cout.flush();
    }

    std::cout << std::endl;
}
