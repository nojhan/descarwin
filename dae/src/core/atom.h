
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
#include <src/structs.h>
#include <src/problem.h>
//}

#include "utils/pddl.h"

namespace daex
{

class Atom
{
public:
    //! Constructeur prédicats + objets + date au plus tot + pointeur vers structure interne YAHSP
    Atom( TimeVal start_time, Fluent* fluent ) : _fluent(fluent), _earliest_start_time(start_time)
    {}
 
    //! Accesseurs
    TimeVal                     earliest_start_time() const { return _earliest_start_time; }
    Fluent *                    fluent()              const { return _fluent; }

protected:
    Fluent *                    _fluent;
    TimeVal                     _earliest_start_time;

public:
    friend std::ostream& operator<<( std::ostream& out, const Atom & atom )
    {
        out << "(Atom[" << atom.earliest_start_time() << "]: " << fluent_name(atom.fluent()) << ")";
        out.flush();
        return out;
    }
};

} // namespace daex

#endif

