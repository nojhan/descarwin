#ifndef __GOAL_H__
#define __GOAL_H__

#include <iterator>
#include <iostream>

#include <eo>

#ifdef WITH_MPI
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#endif // WITH_MPI

#include "utils/json/Json.h"
#include "utils/eoJsonUtils.h"
#include "atom.h"

namespace daex {

class Goal : public std::list<Atom*>, public json::Serializable, public eoPersistent
{
public:

    Goal( TimeVal t ) : _earliest_start_time( t ) {}

    // used by JSON serialization
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

        (void) version; // to avoid compilation warning
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

public:

    json::Object* toJson() const
    {
        // begin with list members
        json::Array* members = new json::Array;
        for( std::list<Atom*>::const_iterator it = this->begin(),
                end = this->end();
            it != end;
            ++it)
        {
            members->push_back( *it );
        }
        // continues with self
        json::Object* obj = new json::Object;
        obj->addPair( "start_time", json::String::make(_earliest_start_time) );
        obj->addPair( "members", members );
        return obj;
    }

    void fromJson( const json::Object* obj )
    {
        // begin with list members
        const json::Array* members = obj->getArray( "members" );
        for (unsigned int i = 0, end = members->size();
                i < end;
                ++i)
        {
            Atom* atom = new Atom;
            *atom = members->getObject<Atom>( i );
            this->push_back( atom );
        }
        // continues with self
        _earliest_start_time = obj->get<TimeVal>( "start_time" );
    }

    void printOn(std::ostream& out) const
    {
        json::printOn( this, out );
    }

    void readFrom(std::istream& _is)
    {
        json::readFrom( this, _is );
    }
};

} // namespace daex

#endif // __GOAL_H__

