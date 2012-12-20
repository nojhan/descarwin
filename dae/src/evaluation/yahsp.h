
#ifndef __DAEX_EVAL_YAHSP_H__
#define __DAEX_EVAL_YAHSP_H__

#include <cstdlib>
#include <algorithm>
#include <vector>
#include <string>
#include <list>
#include <iostream>
 

#include "utils/pddl.h"
#include "core/decomposition.h"
#include "core/plan.h"
#include "cpt-yahsp.h"

#include <sys/time.h>
#include <sys/resource.h>

#include <utility>
#include <sstream>
#include <stdexcept>
#include <numeric>

 
#include "utils/utils.h"
//extern "C" {
#include <src/globs.h>
#include <src/plan.h>
#include <src/yahsp.h>
#include <src/cpt.h>
#include <src/trace.h>
#include <src/options.h>
//}



// FIXME gérer les timers yahsp

//! Affectation de pointeurs depuis les atomes DAEx vers leur équivalent YAHSP
void bindDaeYahsp( daex::pddlLoad & pddl );


//! Affiche un BitArray
std::ostream & operator<<( std::ostream & out, BitArray bitarray ){
  for( unsigned int i = 1; i < (unsigned int) fluents_nb; ++i ) {
        unsigned long bit = bitarray_get( bitarray, fluents[i] );
        if( bit != 0 ) {
            out << " " << fluent_name(fluents[i]);
        }
    }
    return out;
};


//! Évaluateur principal, à utiliser pour les itérations normales
template<class EOT=daex::Decomposition >

class daeYahspEval : public daeCptYahspEval<EOT>
{
 public: daeYahspEval( unsigned int l_max_ = 20, unsigned int b_max_in = 10, unsigned int b_max_last = 30, double fitness_weight = 10, double fitness_penalty = 1e6 ):daeCptYahspEval<EOT>( l_max_,b_max_in, b_max_last, fitness_weight, fitness_penalty )//,
        //_previous_state( NULL ) //, _intermediate_goal_state(NULL), _intermediate_goal_state_nb(0)
{
    // some init steps are not done here, but in pddl_load.cpp
    // notably the call to cpt_main
    // because it can only be called once
        
    //_previous_state = bitarray_create( fluents_nb );

    // _intermediate_goal_state = (Fluent**) malloc( fluents_nb * sizeof( Fluent* ) );

    // if( _intermediate_goal_state == NULL ) {
    //     std::ostringstream msg;
    //     msg << "Error: cannot allocate an intermediate goal state of size " << fluents_nb << std::endl;
    //     throw std::runtime_error( msg.str().c_str() );
    // }
};

