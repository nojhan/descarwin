#ifndef __GOAL_H__
#define __GOAL_H__

#include <iterator>
#include <iostream>

#include <eo>

#include "utils/eoJsonUtils.h"
#include "atom.h"

namespace daex {

class Goal : public std::list<Atom>, public eoserial::Persistent, public eoPersistent
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

            std::list<Atom>::const_iterator it = goal.begin();

            out << *it;

            it++;

            while( it != goal.end() ) {
                out << " " << *it;

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

    template< class T >
    struct IndexPushAlgorithm : public eoserial::PushAlgorithm<T>
    {
        void operator()( eoserial::Array & array, const T& obj )
        {
            array.push_back( eoserial::make( obj.fluentIndex() ) );
        }
    };

    eoserial::Object* pack() const
    {
        // begin with list atoms
        /*
        eoserial::Array* atoms = eoserial::makeArray
            < std::list<Atom>, PointerPushAlgorithm >
            ( *this );
        */

        // FIXME enregistrer la liste des fluent index plutôt que les atom*
        eoserial::Array* atoms = eoserial::makeArray
            < std::list<Atom>, IndexPushAlgorithm >
            ( *this );

        // continues with self
        eoserial::Object* obj = new eoserial::Object;
        obj->add( "start_time", eoserial::make(_earliest_start_time) );
        obj->add( "atoms", atoms );
        return obj;
    }

protected:
    /**
     * Static pointer to "global" vector of atoms (the ones contained in the object
     * pddl), used to retrieve atoms on deserialization.
     */
    static const std::vector< daex::Atom > * _atoms;

public:
    /**
     * Static setter to above described pointer.
     */
    static void atoms( const std::vector<daex::Atom> * a )
    {
        _atoms = a;
    }

protected:
    template< class T >
    struct UnpackByIndex : public eoserial::Array::BaseAlgorithm< T >
    {
        void operator()( const eoserial::Array& array, unsigned int i, T & container ) const
        {
            int atomIndex;
            eoserial::unpack( array, i, atomIndex );
            container.push_back( _atoms->at( atomIndex ) );
        }
    };

public:
    void unpack( const eoserial::Object* obj )
    {
        // begin with list members
        clear();
        eoserial::unpackArray< std::list<Atom>, UnpackByIndex >
            ( *obj, "members", *this );
        
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

