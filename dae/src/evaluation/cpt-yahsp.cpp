
#include <sstream>
#include <stdexcept>
#include <cstdlib>

#include "cpt-yahsp.h"


//! Convertit un TimeVal en string, en utilisant print_time de CPT/YAHSP
std::string timeValToString( TimeVal x )
{
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
}


//! Fitness des décompositions dont on arrive à construire le plan
double daeCptYahspEval::fitness_feasible( daex::Decomposition & decompo )
{
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
    return Q + ( (double)decompo.size() - (double)decompo.get_number_useful_goals() + 1.0 ) / Q + (double)decompo.get_number_evaluated_nodes() / ( (double)_l_max * (double)_b_max );
}

//! Fitness des décompositions dont on arrive pas à construire le plan
double daeCptYahspEval::fitness_unfeasible( daex::Decomposition & decompo, BitArray state )
{
    // TODO what is _k in the fitness ??
    // JACK the paper use k, but not the code, the code does not uses the distance measure, only consider U as a variable, does not evaluate too long decomposition and uses several unfeasible fitness:
    /* if the size of the decomposition is greater than l_max: 
           penalty * (l_max+1)       *  l_max * 10 * 2
     * when a subplan cannot be found before the last goal in the decomposition: 
           penalty * (l_max - u + 1) *  l_max * 10
     * when a subplan cannot be found between the last goal and the final goal: 
           penalty * (l_max - u + 1)
     * if the compression fails (but does it really happens?): 
           penalty * (l_max+1)
     */
    // NOTE in the current code, the compression is not supposed to fail, thus there is an assert that check it, thus we haven't implemented the last fitness
    // unkonw_parameter = 10 (in the code and in the paper)
    return /*1e99 +*/ (double)_unknown_parameter * /*_k * */ (double)distance_to_goal_Hamming( state ) + (double)decompo.size() - (double)decompo.get_number_useful_goals(); 
}

//! Fitness des décompositions dont la taille dépasse l_max
double daeCptYahspEval::fitness_unfeasible_too_long( )
{
  // FIXME there is a unknown parameter here, *2 is hardcoded, we need to put it in the interface
  return _fitness_penalty * ((double)_l_max + 1.0) * (double)_l_max * (double)_unknown_parameter * 2;
}

//! Fitness des décompositions qui ont échoué avant le dernier goal de la décomposition
double daeCptYahspEval::fitness_unfeasible_intermediate( daex::Decomposition & decompo )
{
  return _fitness_penalty * ((double)_l_max - (double)decompo.get_number_useful_goals() + 1.0) * (double)_l_max * (double)_unknown_parameter;
}

//! Fitness des décompositions qui ont échoué à atteindre le goal final du problème
double daeCptYahspEval::fitness_unfeasible_final( daex::Decomposition & decompo )
{
  return _fitness_penalty * ((double)_l_max - (double)decompo.get_number_useful_goals() + 1.0);
}

//! Renvoie le nombre d'atomes communs entre le goal final et un goal donné
unsigned int daeCptYahspEval::distance_to_goal_Hamming( BitArray state )
{
    unsigned int nb = goal_state_nb;

    for( unsigned int i = 0; i < (unsigned int) goal_state_nb; ++i ) {
        if ( bitarray_get( state, goal_state[i] ) != 0 ) {
            nb--;
        }
    }

    return nb;
}

