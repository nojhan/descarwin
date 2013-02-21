
#ifndef __GOAL_H__
#define __GOAL_H__

#include <iterator>
#include <iostream>

#include <eo>

#include "atom.h"

namespace daex {

class Goal : public std::list<Atom*>
{
public:

    Goal( TimeVal t ) : _earliest_start_time( t ) {}

    Goal() : _earliest_start_time( 0 ) {}

    TimeVal earliest_start_time(  ) const
    {
        return _earliest_start_time;
    }

    void earliest_start_time(TimeVal d)
    {
      _earliest_start_time = d;
    }

    friend std::ostream& operator<<( std::ostream & out, const Goal & goal )
    {
    out << "(Goal[" << goal.size() << "]: ";

    if( goal.size() != 0 ) {

        std::list<Atom*>::const_iterator it = goal.begin();

        out << *(*it);

        it++;

        while( it != goal.end() ) {
            out << " " << *(*it);

            it++;
        }
    }

    out << ")";

    return out;
};

    Goal::iterator iter_at( unsigned int i );

protected:

    //! Earliest start time among all atoms
    TimeVal _earliest_start_time;
};


//! A goal that embed a search strategy, to orient a multi-objective search toward makespan or cost.
class GoalMO : public Goal
{
public:

    GoalMO( TimeVal t, bool strategy_makespan ) : Goal(t), _strategy_makespan(strategy_makespan) {}
    GoalMO() : Goal(), _strategy_makespan(true) {}

    bool strategy_makespan() { return _strategy_makespan; }
    void strategy_makespan( bool weight ) { _strategy_makespan = weight; }

protected:
    //! If true, use a search strategy that favors the makespan over the cost
    bool _strategy_makespan;

};


} // namespace daex

#endif // __GOAL_H__

