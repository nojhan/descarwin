
#ifndef __DECOMPOSITION_H__
#define __DECOMPOSITION_H__

#include <iterator>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <functional>

#include <eo>

#include "decomposition.h"
#include "goal.h"
#include "plan.h"

namespace daex
{

//! A decomposition is a list of Goal objects, and we are trying to minimize a scalar fitness (e.g. time or number of actions)
class Decomposition : public std::list<Goal>, public EO< eoMinimizingDualFitness >
{
public:

    //! After a modification of the decomposition, it needs to be re-evaluated
    //! Variation operator should use this method to indicate it
    void invalidate();

    //! At instanciation, a decomposition does not have any plan
    /*!
     * Note: and is not feasible, @see eoDualFitness
     */
    //Decomposition() : _plan_global(NULL) {}


    Plan plan_copy() const
    {
        return _plan_global;
    }

    Plan & plan()
    {
        return _plan_global;
    }

    Plan & subplan(unsigned int i)
    {
        return _plans_sub[i];
    }

    std::vector<Plan> & subplans()
    {
        return _plans_sub;
    }

    Plan & last_subplan()
    {
        return *(_plans_sub.end() - 1 );
    }

    unsigned int subplans_size() const
    {
        return _plans_sub.size();
    }

    //! If no goal has been reached, return -1, else return the index of the goal
    int last_reached() const
    {
        return static_cast<int>( subplans_size() ) - 1;
    }

    //! If one has reached no goal or the first one, return 0, else return the index of the goal, if the end state has been reached, return the index of the last goal in the decomposition
    unsigned int last_reached_safe() const
    {
        int last_reached = static_cast<int>( subplans_size() ) - 1;

        if( last_reached < 0 ) {
            last_reached = 0;

        } else if( last_reached >= this->size() ) {
            last_reached = this->size() - 1;
        }

        return static_cast<unsigned int>( last_reached );
    }

    /*
    Decomposition::iterator iter_last_reached()
    {
        return iter_at( subplans_size() - 1 );
    }
    */

    void plan_global( Plan p );

    void plans_sub_add( Plan p );

    void plans_sub_reset();

    /*virtual*/ void printOn( std::ostream & out ) const;

    Decomposition::iterator iter_at( unsigned int i );

    //virtual ~Decomposition() {}
    

protected:

    //! Plan global compressé
    Plan _plan_global;

    //! Vecteur des sous-plans
    std::vector< Plan > _plans_sub;

}; // class Decomposition


//! Print a decomposition in a simple format
/** On a single line, print the date and the number of atoms of each goal, i.e.:
 *    Decomposition (size): goal(#atoms) ...
 *  e.g.:
 *    Decompositon (3): 2(5) 12(45) 123(4)
 */
void simplePrint( std::ostream & out, Decomposition & decompo );


} // namespace daex

#endif

