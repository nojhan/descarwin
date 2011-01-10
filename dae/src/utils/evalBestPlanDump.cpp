
#include "evalBestPlanDump.h"

namespace daex {

void evalBestMakespanPlanDump::call( Decomposition & eo )
{
    if( eo.plan().yahsp_plan().makespan < _best ) {
        _best = eo.plan().yahsp_plan().makespan;
        dump( eo );
    } // if better
}

} // namespace daex

