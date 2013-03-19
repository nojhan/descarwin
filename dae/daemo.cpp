
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
#include "evaluation/yahsp_mo.h"
#include "core/decomposition_mo.h"
#include "variation/strategy_mo.h"

#ifdef WITH_MPI
#include <mpi/eoMpi.h>

#include "do/make_parallel_eval_dae.h"
#include "do/make_parallel_multistart_dae.h"
#endif // WITH_MPI

#include <utils/eoTimer.h>

int main ( int argc, char* argv[] )
{
    daex::DecompositionMO indiv;

    // FIXME segfault on empty decomposition!!!
    for( unsigned int i=0; i<50; ++i ) {
        indiv.push_back( daex::DecompositionMO::AtomType() );
    }

    daemoYahspEvalAdd<daex::DecompositionMO> eval_add( 1 );
    eval_add( indiv );

    daemoYahspEvalMax<daex::DecompositionMO> eval_max( 1 );
    eval_max( indiv );
}

