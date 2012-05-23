
#ifndef __DAEX_yahsp_plan_H__
#define __DAEX_yahsp_plan_H__

#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>

//extern "C" {
#include <src/cpt.h>
#include <src/plan.h>
#include <src/yahsp.h>
//}

#include "utils/json/Json.h"

#ifdef WITH_MPI
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#endif // WITH_MPI

namespace daex
{

class Plan : public json::Serializable
{
protected:

    //! The makespan/cost of the plan, as extracted from the YAHSP SolutionPlan * pointer in the constructor
    TimeVal _makespan;
    TimeVal _cost_add;
    TimeVal _cost_max ;
 
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

#ifdef WITH_MPI
    // Gives access to boost::serialization
	friend class boost::serialization::access;

    /**
     * Serializes the plan into a boost archive (useful for boost::mpi)
     */
	template <class Archive>
	void serialize( Archive & ar, const unsigned int version )
	{

        (void) version; // avoid compilation warning
	}
#endif // WITH_MPI

    //! Construct a valid plan from a YAHSP's pointer: get the makespan and the string representation
    
     Plan() : _makespan( MAXTIME ),_cost_add(MAXTIME ), _cost_max(MAXTIME ), _search_steps(0), _is_valid(false), _plan_rep("No plan") {}
	
      ~Plan(){}
     
     Plan( SolutionPlan * p_plan ) : _makespan(p_plan->makespan), _cost_add(p_plan ->cost_add), _cost_max(p_plan-> cost_max), _search_steps(0), _is_valid(true)
      {   // get the plan representation
//FIXME OMP DIRTY
#pragma omp critical
        _plan_rep = plan_to_str( p_plan );
      }
    
    //! Construct a invalid plan from scratch
      Plan  & operator=(const daex::Plan  & other){
       if (this != &other) {
            _makespan = other._makespan;
            _cost_add = other._cost_add; 
            _cost_max = other._cost_max;
             _search_steps = other._search_steps;
             _is_valid = other._is_valid;
	     _plan_rep  = other._plan_rep;
        }
        return *this;
    }


    void search_steps( unsigned int steps ) { _search_steps = steps; }
    unsigned int search_steps() const { return _search_steps; }

    TimeVal makespan() const { return _makespan; }
    
    TimeVal cost_add() const {return _cost_add;}
     
    TimeVal cost_max() const {return _cost_max;}

    void makespan(TimeVal makespan){ _makespan = makespan;} // mutateur nouvellement introduit
    
    void cost_add(TimeVal cost_add){_cost_add = cost_add;}
     
    void cost_max(TimeVal cost_max){_cost_max = cost_max;}

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
    friend std::ostream& operator<<( std::ostream& out, const daex::Plan & plan ) 
    {
        out << plan._plan_rep;
        return out;
    }

    std::string & plan_rep()
    {
        return _plan_rep ;
    }

    json::Object* toJson(void)
    {
        json::Object* json = new json::Object;
        json->addPair( "makespan", json::String::make(_makespan) );
        json->addPair( "cost_add", json::String::make(_cost_add) );
        json->addPair( "cost_max", json::String::make(_cost_max) );
        json->addPair( "search_steps", json::String::make(_search_steps) );
        json->addPair( "is_valid", json::String::make(_is_valid) );
        json->addPair( "plan_rep", json::String::make(_plan_rep) );
        return json;
    }

    void fromJson( json::Object* json )
    {
        _makespan = json->get< TimeVal >( "makespan" );
        _cost_add = json->get< TimeVal >( "cost_add" );
        _cost_max = json->get< TimeVal >( "cost_max" );
        _search_steps = json->get< unsigned int >( "search_steps" );
        _is_valid = json->get< bool >( "is_valid" );
        _plan_rep = json->get< std::string >( "plan_rep" );
    }
};

} // namespace daex

#endif // __DAEX_yahsp_plan_H__