    virtual ~daeYahspEval(){
  //free( _previous_state );
    //free( _intermediate_goal_state );
};

public:
    virtual void step_recorder() {};
    virtual void step_recorder_fail() {};

public:
   virtual void  call( EOT & decompo ){
                                 #ifndef NDEBUG
                                     eo::log << eo::xdebug << "decompo.size=" << decompo.size() << std::endl;
                                 #endif
                                 #ifndef NDEBUG

                                     eo::log << eo::xdebug << "Check goal consistency" << std::endl;
                                     for( std::list<daex::Goal>::iterator igoal  = decompo.begin(), goal_end = decompo.end(); igoal != goal_end; ++igoal ) {
                                         assert_noduplicate( igoal->begin(), igoal->end() );
                                         assert_nomutex(     igoal->begin(), igoal->end() );
                                     }
                                 #endif
    if( ! decompo.invalid() ) { // do nothing
                                 #ifndef NDEBUG
                                         eo::log << eo::debug << "-";
                                         eo::log.flush();
                                 #endif
    } else { // if decompo.invalid

#ifndef PAPERVERSION
            //   JACK the code does not even try to evaluate decompositions that are too long 
              // FIXME what is the effect on variation operators that relies on last_reached?
        if( decompo.size() >  daeCptYahspEval<EOT>::_l_max ) {
           decompo.fitness(daeCptYahspEval<EOT>::fitness_unfeasible_too_long());
           decompo.setFeasible(false);
        } else 
                                 #endif
        {
        BitArray previous_state = bitarray_create( fluents_nb );  
                                 #ifndef NDEBUG
                                         eo::log << eo::xdebug << "malloc plans...";
                                         eo::log.flush();
                                 #endif
        cpt_malloc( plans, decompo.size()+1 ); // +1 for the subplan between the last goal and the final state
        plans_nb = 0;
                                 #ifndef NDEBUG
                                         eo::log << eo::xdebug << "ok" << std::endl;
                                         eo::log << eo::xdebug << "yahsp reset...";
                                         eo::log.flush();
                                 #endif
        yahsp_reset();
                                 #ifndef NDEBUG
                                         eo::log << eo::xdebug << "ok" << std::endl;
                                 #endif
        decompo.reset_number_evaluated_goals(); // compteur de goals
        decompo.reset_number_useful_goals(); // compteur de goals utiles
        decompo.reset_number_evaluated_nodes(); // compteur des tentatives de recherche

                                 #ifndef NDEBUG
                                         eo::log << eo::xdebug << "for each goal:" << std::endl;
                                 #endif
        unsigned int code = 0; // return code of cpt_search
       // b_max( b_max_in); // set the generic b_max 
        decompo.b_max(  daeCptYahspEval<EOT>::_b_max_in ); // VV : set b_max for the decomposition

        //parcourt les goals de la décomposition ///std::list<daex::Goal>
		#ifndef NDEBUG
        	unsigned int goalCounter = 0;
        #endif
        for( daex::Decomposition::iterator igoal = decompo.begin(), iend = decompo.end(); igoal != iend; ++igoal ) {
                                 #ifndef NDEBUG
        									 eo::log << eo::xdebug << "\t goal #" << goalCounter++ << "/" << decompo.size() << std::endl;
                                             eo::log << eo::xdebug << "\t\tcopy of states and fluents...";
                                             eo::log.flush();

                                 #endif
         //  copie des goals daex dans leur equivant YAHSP
//           nouvelle allocation de tableau de goal
            assert( igoal->size() > 0 );
            Fluent **intermediate_goal_state = (Fluent **) malloc(igoal->size() * sizeof(Fluent *));
            unsigned int i = 0;
            for( daex::Goal::iterator iatom = igoal->begin(); iatom != igoal->end(); ++iatom ) {
              //  le compilateur demande à expliciter le template pour fluents, 
                //car le C++ ne prend pas en compte les types de retour dans la signature (beurk).
                intermediate_goal_state[i] =  (*iatom)->fluent();
                i++;
            }
             assert( i ==  igoal->size());
          //  search a plan towards the current goal
            bitarray_copy( previous_state, *get_current_state(), fluents_nb );
            code = solve_next( decompo, intermediate_goal_state, igoal->size(), daeCptYahspEval<EOT>::_b_max_in );
            free(intermediate_goal_state);

            if( code != PLAN_FOUND ) {
#ifdef PAPERVERSION
              decompo.fitness(fitness_unfeasible(decompo, previous_state), false);
         decompo.setFeasible(false);
#else
              decompo.fitness(this->fitness_unfeasible_intermediate(decompo));
          decompo.setFeasible(false);
#endif 
              break;
            }
        } // for igoal in decompo
//        here we have reached the last goal of the decomposition, it remains searching towards the ultimate goal
        if((decompo.size() == 0) || (code == PLAN_FOUND)) {
          //  set the b_max specific to this step
          //  b_max( _b_max_last );
            decompo.b_max( daeCptYahspEval<EOT>::_b_max_last ); // VV : set b_max for the decomposition

            bitarray_copy( previous_state, *get_current_state(), fluents_nb );
            unsigned int code = solve_next( decompo, goal_state, goal_state_nb, daeCptYahspEval<EOT>::_b_max_last );

            if( code == PLAN_FOUND ) {
                compress( decompo );
                /*
                if (solution_plan->makespan < 6)
                    std::cout << decompo.size() << " " << plans_nb << std::endl;
                */

                decompo.fitness(this->fitness_feasible( decompo ));
        decompo.setFeasible(true);

                                 #ifndef NDEBUG
                                                 eo::log << eo::debug << "*";
                                                 eo::log.flush();
                                 #endif
            } else {
#ifdef PAPERVERSION
                decompo.fitness(fitness_unfeasible(decompo, previous_state));
        decompo.setFeasible(false);
#else
                decompo.fitness(this->fitness_unfeasible_final(decompo) );
        decompo.setFeasible(false);
#endif 
            } // if PLAN_FOUND for last goal
        } // if PLAN_FOUND
        cpt_free(previous_state);
      } // if size > _l_max
    } // if !decompo.invalid
   free_yahsp_structures();
}; 
    
    void post_call( EOT & decompo ) {
        decompo.plan().search_steps( decompo.get_number_evaluated_nodes() );
        
        } ;
    
     void pre_call( EOT & decompo ) {/* FIXME unused variable */};   
     
     void operator()( EOT & decompo ) { 
        pre_call( decompo );
        call( decompo );
        post_call( decompo );
     } ;
     

protected:

