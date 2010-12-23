
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


void bindDaeYahspCpt( daex::pddlLoad & pddl, std::string solver /*= SOLVER_YAHSP*/ )
{
    assert( solver == SOLVER_YAHSP || 
            solver == SOLVER_CPT );

    /* 
     * Attention : du fait que YAHSP est utilisé dans pddlLoad comme bibliothèque de parsing
     * on ne réinitialise pas ici le solveur.
    */

    // le nombre d'atomes précédemment initialisés par le parsing du pddl
    // doit etre égal au nombre de fluent chargé par le solveur
    assert( pddl._atoms.size() == fluents_nb );

    // parcours les atomes où lier
    for( unsigned int i=0; i < fluents_nb; ++i ) {

        Fluent ** corresponding_f = std::find( fluents, fluents + fluents_nb, *pddl._atoms[i] );

        // on doit en trouver un, sinon il y a un problème bloquant
        assert( corresponding_f != fluents + fluents_nb );

        pddl._atoms[i]->store_fluent_of( solver, fluents[i] );
            
    } // for iatom in _atoms
}


bool operator==( const Fluent * fluent, const daex::Atom & atom )
{
    // si les noms ne correspondent pas
    if( fluent->atom->predicate->name != atom.predicate()->name() ) {
        return false;
    }
    
    // si les noms correspondent
        
    // les arités doivent etres égales, sinon il y a un problème
    assert( fluent->atom->terms_nb == atom.objects().size() );
    assert( atom.objects().size() == atom.predicate()->arity() );
    
    // In ipc2008/tempo-sat/parcprinter-strips, there exists constants atoms (= predicates with null arity), 
    // so that this assert may not always be useful
    //assert( fluent->atom->terms_nb > 0 );

    // cherche des termes non égaux
    for( unsigned int i = 0; i < fluent->atom->terms_nb; ++i ) {

        // si on en trouve
        if( *( fluent->atom->terms[i] ) != *( atom.objects().at(i) ) ) {
            return false;
        }
    } // for i in fluent.terms

    // trouvé aucune différence => égalité
    return true;
}

bool operator!=( const PDDLTerm & term, const daex::pddlObject & object )
{
    return term.name != object.value();
    // Note : a priori, ça ne sert à rien de vérifier l'égalité des types car les prédicats ont des noms uniques
}

//! Fitness des décompositions dont on arrive à construire le plan
double daeCptYahspEval::fitness_feasible( daex::Decomposition & decompo )
{
    unsigned int Q = solution_plan->makespan;
    assert( Q > 0 );

    //    std::cout << "SEQUENTIAL=" << this->_sequential;

    //    std::cout << "MAKESPAN=" << Q << "LMAX=" << _l_max << "BMAX=" << _b_max << "(" << (double)_b_max << ")";
    // JACK the code uses a factor GCD/LCM on the first makespan used, but not on the second one, while the paper uses the same makespan
    //    return (double)Q + ( (double)decompo.size() - (double)_u + 1.0 ) / (double)Q + (double)_B / (double)_l_max * (double)_b_max ;
    if (this->_sequential == true) return (double)Q + ( (double)decompo.size() - (double)_u + 1.0 ) / (double)Q + (double)_B / (double)_l_max * (double)_b_max ;
    else return (double)Q * pddl_domain->time_gcd / pddl_domain->time_lcm + ( (double)decompo.size() - (double)_u + 1.0 ) / (double)Q + (double)_B / (double)_l_max * (double)_b_max ;

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
    return /*1e99 +*/ (double)_unknown_parameter * /*_k * */ (double)distance_to_goal_Hamming( state ) + (double)decompo.size() - (double)_u; 
}

//! Fitness des décompositions dont la taille dépasse l_max
double daeCptYahspEval::fitness_unfeasible_too_long( )
{
  // FIXME there is a unknown parameter here, *2 is hardcoded, we need to put it in the interface
  return _fitness_penalty * ((double)_l_max + 1.0) * (double)_l_max * (double)_unknown_parameter * 2;
}

//! Fitness des décompositions qui ont échoué avant le dernier goal de la décomposition
double daeCptYahspEval::fitness_unfeasible_intermediate( )
{
  return _fitness_penalty * ((double)_l_max - (double)_u + 1.0) * (double)_l_max * (double)_unknown_parameter;
}

//! Fitness des décompositions qui ont échoué à atteindre le goal final du problème
double daeCptYahspEval::fitness_unfeasible_final( )
{
  return _fitness_penalty * ((double)_l_max - (double)_u + 1.0);
}

//! Renvoie le nombre d'atomes communs entre le goal final et un goal donné
unsigned int daeCptYahspEval::distance_to_goal_Hamming( BitArray state )
{
    unsigned int nb = goal_state_nb;

    for( unsigned int i = 0; i < goal_state_nb; ++i ) {
        if ( bitarray_get( state, goal_state[i] ) != 0 ) {
            nb--;
        }
    }

    return nb;
}

