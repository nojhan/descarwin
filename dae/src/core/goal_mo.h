#ifndef __GOAL_MO_H__
#define __GOAL_MO_H__

#include "goal.h"
#include "strategies.h"

namespace daex {

//! A goal that embed a search strategy, to orient a multi-objective search toward makespan or cost.
class GoalMO : public Goal
{
public:

    GoalMO( TimeVal t, Strategies::Type strategy ) : Goal(t), _strategy(strategy) {}
    GoalMO() : Goal(), _strategy( Strategies::makespan_add ) {}

    Strategies::Type strategy() { return _strategy; }

    void strategy( Strategies::Type strategy ) { _strategy = strategy; }

protected:
    //! If true, use a search strategy that favors the makespan over the cost
    Strategies::Type _strategy;

};

} // namespace daex

#endif // __GOAL_MO_H__

