
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

class Atom : public eoPersistent, public json::Serializable
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
        json::printOn( this, out );
    }

    void readFrom(std::istream& _is)
    {
        json::readFrom( this, _is );
    }

    json::Object* toJson() const
    {
        json::Object* obj = new json::Object;
        obj->addPair( "start_time", json::String::make(_earliest_start_time) );
        obj->addPair( "fluent_index", json::String::make(_fluentIndex) );
        return obj;
    }

    void fromJson( const json::Object* json )
    {
        _earliest_start_time = json->get<TimeVal>( "start_time" );
        _fluentIndex = json->get<unsigned int>( "fluent_index" );
    }
};

} // namespace daex

#endif

