
#ifndef __DAEX_EVAL_CPT_H__
#define __DAEX_EVAL_CPT_H__

#include <iostream>
#include <algorithm>

#include <eo>

#include "utils/pddl.h"
#include "utils/pddl_load.h"
#include "core/decomposition.h"
#include "cpt-yahsp.h"

extern "C" {
#include <src/globs.h>
#include <src/plan.h>
#include <src/yahsp.h>
#include <src/cpt.h>
#include <src/trace.h>
#include <src/options.h>
}

//! Affectation de pointeurs depuis les atomes DAEx vers leur équivalent CPT
void bindDaeCpt( daex::pddlLoad & pddl );

class daeCptEval : public daeCptYahspEval
{

public:
    
    daeCptEval( unsigned int l_max_ = 20, unsigned int b_max_ = 100000  ) :
        daeCptYahspEval(l_max_,b_max_),
        _current_state( NULL )
    {
        // TODO impossible dans la liste d'initialisation d'appeler le constructeur de la classe abstraite de base ?
         //daeCptYahspEval::b_max(b_max);
         //daeCptYahspEval::l_max(l_max);

        // normalement on devrait allouer init_state_nb
        //    _current_state = new Fluent*[ _current_state_nb ];
        // mais on fait des réallocation dans exec_plan, où on aura fluents_nb
        // éléments, donc on alloue tout de suite le maximum
        _current_state = new Fluent*[ fluents_nb ];
    }

    ~daeCptEval()
    {
        assert( _current_state != NULL );
        delete[] _current_state;
    }

public:
    //void operator() ( daex::Decomposition & decompo );
    void call( daex::Decomposition & decompo );

protected:

    void exec_plan();

    BitArray state_convert( Fluent * * & f_state, unsigned int f_state_nb );

protected:
    //! État courant de la recherche
    Fluent * * _current_state;

    //! Nombre de fluents dans l'état courant
    unsigned int _current_state_nb;

    //! Nombre de backtracks utilisés
    unsigned int _total_backtracks;

    // init_state et goal_state sont des variables globales définies dans globs.h
    // pointeur sur un tableau de Fluent CPT
    Fluent * * _intermediate_goal_state;
    unsigned int _intermediate_goal_state_nb;
};

#endif // __DAEX_EVAL_CPT_H__

