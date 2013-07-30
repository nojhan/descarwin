
#ifndef __DAE_BIND_YAHSP_CPT_H__
#define __DAE_BIND_YAHSP_CPT_H__

#include <sstream>
#include <stdexcept>
#include <cstdlib>
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
std::string timeValToString( TimeVal x ){
    char* buffer;
    size_t buffer_size;

    FILE* stream = open_memstream( & buffer, & buffer_size );

    if( stream == NULL ) {
        std::ostringstream msg;
        msg << "Cannot open memstream for printing TimeVal (" << x << ")";
        throw std::runtime_error( msg.str() );
    }

    // Attention : utilise opt.rationals et pddl_domain
    print_time( stream, x );

    fclose( stream );

    std::string res( buffer );

    return res;
};

//! Convertit un plan CPT/YAHSP en plan DAEx
daex::Plan convertPlanCptToDae( SolutionPlan plan );

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
template<class EOT>
class daeCptYahspEval : public eoEvalFunc< EOT>
{
public:
  daeCptYahspEval( unsigned int l_max_ = 20, unsigned int b_max_in = 10, unsigned int b_max_last = 30, double fitness_weight = 10, double fitness_penalty = 1e60, bool sequential = false ) : _l_max( l_max_ ), _b_max_in(b_max_in),_b_max_last( b_max_last),_unknown_parameter(fitness_weight),_fitness_penalty( fitness_penalty ),_sequential( sequential ) { }

    virtual ~daeCptYahspEval() {}

public:
    unsigned int l_max() const { return _l_max; }
     void l_max( unsigned int l ) { _l_max = l; }

public:
    //! Proxy EO avec appels aux timers
    virtual void operator() ( EOT & decompo )=0; 
//      {
//          pre_call( decompo );
//          call( decompo );
//          post_call( decompo );
//      }

    //! Code de l'évaluation proprement dite de la décomposition
    virtual void call( EOT & decompo )  = 0;

protected:

    virtual void pre_call( EOT & decompo ) =0;   
    

    virtual  void post_call( EOT & decompo ) =0;
     

protected:

    //! Renvoie le nombre d'atomes communs entre le goal final et un goal donné
    /* Cette méthode est virtuelle pure car elle est implémentée différemment
     * dans CPT et dans YAHSP, du fait des structures de données différentes
     * utilisées pour maintenir l'état courant.
     */
     unsigned int distance_to_goal_Hamming( BitArray state ){
    unsigned int nb = goal_state_nb;

    for( unsigned int i = 0; i < (unsigned int) goal_state_nb; ++i ) {
        if ( bitarray_get( state, goal_state[i] ) != 0 ) {
            nb--;
        }
    }

    return nb;
};

     double convertMakespan(TimeVal m) {return (double)m * pddl_domain->time_gcd / pddl_domain->time_lcm;};

    //! Fitness des décompositions dont on arrive à construire le plan
     //! Note: the call to decomposition.fitness(...) automatically validate the fitness
      double fitness_feasible( EOT & decompo ) {
    // Pour travailler sur les valeurs affichables, on va partout ramener à la bonne valeur
    //unsigned int Q = solution_plan->makespan;
    double Q = (double) decompo.plan().makespan() * pddl_domain->time_gcd / pddl_domain->time_lcm;
    assert( Q > 0 );

    //    std::cout << "SEQUENTIAL=" << this->_sequential;

    //    std::cout << "MAKESPAN=" << Q << "LMAX=" << _l_max << "BMAX=" << _b_max << "(" << (double)_b_max << ")";
    // JACK the code uses a factor GCD/LCM on the first makespan used, but not on the second one, while the paper uses the same makespan
    //    return (double)Q + ( (double)decompo.size() - (double)_u + 1.0 ) / (double)Q + (double)_B / (double)_l_max * (double)_b_max ;
    
    // Non : Q doit soit être partout ramené à sa valeur affichée (en multipliant, divisant...)
    // soit ne jamais l'être. Pour avoir des valeurs plus sympas, j'ai modifié Q au départ.
    //if (this->_sequential == true) return (double)Q + ( (double)decompo.size() - (double)_u + 1.0 ) / (double)Q + (double)_B / (double)_l_max * (double)_b_max ;
    //else return (double)Q * pddl_domain->time_gcd / pddl_domain->time_lcm + ( (double)decompo.size() - (double)_u + 1.0 ) / (double)Q + (double)_B / (double)_l_max * (double)_b_max ;
    return Q + ( (double)decompo.size() - (double)decompo.get_number_useful_goals() + 1.0 ) / Q + (double)decompo.get_number_evaluated_nodes() / ( (double)_l_max * (double)decompo.b_max() );

// canonical value return Q ;
};

    //! Fitness des décompositions dont on arrive pas à construire le plan
    //! Note: the call to decomposition.fitness(...) automatically validate the fitness

      double fitness_unfeasible( EOT & decompo, BitArray state ){
    // TODO what is _k in the fitness ??
    // JACK the paper use k, but not the code, the code does not uses the distance measure, only consider U as a variable, does not 	evaluate too long decomposition and uses several unfeasible fitness:
    /* if the size of the decomposition is greater than l_max: 
           penalty * (l_max+1)       *  l_max * 10 * 2
     * when a subplan cannot be found before the last goal in the decomposition: 
           penalty * (l_max - u + 1) *  l_max * 10
     * when a subplan cannot be found between the last goal and the final goal: 
           penalty * (l_max - u + 1)
     * if the compression fails (but does it really happens?): 
           penalty * (l_max+1)
     */
    // NOTE in the current code, the compression is not supposed to fail, thus there is an assert that check it, thus we haven't 	implemented the last fitness
    // unkonw_parameter = 10 (in the code and in the paper)
     return /*1e99 +*/  (double)_unknown_parameter * /*_k * */ (double)distance_to_goal_Hamming( state ) + (double)decompo.size() - 	(double)decompo.get_number_useful_goals(); 

	} ;

      double fitness_unfeasible_too_long( ){
  // FIXME there is a unknown parameter here, *2 is hardcoded, we need to put it in the interface
  return _fitness_penalty * ((double)_l_max + 1.0) * (double)_l_max * (double)_unknown_parameter * 2;
};
      double fitness_unfeasible_intermediate( EOT& decompo ){
  return _fitness_penalty * ((double)_l_max - (double)decompo.get_number_useful_goals() + 1.0) * (double)_l_max * (double)_unknown_parameter;
};
      double fitness_unfeasible_final( EOT & decompo ){
  return _fitness_penalty * ((double)_l_max - (double)decompo.get_number_useful_goals() + 1.0);
};

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