    unsigned int solve_next( EOT & decompo, Fluent** next_state, unsigned int next_state_nb, long max_evaluated_nodes ){
                                 #ifndef NDEBUG
                                     eo::log << eo::xdebug << "ok" << std::endl;
                                     eo::log << eo::xdebug << "\t\tcall the solver...";
                                     eo::log.flush();
                                 #endif
    unsigned int return_code = cpt_search( init_state, init_state_nb, next_state, next_state_nb, false, false, false, max_evaluated_nodes );
                                 #ifndef NDEBUG
                                     eo::log << eo::xdebug << "ok" << std::endl;
                                     eo::log << eo::xdebug << "\t\treturn code : " << return_code << " " << std::endl;
                                     eo::log << eo::xdebug << "\t\t\t PLAN_FOUND:" <<PLAN_FOUND<<" ; GOALS_MUTEX:"<<GOALS_MUTEX<<" ; INIT_PROP_FAILED:"<<INIT_PROP_FAILED<<" ; BACKTRACK_LIMIT:"<<BACKTRACK_LIMIT<<" ; BOUND_LIMIT:"<<BOUND_LIMIT<<" ; NO_PLAN:"<<NO_PLAN<<std::endl;
                                     eo::log << eo::xdebug << "\t\t saving search data...";
                                     eo::log.flush();

                                 #endif
    if( return_code == NO_PLAN || return_code == GOALS_MUTEX ) {
        step_recorder_fail();
                                 #ifndef NDEBUG
                                         eo::log << eo::debug << "x";
                                         eo::log.flush();
                                         eo::log << eo::xdebug << std::endl;
                                 #endif
    } else if( return_code == PLAN_FOUND ) { assert( solution_plan != NULL );

        if( solution_plan->steps_nb > 0 )  {
            decompo.incr_number_useful_goals(1); // un goal utile supplémentaire
            decompo.incr_number_evaluated_nodes(static_cast<unsigned int>( solution_plan->backtracks ));
        }
        decompo.incr_number_evaluated_goals(1); // incrémente le compteur de plan

        plans[plans_nb] = solution_plan;
        plans_nb++;

        // if k != plans_nb => the same decomposition is evaluated simultaneously by several threads !!!
        assert (decompo.get_number_evaluated_goals() == (unsigned int) plans_nb);

        decompo.plans_sub_add( daex::Plan() ); // On ne stocke plus les sous-plans mais on garde la structure notamment pour last_reached.
        decompo.last_subplan().search_steps( decompo.get_number_evaluated_nodes() );

                                 #ifndef NDEBUG
                                         eo::log << eo::xdebug << "ok" << std::endl;
                                         eo::log << eo::xdebug << "\t\t # evaluated nodes:" << solution_plan->backtracks << std::endl;
                                         eo::log << eo::xdebug << "\t\trecord steps...";
                                         eo::log.flush();
                                 #endif
        step_recorder();
        
        solution_plan = NULL; // the corresponding pointer is stored in plans, thus we do not free it

                                 #ifndef NDEBUG
                                         eo::log << eo::xdebug << "ok" << std::endl;
                                 #endif

    }
                        /*else { // return_code != NO_PLAN && != GOALS_MUTEX && != PLAN_FOUND
                                   throw std::runtime_error( "Unknown error code from cpt_search" );
                      }*/
    return return_code;
};

     void compress( EOT & decompo ){
                                 #ifndef NDEBUG
                                         eo::log << eo::xdebug << "\t\tcompression...";
                                         eo::log.flush();
                                 #endif
        // compression, utilise la globale "plans" construire plus haut et créé un plan compressé dans solution_plan
        yahsp_compress_plans();
                                         assert(solution_plan != NULL);
        // TODO pendant les tests, le plan ne peut pas etre vide, mais en compétition, cela peut arriver, auquel cas il faudra virer l'assert (penser à compiler en NDEBUG)

                                         assert(solution_plan->makespan > 0);

                                 #ifndef NDEBUG
                                         eo::log << eo::xdebug << "ok" << std::endl;
                                         eo::log << eo::xdebug << "\t\tsave compressed plan and fitness...";
                                         eo::log.flush();
                                 #endif
        decompo.plan_global( daex::Plan( solution_plan ) ); // sauvegarde le plan compressé global pour DAEx
                                       // NOTE: solution_plan is freed in free_yahsp_structures
        decompo.last_subplan().search_steps( decompo.get_number_evaluated_nodes() );
                                 #ifndef NDEBUG
                                         eo::log << eo::xdebug << "ok" << std::endl;
                                 #endif
     };


