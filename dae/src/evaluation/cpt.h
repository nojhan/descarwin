
#ifndef __DAEX_EVAL_CPT_H__
#define __DAEX_EVAL_CPT_H__

#include <iostream>
#include <algorithm>

#include <eo>

#include "utils/pddl.h"
#include "utils/pddl_load.h"
#include "core/decomposition.h"
#include "cpt-yahsp.h"

//extern "C" {
#include <src/globs.h>
#include <src/plan.h>
#include <src/yahsp.h>
#include <src/cpt.h>
#include <src/trace.h>
#include <src/options.h>
//}

//! Affectation de pointeurs depuis les atomes DAEx vers leur équivalent CPT
void bindDaeCpt( daex::pddlLoad & pddl ){
{
    bindDaeYahspCpt( pddl, SOLVER_CPT );
};



template<class EOT= daex::Decomposition>
class daeCptEval : public daeCptYahspEval<EOT>
{

public:
    
    daeCptEval( unsigned int l_max_ = 20, unsigned int b_max_ = 100000  ) :
        daeCptYahspEval<EOT>(l_max_,b_max_),
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
    void call( EOT & decompo ){
    
    // COMMUN <
    if( ! decompo.invalid() ) {
        return;
    }
   
    // compteur de goals
    _k = 0;

    // compteur de goals utiles
    _u = 0;

    // compteur des tentatives de recherche
    _B = 0;

    // > COMMUN
    
    // Nombre de backtracks utilisés
    _total_backtracks = 0;

    // COMMUN <
    // libère la variable globale "plans", utilisée par cpt lors de la compression
    for( unsigned int p=0; p < plans_nb; ++p ) {
        plan_free( plans[p] );
    }
    plans_nb = 0;
    cpt_malloc( plans, _l_max );
    // > COMMUN

    // initialise ce qui va stocker les sous-plans
    decompo.plans_sub_reset();

    // initialisation de l'état courant à partir de l'état initial
    /*if( _current_state != NULL ) {
        assert( _current_state_nb > 0 );
        delete[] _current_state;
    }*/
    // normalement on devrait allouer init_state_nb
    //    _current_state = new Fluent*[ _current_state_nb ];
    // mais on fait des réallocation dans exec_plan, où on aura fluents_nb
    // éléments, donc on alloue tout de suite le maximum
    //_current_state = new Fluent*[ fluents_nb ];

    std::copy( init_state, init_state + init_state_nb, _current_state );

    // garde le nombre d'éléments correct dans une variable annexe
    _current_state_nb = init_state_nb;

    /* FIXME demander à Vincent :
     * Dans dae.c, la fonction cptResetSearch fait un        
       FORMIN(a, actions, 2) {
          a->nb_instances = (opt.max_plan_length == 0 ? 0 : opt.max_plan_length - 1);
        } EFOR;
     * Pourquoi ?
     */

    // COMMUN <
    // parcours les goals de la décomposition
    EOT::iterator iend = decompo.end();
    for( EOT::iterator igoal = decompo.begin(); 
         igoal != iend;
         ++igoal ) {

        // nouvelle allocation de tableau de goal
        _intermediate_goal_state_nb = igoal->size();
        assert( _intermediate_goal_state_nb > 0 );
        _intermediate_goal_state = new Fluent*[_intermediate_goal_state_nb];

        // récupère le pointeur vers le fluent d'origine à partir des atomes dans le goal
        unsigned int i = 0;
        for( daex::Goal::iterator iatom = igoal->begin(); iatom != igoal->end(); ++iatom ) {
            // le compilateur demande à expliciter le template pour fluents, car le C++ ne prend pas en compte les types de retour dans la signature (beurk).
            _intermediate_goal_state[i++] = (*iatom)->fluent_of<Fluent*>( "cpt" );
        } // for iatom in goal
        // > COMMUN

        
        /*
         * Codes renvoyés par CPT :
            PLAN_FOUND 0
            GOALS_MUTEX 1
            INIT_PROP_FAILED 2
            BACKTRACK_LIMIT 3
            BOUND_LIMIT 4
         */
        
        // premier démarrage du timer pour le solveur
        _timer_subsolver.restart();

        // premier appel au solveur, pour aller de l'init à la première station
        unsigned int return_code = 
            cpt_search( _current_state, _current_state_nb, 
                        _intermediate_goal_state, _intermediate_goal_state_nb,
                        false, false, false );

        // récupère le temps de calcul du sous-solveur sur cette itération
        double time_subsolver = _timer_subsolver.get();

        // incrémente le compteur subsolver pour le plan global
        decompo.plan().time_subsolver( decompo.plan().time_subsolver() + time_subsolver );


        // si un plan est trouvé
        if( return_code == PLAN_FOUND ) {

            if( solution_plan->steps_nb > 0 ) {
                // un goal utile supplémentaire
                _u++;

                // un certains nombre de backtracks supplémentaires
                _B += static_cast<unsigned int>( solution_plan->backtracks );
            }

            // incrémente le compteur de plan
            _k++;

            // exécution du plan
            // TODO compter l'exécution du plan comme du temps passé dans le sous-solveur ?
            exec_plan();

            // ici, on a un _current_state à jour

            // stocke le plan intermédiaire dans la variable globale "plans"
            // utilisée par cpt lors de la compression
            plans[plans_nb++] = solution_plan;

            // convertit et stocke les plans intermédiaires dans des structures propres à DAEx
            decompo.plans_sub_add( convertPlanCptToDae( solution_plan ) );

            // timer à jour pour le dernier plan inséré
            decompo.last_subplan().time_subsolver( time_subsolver );
            decompo.last_subplan().search_steps( _B );

            // maintiens le temps de calcul ayant été nécéssaire pour l'ensemble de la recherche jusqu'ici
            // dans le sous-plan sauvegardé
            decompo.last_subplan().time_eval( _timer_eval.get() );

            // Note : le prochain cpt_search libère solution_plan s'il a déjà été
            // alloué, avant de réallouer un nouveau plan. 
            // On veut le garder, on le met donc à NULL, afin de garder 
            // la mémoire allouée.
            // Attention, il faut faire un plan_free, sinon fuite de
            // mémoire (cf. début de cette méthode).
            solution_plan = NULL;
            
        } else {
           assert( return_code == GOALS_MUTEX      ||
                   return_code == INIT_PROP_FAILED ||
                   return_code == BACKTRACK_LIMIT  ||
                   return_code == BOUND_LIMIT      ); 
            
            decompo.fitness( fitness_unfeasible( decompo, state_convert(_current_state,_current_state_nb) ) );
            return;
            
        }



    } // for igoal in decompo


    // à ce point, on a atteint le dernier goal de la décomposition, 
    // et on cherche à atteindre le goal final


    _timer_subsolver.restart();

    unsigned int return_code = 
        cpt_search( _current_state, _current_state_nb, 
                    goal_state, goal_state_nb,
                    false, false, false );

    double time_subsolver = _timer_subsolver.get();

    // si un plan est trouvé
    if( return_code == PLAN_FOUND ) {

        if( solution_plan->steps_nb > 0 ) {
            // un goal utile supplémentaire
            _u++;

            // un certains nombre de backtracks supplémentaires
            _B += static_cast<unsigned int>( solution_plan->backtracks );
        }

        // incrémente le compteur de plan
        _k++;

        // stocke le dernier plan intermédiaire dans la variable globale "plans"
        // utilisée par cpt lors de la compression
        plans[plans_nb++] = solution_plan;

        // convertit et stocke les plans intermédiaires dans des structures propres à DAEx
        decompo.plans_sub_add( convertPlanCptToDae( solution_plan ) );

        // timer à jour pour le dernier plan inséré
        decompo.last_subplan().time_subsolver( time_subsolver );
        decompo.last_subplan().search_steps( _B );

        // maintiens le temps de calcul ayant été nécéssaire pour l'ensemble de la recherche jusqu'ici
        // dans le sous-plan sauvegardé
        decompo.last_subplan().time_eval( _timer_eval.get() );

        // Note : le prochain cpt_search libère solution_plan s'il a déjà été
        // alloué, avant de réallouer un nouveau plan. 
        // On veut le garder, on le met donc à NULL, afin de garder 
        // la mémoire allouée.
        // Attention, il faut faire un plan_free, sinon fuite de
        // mémoire (cf. début de cette méthode).
        solution_plan = NULL;
        

        // Compression
        

        // TODO valider l'emploi de la compression des orderings et causals, une combinaison des deux, ou pas
        
        // force temporairement les précédences qualitatives
        bool cqp = opt.complete_qualprec;
        opt.complete_qualprec = true;
       
        _timer_subsolver.restart();

        // lance une recherche compressive
        unsigned int return_code = 
            cpt_search( _current_state, _current_state_nb, 
                        goal_state, goal_state_nb,
                        true, // compression
                        true, // compresse avec les causals
                        true  // compresse avec les orderings
                      );


        // remet l'option à l'état original
        opt.complete_qualprec = cqp;


        // si pas de compression trouvée : concaténation
        if( return_code != PLAN_FOUND ) {

            assert( solution_plan == NULL );


            // compte le nombre total d'étapes
            unsigned int total_steps_nb = 0;
            for( unsigned int i = 0; i < plans_nb; ++i ) {
                total_steps_nb += plans[i]->steps_nb;
            }

            // alloue la solution concaténée
            solution_plan = new SolutionPlan;
            solution_plan->steps = new Step*[ total_steps_nb ];
            solution_plan->steps_nb = total_steps_nb;

            // curseur pour la copie
            unsigned int solution_index = 0;

            // on construit la concaténation des plans intermédiaires
            for( unsigned int i = 0; i < plans_nb; ++i ) {

                // copie les steps du plan i
                std::copy( plans[i]->steps, plans[i]->steps + plans[i]->steps_nb, 
                           solution_plan->steps + solution_index );

                solution_index += plans[i]->steps_nb;

            } // for i in plans
            
            assert( solution_index == total_steps_nb );

        } // si pas de compression trouvée

        // timer subsolver mis à jour après l'éventuelle concaténation seulement
        decompo.plan().time_subsolver( decompo.plan().time_subsolver() + _timer_subsolver.get() );

        decompo.fitness( fitness_feasible( decompo )  );

        // sauvegarde le plan compressé global pour DAEx
        decompo.plan_global( convertPlanCptToDae( solution_plan ) );

        return;
        
    } else {
       assert( return_code == GOALS_MUTEX      ||
               return_code == INIT_PROP_FAILED ||
               return_code == BACKTRACK_LIMIT  ||
               return_code == BOUND_LIMIT      ); 
        
        decompo.fitness( fitness_unfeasible( decompo, state_convert(_current_state,_current_state_nb) ) );
        
        return;
    }


    // FIXME valider le choix entre : pénalisation des individus dont la taille dépasse l_max OU opérateur de croisement empechant par construction les dépassements de taille

    // FIXME dans le cas de la planif séquentielle, vérifier que la somme des makespans des solutions intermédiaires est égale au makespan total après compression. Si non : ne pas compresser.
}
;

protected:

    void exec_plan(){
    // TODO utiliser std::find au lieu de toutes ces boucles
    
    for( unsigned int s = 0; s < solution_plan->steps_nb; s++ ) {
         Step step = *(solution_plan->steps[s]);

        // pour tous les fluents dans l'état, 
        // regarde s'ils sont dans la liste de retrait de l'action considérée
        // si oui, le supprime
        for( unsigned int f = 0; f < _current_state_nb; f++ ) {
            for( unsigned int d = 0; d < step.action->del_nb; d++ ) {
                if( _current_state[f] == step.action->del[d] ) {
                    // prend le dernier fluent
                    // écrase le fluent à supprimer avec
                    // réduit la taille de un élément
                    // ré-itère à l'élément courant
                    _current_state[ f-- ] = _current_state[ --_current_state_nb ];
                    break;
                } // if f == d
            } // for d in step.action->del
        } // for f in _current_state

        // pour tous les fluents à ajouter, de l'action en cours
        // on ne le rajoute dans l'état courant que s'il n'y est pas déjà
        for( unsigned int a = 0; a < step.action->add_nb; a++ ) {

            bool found = false;

            for( unsigned int f = 0; f < _current_state_nb; f++ ) {
                // si le fluent considéré est dans la liste des ajouts
                if( step.action->add[a] == _current_state[f] ) {
                    // on passe à l'ajout suivant
                    found = true;
                    break;
                }
             } // for f in _current_state

            // si on a pas trouvé le fluent dans l'état courant
            if( ! found ) {
                // on l'y ajoute
                _current_state[ _current_state_nb++ ] = step.action->add[a];
            }
        } // a in step.action->add

    } // for s in solution_plan->steps
}
;

    BitArray state_convert( Fluent * * & f_state, unsigned int f_state_nb ){
    // Note : les bits sont tous mis à zéro grace à un calloc
    BitArray state = bitarray_create( fluents_nb );

    for( unsigned int i = 0; i < f_state_nb; ++i ) {
        bitarray_set( state, f_state[i] );
    } // for i in _ccurent_state

   return state;
};

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

