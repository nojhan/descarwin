
#include <sys/time.h>
#include <sys/resource.h>

#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <cfloat>

#include <eo>
#include <moeo>
#include <ga.h>
#include <utils/eoFeasibleRatioStat.h>

#include "daex.h"
#include "evaluation/cpt-yahsp.h"
#include "evaluation/yahsp.h"
#include "core/decomposition_mo.h"
#include "variation/strategy_mo.h"

#ifdef WITH_MPI
#include <mpi/eoMpi.h>

#include "do/make_parallel_eval_dae.h"
#include "do/make_parallel_multistart_dae.h"
#endif // WITH_MPI

#include <utils/eoTimer.h>

void print_strategies( daex::DecompositionMO decompo )
{
    for( daex::DecompositionMO::iterator igoal=decompo.begin(), end=decompo.end(); igoal!=end; ++igoal) {
        std::clog << igoal->strategy() << " ";
    }
    std::clog << std::endl;
}

int main ( int argc, char* argv[] )
{
    daex::DecompositionMO indiv;
    indiv.invalidate();
    for( unsigned int i=0; i<50; ++i ) {
        indiv.push_back( daex::DecompositionMO::AtomType() );
    }

    std::clog << "random" << std::endl;
    daex::StrategyRandom<daex::DecompositionMO> sr; sr( indiv );
    print_strategies( indiv );

    std::clog << "flip goal" << std::endl;
    daex::StrategyFlipGoal<daex::DecompositionMO> srg( 0.5 ); srg(indiv);
    print_strategies( indiv );

    std::clog << "flip decomposition" << std::endl;
    daex::StrategyFlipDecomposition<daex::DecompositionMO> srd( 0.5 ); srd(indiv);
    print_strategies( indiv );

    std::clog << "fixed" << std::endl;
    daex::StrategyFixed<daex::DecompositionMO> sf( daex::Strategies::makespan_max ); sf(indiv);
    print_strategies( indiv );


}

