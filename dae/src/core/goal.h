#ifndef __GOAL_H__
#define __GOAL_H__

#include <iterator>
#include <iostream>

#include <eo>

#include "utils/json/Json.h"
#include "utils/eoJsonUtils.h"
#include "atom.h"

namespace daex {

class Goal : public std::list<Atom*>, public eoserial::Persistent, public eoPersistent
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

    /**
     * @brief Specific push algorithm used by makeArray, for pointers.
     */
    template< class T>
    struct PointerPushAlgorithm : public eoserial::PushAlgorithm<T>
    {
        void operator()( eoserial::Array & array, const T& obj )
        {
            // as obj is a pointer, just pushes the pointer
            array.push_back( obj );
        }
    };

    eoserial::Object* pack() const
    {
        // begin with list members
        eoserial::Array* members = eoserial::makeArray
            < std::list<Atom*>, PointerPushAlgorithm >
            ( *this );

        // continues with self
        eoserial::Object* obj = new eoserial::Object;
        obj->add( "start_time", eoserial::make(_earliest_start_time) );
        obj->add( "members", members );
        return obj;
    }

    /**
     * @brief Specific UnpackAlgorithm, used by unpackArray, to create objects before
     * handling them.
     */
    template <class C>
    struct UnpackNewAlgorithm : public eoserial::Array::BaseAlgorithm<C>
    {
        void operator()( const eoserial::Array& array, unsigned int i, C & container ) const
        {
            Atom* atom = new Atom;
            eoserial::unpackObject( array, i, *atom );
            container.push_back( atom );
        }
    };

    void unpack( const eoserial::Object* obj )
    {
        // begin with list members
        eoserial::unpackArray< std::list<Atom*>, UnpackNewAlgorithm >
            ( *obj, "members", *this);
        // continues with self
        eoserial::unpack( *obj, "start_time", _earliest_start_time );
    }

    void printOn(std::ostream& out) const
    {
        eoserial::printOn( *this, out );
    }

    void readFrom(std::istream& _is)
    {
        eoserial::readFrom( *this, _is );
    }
};

} // namespace daex

#endif // __GOAL_H__