    //! Free all necessary pointers to global variables
    // that have been used during call
    void free_yahsp_structures()
    {
                                 #ifndef NDEBUG
                                         eo::log << eo::xdebug << "\t\tfree plans...";
                                         eo::log.flush();
                                 #endif
        // libère la variable globale "plans", utilisée par yahsp lors de la compression
        for( unsigned int p=0; p < (unsigned int) plans_nb; ++p ) {
            plan_free( plans[p] );
        }
        plans_nb = 0;

        cpt_free( plans );

        if( solution_plan != NULL ) {
            plan_free( solution_plan );
            solution_plan = NULL;
        }
                                 #ifndef NDEBUG
                                        eo::log << eo::xdebug << "ok" << std::endl;
                                 #endif
    };

    // VV : moved to Decomposition
    //! État à chaque itération
    //BitArray _previous_state;


    // VV : removed this ! shared between all threads... build it in method 'call'
    // init_state et goal_state sont des variables globales définies dans globs.h
    // pointeur sur un tableau de Fluent CPT
    /* Fluent * * _intermediate_goal_state; */
    /* unsigned int _intermediate_goal_state_nb; */
};


//! Classe à utiliser lors de la première itération, pour estimer b_max
template<class EOT=daex::Decomposition>
class daeYahspEvalInit : public daeYahspEval<EOT>
{
public:

    daeYahspEvalInit( 
            unsigned int pop_size, 
            unsigned int l_max, 
            unsigned int b_max_in = 10000, 
            unsigned int b_max_last = 30000, 
            double fitness_weight = 10,
        double fitness_penalty = 1e6
        ) : daeYahspEval<EOT>( l_max, b_max_in, b_max_last, fitness_weight, fitness_penalty ) 
    {
      node_numbers.reserve( pop_size * l_max );
    }

    void call( EOT & decompo ){
#ifndef NDEBUG
    eo::log << eo::logging << std::endl << "init decompo nodes nb: ";
    eo::log.flush();
    int prev = std::accumulate( node_numbers.begin(), node_numbers.end(), 0 );
#endif

    daeYahspEval<EOT>::call( decompo );

#ifndef NDEBUG
    int next = std::accumulate( node_numbers.begin(), node_numbers.end(), 0 );
    eo::log << eo::logging << "     (" << next - prev << ")";
#endif
}

;

    //! Récupère le nombre de noeuds utilisés par une résolution avec yahsp
    void step_recorder(){
    
    node_numbers.push_back( static_cast<int>( solution_plan->backtracks ) ); // TODO SolutionPlan->backtracks est codé comme un double dans plan.h

#ifndef NDEBUG
    eo::log << eo::logging << " " << node_numbers.back();
    eo::log.flush();
#endif
};
    void step_recorder_fail(){
      node_numbers.push_back(stats.evaluated_nodes);
#ifndef NDEBUG
    eo::log << eo::logging << " " << node_numbers.back();
    eo::log.flush();
#endif
};

    //! Le b_max est calculé comme la médiane du nombre total de noeuds parcourus sur l'ensemble de tous les appels à yahsp 
    //lors d'une première phase d'initialisation
    unsigned int estimate_b_max( double quantile = 0.5 ){
    assert( node_numbers.size() > 0 );

    //unsigned int nth = node_numbers.size() / 2; // division euclidienne, indicage à 0 => prend l'élément supérieur
    unsigned int nth = static_cast<unsigned int>( ceil( static_cast<double>( node_numbers.size() ) * quantile ) );

#ifndef NDEBUG
    if( nth == 0 || nth == node_numbers.size()-1 ) {
        eo::log << eo::warnings << "WARNING: while estimating the b_max, the quantile at " << quantile << " returns the " << nth << "th element (size=" << node_numbers.size() << ")" << std::endl;
    }
#endif

    // JACK use a simple computation of the median (rounding in case of an even size)
    
    // code version:
    //return node_numbers[ nth ];

    // suggested version: use a linear interpolation in case of even size
    
    if( node_numbers.size() % 2 != 0 ) { // impair

        std::nth_element( node_numbers.begin(), node_numbers.begin() + nth,  node_numbers.end() );
        // l'element central est la médiane
        return node_numbers[nth];

    } else { // pair

        unsigned int m1, m2;

        std::nth_element( node_numbers.begin(), node_numbers.begin() + nth,  node_numbers.end() );
        m1 = node_numbers[nth];
        std::nth_element( node_numbers.begin(), node_numbers.begin() + nth - 1,  node_numbers.end() );
        m2 = node_numbers[nth-1];
        
        // mean of center elements
        // with rounding, because the b_max should be an uint
        return static_cast<unsigned int>( ceil( static_cast<double>( m1 + m2 ) / 2.0 ) );
    } 
}
;

protected:
    //! Distribution des nombres de noeuds utilisés dans les résolutions
    std::vector<unsigned int> node_numbers;
};


#endif // __DAEX_EVAL_YAHSP_H__

