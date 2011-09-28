
#ifndef __DAE_BIND_YAHSP_CPT_H__
#define __DAE_BIND_YAHSP_CPT_H__

#include <string>
#include <eo>

#include "utils/pddl_load.h"
#include "core/decomposition.h"
#include "utils/posix_timer.h"

/***** En-tetes C *****/
//extern "C" {
#include <src/structs.h>
#include <src/plan.h>
#include <src/yahsp.h>
//}

//! Convertit un TimeVal en string, en utilisant print_time de CPT/YAHSP
std::string timeValToString( TimeVal x );

//! Convertit un plan CPT/YAHSP en plan DAEx
//daex::Plan convertPlanCptToDae( SolutionPlan plan );

//! Affectation de pointeurs depuis les atomes DAEx vers leur équivalent CPT/YAHSP
void bindDaeYahspCpt( daex::pddlLoad & pddl, std::string solver = SOLVER_YAHSP );

//! Teste si un Fluent (CPT/YAHSP) est égal à un Atome (DAEx)
/* Comparateur pour la recherche, utilisé par std::find dans bindDaeYahspCpt
 */
bool operator==( const Fluent * fluent, const daex::Atom & atom );

//! Teste si un terme (CPT/YAHSP) est différent d'un objet (DAEx)
/* Comparateur pour la recherche, utilisé dans operator==
 */
bool operator!=( const PDDLTerm & term, const daex::pddlObject & object );


//! Classe de base pour l'évaluation avec CPT ou YAHSP
/* Regroupe uniquement le code et les déclarations en commun.
 * Note : L'operator() et le calcul de distance sont virtuels purs, 
 * il doivent etre implémentés.
 */
class daeCptYahspEval : public eoEvalFunc<daex::Decomposition>
{
public:
  daeCptYahspEval( unsigned int l_max_ = 20, unsigned int b_max_in = 10, unsigned int b_max_last = 30, double fitness_weight = 10, double fitness_penalty = 1e60, bool sequential = false ) : _l_max( l_max_ ), _b_max_in(b_max_in),_b_max_last( b_max_last),_unknown_parameter(fitness_weight),_fitness_penalty( fitness_penalty ),_sequential( sequential ) { }

    virtual ~daeCptYahspEval() {}

public:
    unsigned int l_max() const { return _l_max; }
    void l_max( unsigned int l ) { _l_max = l; }

public:
    //! Proxy EO avec appels aux timers
    /*virtual*/ void operator() ( daex::Decomposition & decompo ) 
    {
        pre_call( decompo );
        call( decompo );
        post_call( decompo );
    }

    //! Code de l'évaluation proprement dite de la décomposition
    virtual void call( daex::Decomposition & decompo ) = 0;

protected:

    virtual void pre_call( daex::Decomposition & /*decompo*/ ) 
    {}

    virtual void post_call( daex::Decomposition & decompo )
    {
        decompo.plan().search_steps( decompo.get_number_evaluated_nodes() );
    }

protected:

    //! Renvoie le nombre d'atomes communs entre le goal final et un goal donné
    /* Cette méthode est virtuelle pure car elle est implémentée différemment
     * dans CPT et dans YAHSP, du fait des structures de données différentes
     * utilisées pour maintenir l'état courant.
     */
    unsigned int distance_to_goal_Hamming( BitArray state );

    //! Fitness des décompositions dont on arrive à construire le plan
    //! Note: the call to decomposition.fitness(...) automatically validate the fitness
    double fitness_feasible( daex::Decomposition & decompo );

    //! Fitness des décompositions dont on arrive pas à construire le plan
    //! Note: the call to decomposition.fitness(...) automatically validate the fitness
    double fitness_unfeasible( daex::Decomposition & decompo, BitArray state );

    double fitness_unfeasible_too_long( );
    double fitness_unfeasible_intermediate( daex::Decomposition & decompo );
    double fitness_unfeasible_final( daex::Decomposition & decompo );

protected:

    //! Taille maximum d'une décomposition
    unsigned int _l_max;

    //! b_max for searchs within the decomposition
    unsigned int _b_max_in;
    
    //! b_max for the very last search towards the end goal
    unsigned int _b_max_last;

    //! Paramètre pondérant le compteur k lors du calcul de fitness quand le plan n'est pas trouvé
    // TODO qualité de la pondération ? pourquoi ce paramètre et pourquoi fixé à 10 ?
    double _unknown_parameter;

    double _fitness_penalty;

protected:

    bool _sequential;

    // fields moved to Decomposition
    /*
protected:

    //! compteur de goals
    unsigned int _k;

    //! compteur de goals utiles
    unsigned int _u;

    //! compteur des tentatives de recherche
    unsigned int _B;
    */

}; // class daeCptYahspEval

#endif // __DAE_BIND_YAHSP_CPT_H__

