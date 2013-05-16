
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

#include <eo>
#include "utils/eoJsonUtils.h"

namespace daex
{

class Plan : public eoserial::Persistent, public eoPersistent
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
    //! Construct a valid plan from a YAHSP's pointer: get the makespan and the string representation
    
     Plan() : _makespan( MAXTIME ),_cost_add(MAXTIME ), _cost_max(MAXTIME ), _search_steps(0), _is_valid(false), _plan_rep("No plan") {}
	
      ~Plan(){}
     
     Plan( SolutionPlan * p_plan ) : _makespan(p_plan->makespan), _cost_add(p_plan ->cost_add), _cost_max(p_plan-> cost_max), _search_steps(0), _is_valid(true)
      {   // get the plan representation
# ifdef WITH_OMP
#pragma omp critical
# endif // WITH_OMP
        _plan_rep = plan_to_str( p_plan );
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

    std::string escape_newlines( const std::string &in, const std::string &search="\n", const std::string &replace="\\n" ) const
    {
        std::string s = in;
        for( size_t pos = 0; ; pos += replace.length() ) {
            // find the substring
            pos = s.find( search, pos );
            if( pos == std::string::npos ) break;
            // erase and insert == replace
            s.erase( pos, search.length() );
            s.insert( pos, replace );
        }
        return s;
    }

    std::string plan_to_str_escaped( SolutionPlan * p_plan )
    {
        return escape_newlines( plan_to_str( p_plan ) );
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

    eoserial::Object* pack(void) const
    {
        eoserial::Object* json = new eoserial::Object;
        json->add( "makespan", eoserial::make(_makespan) );
        json->add( "cost_add", eoserial::make(_cost_add) );
        json->add( "cost_max", eoserial::make(_cost_max) );
        json->add( "search_steps", eoserial::make(_search_steps) );
        json->add( "is_valid", eoserial::make(_is_valid) );

        const std::string rep = escape_newlines(_plan_rep);
        json->add( "plan_rep", eoserial::make( rep ) );

        return json;
    }

    void unpack( const eoserial::Object* json )
    {
        eoserial::unpack( *json, "makespan", _makespan );
        eoserial::unpack( *json, "cost_add", _cost_add );
        eoserial::unpack( *json, "cost_max", _cost_max );
        eoserial::unpack( *json, "search_steps", _search_steps );
        eoserial::unpack( *json, "is_valid", _is_valid );
        eoserial::unpack( *json, "plan_rep", _plan_rep );
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

#endif // __DAEX_yahsp_plan_H__

