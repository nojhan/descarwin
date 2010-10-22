#ifndef __DAEX_MUTATION_DELGOAL_H__
#define __DAEX_MUTATION_DELGOAL_H__

#include <eo>

#include "core/decomposition.h"

namespace daex {

class MutationDelGoal: public eoMonOp<Decomposition>
{
public:
    bool operator()( Decomposition & decompo );
};

} // namespace daex

#endif // __DAEX_MUTATION_DELGOAL_H__
