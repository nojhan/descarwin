
#ifndef __ATOM_H__
#define __ATOM_H__

#include <string>
#include <vector>
#include <iterator>
#include <list>
#include <cassert>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <gmpxx.h>
//extern "C" {
#include <src/cpt.h>
#include <src/structs.h>
#include <src/branching.h>
#include <src/problem.h>
//}

#include "utils/pddl.h"
#include "utils/eoJsonUtils.h"

// Definitions used in cpt-yahsp can break STL.
// FIXME These undef are inelegant, another solution should be used
// (eg : rename all cpt-yahsp definitions, or not use STL, or...)
#include "undef_yahsp.h"

#include <eo>

extern Fluent** fluents;

namespace daex
{

class Atom : public eoPersistent, public eoserial::Persistent
{

public:
    //! Constructeur prédicats + objets + date au plus tot + pointeur vers structure interne YAHSP
    // Atom( TimeVal start_time, Fluent* fluent ) : _fluent(fluent), _earliest_start_time(start_time)
    Atom( TimeVal start_time, unsigned int fluentIndex ) : _fluentIndex(fluentIndex), _earliest_start_time(start_time)
    {}

    // Empty ctor used for JSON serialization
    Atom( ) : _fluentIndex(0), _earliest_start_time(0) {}

    //! Accesseurs
    TimeVal                     earliest_start_time() const { return _earliest_start_time; }
    Fluent *                    fluent()              const { return fluents[ _fluentIndex ]; }
    // Fluent* fluent() const { return _fluent; }
    unsigned int                fluentIndex()         const { return _fluentIndex; }

protected:
    unsigned int                _fluentIndex;
    // Fluent* _fluent;
    TimeVal                     _earliest_start_time;

public:
    friend std::ostream& operator<<( std::ostream& out, const Atom & atom )
    {
        atom.printOn( out );
        return out;
    }

    friend bool operator==( const Atom& a1, const Atom& a2 )
    {
#ifndef NDEBUG
        if( a1.fluentIndex() == a2.fluentIndex() ) {
            assert( a1.earliest_start_time() == a2.earliest_start_time() );
            return true;
        } else {
            return false;
        }
#else
        return ( a1.fluentIndex() == a2.fluentIndex() );
#endif
    }

    friend bool operator!=( const Atom& a1, const Atom& a2 )
    {
        return !(a1==a2);
    }

    void printOn(std::ostream& out) const
    {
        eoserial::printOn( *this, out );
    }

    void readFrom(std::istream& _is)
    {
        eoserial::readFrom( *this, _is );
    }

    eoserial::Object* pack() const
    {
        eoserial::Object* obj = new eoserial::Object;
        obj->add( "start_time", eoserial::make(_earliest_start_time) );
        obj->add( "fluent_index", eoserial::make(_fluentIndex) );
        return obj;
    }

    void unpack( const eoserial::Object* json )
    {
        eoserial::unpack( *json, "start_time", _earliest_start_time );
        eoserial::unpack( *json, "fluent_index", _fluentIndex );
    }
};

} // namespace daex

#endif

