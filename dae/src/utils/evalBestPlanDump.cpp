
#include "evalBestPlanDump.h"

# ifdef WITH_MPI
#include <mpi/eoMpi.h>
# endif // WITH_mpi

namespace daex {

void evalBestMakespanPlanDump::call( Decomposition & eo )
{
# ifdef WITH_MPI
    if( eo::mpi::Node::comm().rank() == eo::mpi::DEFAULT_MASTER )
    {
# endif
        if( eo.plan().makespan() < _best ) {
#pragma omp critical
            _best = eo.plan().makespan();
            dump( eo );
        }// if better
# ifdef WITH_MPI
    }
# endif
}

void evalBestFitnessPlanDump::call( Decomposition & eo )
{
    // Note: in EO, maximizing by default, later overloaded for minimizing
  // OMP DIRTY - first comparison is unprotected, in order to be non-blocking
    if( eo.fitness() > _best ) {
#pragma omp critical
        if( eo.fitness() > _best ) {
            _best = eo.fitness();
            dump( eo );
        }
    } // if better
}

} // namespace daex

