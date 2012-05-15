
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

#ifdef WITH_MPI

#include "undef_yahsp.h"

// Definitions used in cpt-yahsp can break STL.
// FIXME These undef are inelegant, another solution should be used
// (eg : rename all cpt-yahsp definitions, or not use Boost, or...)
/* #undef save
#undef store
#undef buffer
#undef distance
*/

#include <boost/mpi.hpp>
namespace mpi = boost::mpi;
#endif // WITH_MPI

extern Fluent** fluents;

namespace daex
{

class Atom
{

public:
    //! Constructeur prédicats + objets + date au plus tot + pointeur vers structure interne YAHSP
    // Atom( TimeVal start_time, Fluent* fluent ) : _fluent(fluent), _earliest_start_time(start_time)
    Atom( TimeVal start_time, unsigned int fluentIndex ) : _fluentIndex(fluentIndex), _earliest_start_time(start_time)
    {}
 
#ifdef WITH_MPI
    // Mandatory for boost serialization
	friend class boost::serialization::access;

    /**
     * Atom serialization (useful for boost::mpi).
     */
	template<class Archive>
	void serialize( Archive & ar, const unsigned int version )
	{
		ar 	& _earliest_start_time 
			& _fluentIndex;
	}
#endif // WITH_MPI

    //! Accesseurs
    TimeVal                     earliest_start_time() const { return _earliest_start_time; }
    Fluent *                    fluent()              const { return fluents[ _fluentIndex ]; }

protected:
    // Fluent *                    _fluent;
    unsigned int 				_fluentIndex;
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

