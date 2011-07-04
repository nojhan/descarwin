
#ifndef __DAEX_EVAL_YAHSP_H__
#define __DAEX_EVAL_YAHSP_H__

#include <cstdlib>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>

#include "utils/pddl.h"
#include "core/decomposition.h"
#include "core/plan.h"
#include "cpt-yahsp.h"


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
std::ostream & operator<<( std::ostream & out, BitArray bitarray );


//! Évaluateur principal, à utiliser pour les itérations normales
class daeYahspEval : public daeCptYahspEval
{
 public: daeYahspEval( unsigned int l_max_ = 20, unsigned int b_max_in = 10, unsigned int b_max_last = 30, double fitness_weight = 10, double fitness_penalty = 1e6 );


    virtual ~daeYahspEval();

public:
    virtual void step_recorder() {}
    virtual void step_recorder_fail() {}

public:
    void call( daex::Decomposition & decompo ); 
    
protected:

    unsigned int solve_next( daex::Decomposition & decompo, Fluent** next_state, unsigned int next_state_nb, long max_evaluated_nodes );

    void compress( daex::Decomposition & decompo );


    //! Free all necessary pointers to global variables
    // that have been used during call
    void free_yahsp_structures();

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
class daeYahspEvalInit : public daeYahspEval
{
public:

    daeYahspEvalInit( 
            unsigned int pop_size, 
            unsigned int l_max, 
            unsigned int b_max_in = 10000, 
            unsigned int b_max_last = 30000, 
            double fitness_weight = 10,
	    double fitness_penalty = 1e6
	    ) : daeYahspEval( l_max, b_max_in, b_max_last, fitness_weight, fitness_penalty ) 
    {
      node_numbers.reserve( pop_size * l_max );
    }

    void call( daex::Decomposition & decompo );

    //! Récupère le nombre de noeuds utilisés par une résolution avec yahsp
    void step_recorder();
    void step_recorder_fail();

    //! Le b_max est calculé comme la médiane du nombre total de noeuds parcourus sur l'ensemble de tous les appels à yahsp 
    //lors d'une première phase d'initialisation
    unsigned int estimate_b_max( double quantile = 0.5 );

protected:
    //! Distribution des nombres de noeuds utilisés dans les résolutions
    std::vector<unsigned int> node_numbers;
};


#endif // __DAEX_EVAL_YAHSP_H__

