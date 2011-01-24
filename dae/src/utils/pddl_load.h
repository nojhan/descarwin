
#ifndef __PDDL_LOAD_H_
#define __PDDL_LOAD_H_

/***** C++ headers *****/
#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <map>


/***** GMP C++ headers *****/
/* Bugfix : http://gmplib.org/list-archives/gmp-bugs/2006-November/000627.html
    "Because when __cplusplus is defined, gmp.h includes this:
    __GMP_DECLSPEC_XX std::ostream& operator<< (std::ostream &, mpz_srcptr);
    __GMP_DECLSPEC_XX std::ostream& operator<< (std::ostream &, mpq_srcptr);
    [...]
    which doesn't seem to work inside a block declared extern "C".
    I found that including gmpxx.h *before* gmp.h works"
*/
#include <gmpxx.h>

/***** C headers *****/
//extern "C" {
#include <src/cpt.h>
#include <src/options.h>
#include <src/structs.h>
#include <src/problem.h>
#include <src/solve.h>
#include <src/globs.h>
//} // extern "C"

#include "core/atom.h"

#define SOLVER_YAHSP "yahsp"
#define SOLVER_CPT   "cpt"

#define HEURISTIC_NO 0
#define HEURISTIC_H1 1
#define HEURISTIC_H2 2

namespace daex
{


//! Dictionary associating existing earliest start dates with their corresponding atoms
/** Note: std::map garanty that keys remains ordered according to their value (GNU implementation use a red-black tree).
 */
class ChronoPartition : public std::map<TimeVal, std::vector< daex::Atom*> > 
{
public:

    //! Constructor does nothing but initialize the max date to zero
    ChronoPartition() : _goal_max_date(0) {}

    //! Return the earliest start time of the final goal
    TimeVal goal_max_date() const { return _goal_max_date; }

    //! Loop over the goal states and put the latest date in the _goal_max_date member
    void compute_goal_max_date()
    {
        for( unsigned int i=0; i < goal_state_nb; ++i ) {
            _goal_max_date = std::max( _goal_max_date, goal_state[i]->init );
        } // for i in goal_state
    }

protected:
    //! Latest date of all the goals
    TimeVal _goal_max_date;
};


class pddlLoad
{
public:
    //! Parse the PDDL file, instanciate corresponding atoms in a vector and compute the corresponding chronological partition
    pddlLoad( std::string domain, std::string problem, std::string solver = SOLVER_YAHSP, unsigned int heuristic_start_times = HEURISTIC_H1, /*bool is_sequential = false,*/ std::vector<std::string> solver_args = std::vector<std::string>());


    ~pddlLoad();

    //! Use CPT to parse the PDDL file passed to the constructor, and convert the fluents list to Atom instances in a vector member
    void load_pddl( std::string solver = SOLVER_YAHSP, unsigned int heuristic_start_times = HEURISTIC_H1, /*bool is_sequential = false,*/ std::vector<std::string> solver_args = std::vector<std::string>() );

    //! Check for every atom the earliest date at which it can start.
    /** The chronological partition is a map associating dates to atoms instances.
     */
    void compute_chrono_partition();


    std::vector< daex::Atom*> atoms() const { return this->_atoms; }
    
    const ChronoPartition & chronoPartitionAtom() { return this->_chrono_partition_atom; }

public:
    /** List of available atoms
     *
     * Each atom contains pointers to the objects used by the CPT/YAHSP parser.
     */
    std::vector< daex::Atom*> _atoms;


protected:
    std::string _domain;
    std::string _problem;
    
    //! Note: The date = 0 does exists in the partition
    ChronoPartition _chrono_partition_atom;
};

} // namespace daex

#endif // __PDDL_LOAD_H_

