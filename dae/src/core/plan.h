
#ifndef __DAEX_yahsp_plan_H__
#define __DAEX_yahsp_plan_H__

#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>

//extern "C" {
#include <src/plan.h>
//}


namespace daex
{

class Plan
{
protected:

    //! The makespan/cost of the plan, as extracted from the YAHSP SolutionPlan * pointer in the constructor
    TimeVal _makespan;

    //! Nombre d'étapes de recherche
    /* Nombre de backtracks si solveur type CPT,
     * nombre d'itérations si solveur type YAHSP.
     */
    unsigned int _search_steps;

    //! True if the plan is correctly initialized and contains a SolutionPlan
    bool _is_valid;

    //! The string representation of the plan
    std::string _plan_rep;

public:

    //! Construct a valid plan from a YAHSP's pointer: get the makespan and the string representation
    Plan( SolutionPlan * p_plan ) : _makespan(p_plan->makespan), _search_steps(0), _is_valid(true) 
      {   // get the plan representation
// OMP DIRTY
#pragma omp critical
        _plan_rep = plan_to_str( p_plan );
      }
    
    //! Construct a invalid plan from scratch
    /*! @TODO INT_MAX may not be the best choice here, FIXME
     */
      Plan() : _makespan( INT_MAX ), _search_steps(0), _is_valid(false),_plan_rep("No plan")
	{}

    void search_steps( unsigned int steps ) { _search_steps = steps; }
    unsigned int search_steps() const { return _search_steps; }

    TimeVal makespan() const { return _makespan; }

    void makespan(TimeVal makespan){ _makespan = makespan;} // mutateur nouvellement introduit

    bool isValid(){return _is_valid;}

    void isValid(bool is_valid){_is_valid = is_valid;}

    //! Print a SolutionPlan in a string using YAHSP's print_plan_ipc function
    std::string plan_to_str( SolutionPlan * p_plan )
    {        
        std::ostringstream out;

        char* buffer;
        size_t buffer_size;

        FILE* stream = open_memstream( & buffer, & buffer_size );

        if( stream == NULL ) {
            std::ostringstream msg;
            msg << "Cannot open memstream for printing Plan (" << p_plan << ")";
            throw std::runtime_error( msg.str() );
        }

        // CPT function @ plan.c:66
        // see also print_plan @ plan.c:32
        print_plan_ipc( stream, p_plan, 0.0 ); // last arg is the time taken FIXME ?

        fclose( stream );

        out << buffer;

        free( buffer );

        return out.str();
    }


    //! Print the string rep on a ostream
    friend std::ostream& operator<<( std::ostream& out, Plan & plan )
    {
        out << plan._plan_rep;
	return out;
    }

	 std::string & plan_rep(){

		return _plan_rep ;
	}

};

} // namespace daex

#endif // __DAEX_yahsp_plan_H__

