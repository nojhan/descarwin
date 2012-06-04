
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
#include "utils/json/Json.h"
#include "utils/eoJsonUtils.h"

// Definitions used in cpt-yahsp can break Boost or STL.
// FIXME These undef are inelegant, another solution should be used
// (eg : rename all cpt-yahsp definitions, or not use Boost, or...)
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
    Atom( ) {}

    //! Accesseurs
    TimeVal                     earliest_start_time() const { return _earliest_start_time; }
    Fluent *                    fluent()              const { return fluents[ _fluentIndex ]; }

protected:
    unsigned int 				_fluentIndex;
    TimeVal                     _earliest_start_time;

public:
    friend std::ostream& operator<<( std::ostream& out, const Atom & atom )
    {
        atom.printOn( out );
        return out;
    }

    void printOn(std::ostream& out) const
    {
        eoserial::printOn( this, out );
    }

    void readFrom(std::istream& _is)
    {
        eoserial::readFrom( this, _is );
    }

    eoserial::Object* pack() const
    {
        eoserial::Object* obj = new eoserial::Object;
        obj->addPair( "start_time", eoserial::String::make(_earliest_start_time) );
        obj->addPair( "fluent_index", eoserial::String::make(_fluentIndex) );
        return obj;
    }

    void unpack( const eoserial::Object* json )
    {
        json->unpack( "start_time", _earliest_start_time );
        json->unpack( "fluent_index", _fluentIndex );
    }
};

} // namespace daex

#endif

