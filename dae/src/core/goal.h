
#ifndef __GOAL_H__
#define __GOAL_H__

#include <iterator>
#include <iostream>

#include <eo>

#ifdef WITH_MPI
#include <boost/serialization/list.hpp>
#endif // WITH_MPI

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

#ifdef WITH_MPI
    // Mandatory for boost serialisation
	friend class boost::serialization::access;

    /**
     * Serializes the goal into a boost archive (useful for boost::mpi)
     */
	template <class Archive>
	void serialize( Archive & ar, const unsigned int version )
	{
		// First, serializes parent part
		ar & boost::serialization::base_object< std::list< Atom* > >( *this );
		// then specific members
		ar & _earliest_start_time;
	}
#endif // WITH_MPI

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

} // namespace daex

#endif // __GOAL_H__

