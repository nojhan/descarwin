
#ifndef __DAEX_MUTATION_ADDGOAL_H__
#define __DAEX_MUTATION_ADDGOAL_H__

#include <eo>

#include "utils/pddl_load.h"
#include "core/decomposition.h"
#include "utils/utils.h"

namespace daex {


class MutationAddGoal: public eoMonOp<Decomposition>
{
public:
    MutationAddGoal(
            const ChronoPartition & times,
            unsigned int radius = 1/*,
            unsigned int l_max = 20*/
    ) : _times(times), _radius( radius )/*, _l_max(l_max)*/ {}

    bool operator()( Decomposition & decompo );

protected:

    //! Construct a random goal between the given dates
    // Atoms used are those with their earliest start time a the chosen date
    Goal random_goal(  TimeVal t1, TimeVal t2 );

    const ChronoPartition & _times;

    //! Number of neighbour earliest start date to consider when adding atoms
    unsigned int _radius;

    //! Maximal size of a decomposition
    /** If the decomposition is already at the max size, the mutation don't add any new goal
     */
    //unsigned int _l_max;
};


} // namespace daex

#endif // __DAEX_MUTATION_ADDGOAL_H__
