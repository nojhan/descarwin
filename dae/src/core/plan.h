
#ifndef __DAEX_yahsp_plan_H__
#define __DAEX_yahsp_plan_H__

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>

//extern "C" {
#include <src/plan.h>
//}


namespace daex
{

class Plan
{
protected:

    SolutionPlan _yahsp_plan;

    //! Temps CPU pris pour la dernière évaluation
    double _time_eval;

    //! Temps CPU pris par le sous-solveur lui-meme
    double _time_subsolver;

    //! Nombre d'étapes de recherche
    /* Nombre de backtracks si solveur type CPT,
     * nombre d'itérations si solveur type YAHSP.
     */
    unsigned int _search_steps;

    //! True if the plan is correctly initialized and contains a SolutionPlan
    bool _is_valid;
   
public:
//    Plan( SolutionPlan* plan, unsigned int search_steps, unsigned int search_time, unsigned int total_time ) : 
    //Plan( SolutionPlan plan ) : _yahsp_plan(plan), _search_steps(0), _time_subsolver(0), _time_eval(0) {}

    Plan( SolutionPlan plan ) : _yahsp_plan(plan), _is_valid(true), _search_steps(0), _time_subsolver(0), _time_eval(0) {}
    
    Plan() : _is_valid(false), _search_steps(0), _time_subsolver(0), _time_eval(0) {}

      // SEG FAULT !!! FIXME
      /*      ~Plan(){
	plan_free( &_yahsp_plan );
      }
      */
    SolutionPlan yahsp_plan() const { return _yahsp_plan; }
    SolutionPlan* p_yahsp_plan() { return &_yahsp_plan; }

    void time_eval( double time ) { _time_eval = time; }
    double time_eval() const { return _time_eval; }

    void time_subsolver( double time ) { _time_subsolver = time; }
    double time_subsolver() const { return _time_subsolver; }

    void search_steps( unsigned int steps ) { _search_steps = steps; }
    unsigned int search_steps() const { return _search_steps; }


    friend std::ostream& operator<<( std::ostream& out, Plan & plan )
    {
        
        if( ! plan._is_valid )
        {
            out << "No plan found (decomposition is unfeasible)";

        } else { // if plan._is_valid
        
            char* buffer;
            size_t buffer_size;

            FILE* stream = open_memstream( & buffer, & buffer_size );

            if( stream == NULL ) {
                std::ostringstream msg;
                msg << "Cannot open memstream for printing Plan (" << plan << ")";
                throw std::runtime_error( msg.str() );
            }

            // CPT function @ plan.c:66
            // see also print_plan @ plan.c:32
            print_plan_ipc( stream, plan.p_yahsp_plan(), plan.time_eval() );

            fclose( stream );

            out << buffer;

        } // if !plan._is_valid

        return out;
    };
};

} // namespace daex

#endif // __DAEX_yahsp_plan_H__

