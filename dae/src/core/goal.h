
#ifndef __GOAL_H__
#define __GOAL_H__

#include <iterator>
#include <iostream>

#include <eo>

#include "atom.h"

namespace daex {

struct SolverConfig {
    std::string name;
    unsigned int b_max;
    unsigned int b_max_last;
};


class Goal : public std::list<Atom*>
{
public:

    //! The empty solver strinq indicates to use YAHSP by default
    Goal( TimeVal t, 
          std::string default_solver = "", unsigned int default_b_max = 10, unsigned int default_b_max_last = 30 
        ) : _earliest_start_time( t )
    {
        add_solver( default_solver, default_b_max, default_b_max_last );
    }


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


    void add_solver( std::string name, unsigned int b_max, unsigned int b_max_last )
    {
        SolverConfig solver;
        solver.name = name;
        solver.b_max = b_max;
        solver.b_max_last = b_max_last;

        add_solver( solver );
    }


    void add_solver( SolverConfig solver )
    {
        _solvers.push_back( solver );
    }

protected:

    //! Earliest start time among all atoms
    TimeVal _earliest_start_time;

    //! Candidates solver to be used to evaluate this goal
    std::vector< SolverConfig > _solvers;
};

} // namespace daex

#endif // __GOAL_H__

