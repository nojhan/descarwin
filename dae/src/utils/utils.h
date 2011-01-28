
#ifndef __DAEX_UTILS_H__
#define __DAEX_UTILS_H__

#include "core/atom.h"

//extern "C" {
#include <src/structs.h>
//} // extern "C"


namespace daex {

//! Return true if candidate is mutex with at least one of the atom in goal(first,last)
template<class T>
  bool has_one_mutex_in( Atom* const& candidate, T const first, T const last)
{
  for( T it=first; it!=last; ++it  ) {
        // il faut absolument passer par une variable intermédiaire, sans quoi le compilo
        // merde complètement

        Atom* a = *it;

        // Warning: fmutex does not test if an atom is mutex with itself, thus we have to do it manually
        if( candidate == a 
            || 
            fmutex( candidate->fluent(),
                            a->fluent() ) ) {

            return true;
        }
    }
    return false;
}



//! Return the iterator to a randomly chosen atom in applicants that appear to be [no]mutex with at least one of the atom in goal
/**
 * If search_mutex is true, search for mutex, else, for nomutex.
 * Returns applicants.end() if no atom with the correct property can be found.
 * @see draw_until_nomutex
 * @see draw_until_mutex
 */
/*
template< class T1, class T2 >
typename T1::iterator draw_until_mutex_if( T1 const& applicants, T2 const& goal, unsigned int maxtry, bool search_mutex )
*/
//template< class T2 >
//std::vector<Atom*>::const_iterator draw_until_mutex_if( std::vector<Atom*> const& applicants, T2 const& goal, unsigned int maxtry, bool search_mutex )
/*
template< class T1, class T2 >
typename T1::const_iterator draw_until_mutex_if( T1 const& applicants, T2 const& goal, unsigned int maxtry, bool search_mutex )
{
    assert( maxtry <= applicants.size() );

    typedef std::list< typename T1::const_iterator > IterList;
    //typedef std::list< std::vector<Atom*>::const_iterator > IterList;
    IterList candidates;
    for( 
//typename T1::iterator
std::vector<Atom*>::const_iterator it = applicants.begin(), end = applicants.end(); it != end; ++it ) {
        candidates.push_back( it );
    }

    // puisque maxtry <= applicants.size, on a pas besoin de tester si applicants.size > 0
    while( maxtry > 0 ) {

        // choisir un des candidats restant au hasard
        //IterList::iterator itapp;
        typename IterList::iterator itapp;
        itapp = candidates.begin();
        std::advance( itapp, rng.random( candidates.size() ) );
        
        // si on cherche les mutex ET qu'il y en a au moins un dans le goal
        // OU
        // si on cherche les nomutex ET qu'il n'y en a pas dans le goal
        if(   search_mutex &&  has_one_mutex_in( **itapp, goal )
           ||
             !search_mutex && !has_one_mutex_in( **itapp, goal ) ) {
            // on en a trouvé un, on renvoi l'itérateur correspondant
            return *itapp;

        } else {
            maxtry--;
            // on ne veut pas ré-éssayer au hasard cet atome, on le supprime
            candidates.erase( itapp );
        }
    } // while maxtry

    return applicants.end();
}
*/

    
template< class T1, class T2 >
typename T1::const_iterator draw_until_mutex_if( T1 const& applicants, T2 const goal_first, T2 const goal_last, unsigned int maxtry, bool search_mutex )
{
    assert( maxtry <= applicants.size() );

    typedef std::list< typename T1::const_iterator > IterList;
    //typedef std::list< std::vector<Atom*>::const_iterator > IterList;
    IterList candidates;
    for( /*typename T1::iterator*/std::vector<Atom*>::const_iterator it = applicants.begin(), end = applicants.end(); it != end; ++it ) {
        candidates.push_back( it );
    }

    // puisque maxtry <= applicants.size, on a pas besoin de tester si applicants.size > 0
    while( maxtry > 0 ) {

        // choisir un des candidats restant au hasard
        //IterList::iterator itapp;
        typename IterList::iterator itapp;
        itapp = candidates.begin();
        std::advance( itapp, rng.random( candidates.size() ) );
        
        // si on cherche les mutex ET qu'il y en a au moins un dans le goal
        // OU
        // si on cherche les nomutex ET qu'il n'y en a pas dans le goal
        if(   ( search_mutex &&  has_one_mutex_in( **itapp, goal_first, goal_last ) )
           ||
	      ( !search_mutex && !has_one_mutex_in( **itapp, goal_first, goal_last ) ) ) {

#ifndef NDEBUG
            if( !search_mutex ) {
                for( T2 iatom = goal_first; iatom != goal_last; ++iatom ) {
                     assert( **itapp != *iatom );
                }
            }
#endif
            // on en a trouvé un, on renvoi l'itérateur correspondant
            return *itapp;

        } else {
            maxtry--;
            // on ne veut pas ré-éssayer au hasard cet atome, on le supprime
            candidates.erase( itapp );
        }
    } // while maxtry

    return applicants.end();
}


//! Return the iterator to a randomly chosen atom in applicants which is not MUTEX with none of the atoms in goal. @see draw_until_mutex_if
template< class T1, class T2 >
typename T1::const_iterator draw_until_nomutex( T1 const& applicants, T2 const goal_first, T2 const goal_last, unsigned int maxtry = 0 )
{
    if( maxtry == 0 ) { maxtry = applicants.size(); }

    return draw_until_mutex_if( applicants, goal_first, goal_last, maxtry, false );
}


//! Build a random subset of non-mutex atoms from a candidate list. 
/*! Among mutex, atoms are chosen randomly, in an iterative manner.
 * Thus, the subset is the larg we can get with a random walk in the candidate list.
 */
//std::vector<Atom*> nomutex( std::vector<Atom*> candidate_atoms );
//std::list<Atom*> nomutex( std::vector<Atom*> candidate_atoms );
//std::list<Atom*> nomutex( std::list<Atom*> candidate_atoms );
template< class T >
T nomutex( T const& candidates )
{
    // copie dans une liste pour pouvoir faire des erases sans pb de segfault, par la suite
    std::list<Atom*> candidate_atoms;
    std::copy( candidates.begin(), candidates.end(), std::back_inserter( candidate_atoms ) );

    assert( candidate_atoms.size() > 0 );

    //typedef std::vector<Atom*> T;

    T nomutex_atoms;

    while( candidate_atoms.size() > 0 ) {

        // le i-ème atome
        std::list<Atom*>::iterator ca_it = candidate_atoms.begin();
        // un atome au hasard
        std::advance(ca_it, rng.random( candidate_atoms.size() ) );

        // s'il ne reste pas qu'un seul candidat, on cherche les mutex, sinon on ne supprimera que le candidat
        if( candidate_atoms.size() > 1 ) {

            // vecteur contenant les itérateurs vers les atomes à supprimer après avoir tout parcouru
            typedef std::vector< std::list<Atom*>::iterator > ItersVec;
            ItersVec iters_to_erase;
            iters_to_erase.reserve( candidate_atoms.size() );

            // parcours les atomes candidats
            for( std::list<Atom*>::iterator it = candidate_atoms.begin(), end = candidate_atoms.end(); it != end; ++it ) {
            
                // ce ne sont pas les meme
                if( ca_it != it ) {
                    Atom* ca = *ca_it;
                    Atom* a = *it;

                    assert( ca != a );
                    assert( ca->fluent() != a->fluent() );

                    // si le candidat est mutex avec cet atome
                    if( fmutex(
                                ca->fluent(), 
                                a->fluent() 
                              )
                      ) {

                        // le supprime de la liste des candidats
                        //candidate_atoms.erase( it );
                        iters_to_erase.push_back( it );
                    } // if mutex
                } // if ca_it != it
            } // for it in candidate_atoms
            
            // supprime effectivement les atomes mutex et l'atome gardé de la liste des candidats
            for( typename ItersVec::iterator itte = iters_to_erase.begin(), iend = iters_to_erase.end(); itte != iend; ++itte ) {
                candidate_atoms.erase( *itte );
            }
            iters_to_erase.clear();

        } // if candidate_atoms.size > 1

        // garde le i-ème atome
        nomutex_atoms.push_back( *ca_it );
        // le supprime des candidats
        candidate_atoms.erase( ca_it );

    } // while candidate_atoms.size != 0

/*
#ifndef NDEBUG
    // Check that the resulting list effectively contains no atoms that are pairwise mutualy exlusives
    for(     typename T::iterator iatom  = nomutex_atoms.begin(), end  = nomutex_atoms.end(); iatom != end;  ++iatom  ) {
        for( typename T::iterator iatom2 = ++iatom,               end2 = nomutex_atoms.end(); iatom != end2; ++iatom2 ) {
            //assert( ! fmutex( (*iatom )->fluent(), (*iatom2)->fluent() ) );
            if( (*iatom )->fluent() == (*iatom2)->fluent() ) { exit(1); }
        }
    }
#endif
*/
    return nomutex_atoms;
}


//! Build a goal from a random atoms subset of random size, chosen in a container of atoms
/*!
 * First a random number of atoms in drawn, then atoms are randomly chosen among candidates.
 * The earliest start time of the goal is updated.
 */
template< class T>
Goal random_subset( T candidate_atoms )
{
#ifndef NDEBUG
    eo::log << eo::xdebug << "\t\trandom_subset of " << candidate_atoms.size() << " candidates:";
#endif

    // extraire un sous-ensemble aléatoire de candidate_atoms, ce qui formera le goal à insérer
    // la date au plus tot est forcément celle utilisée pour le choix des atomes
    Goal goal(0);
    TimeVal goaldate = 0;
    
    // d'abord un _nombre_ aléatoire d'atomes à choisir dans [1, candidate_atoms.size]
    unsigned int N = 1 + rng.random( candidate_atoms.size() );

#ifndef NDEBUG
    eo::log << eo::xdebug << " " << N << " atoms chosen, ";
#endif

    unsigned int i = 0;
    do {
        // Un atome au hasard dans ceux qui n'ont pas été choisis
        unsigned int j = rng.random( candidate_atoms.size() );

        typename T::iterator it = candidate_atoms.begin();
        std::advance(it,j);
        goal.push_back(*it);

        // mise à jour incrémentale de la date du goal
        goaldate = std::max( (*it)->earliest_start_time(), goaldate );

        assert(goaldate != 0);

        // supprime l'élément choisi
        candidate_atoms.erase( it );

        i++;
    } while( i < N );


    // ajoute la date au goal
    goal.earliest_start_time(goaldate);

#ifndef NDEBUG
	eo::log << eo::xdebug << "goal final date: " << goal.earliest_start_time();
    eo::log << eo::xdebug << std::endl;
#endif
    return goal;
}


template<class T>
void assert_nomutex( T begin, T end ) 
//void assert_nomutex( Goal::iterator begin, Goal::iterator end ) ;

{
    // check if there is no atom that has mutex in the same goal or a duplicate
    T iend = end;
    iend--;
    for( T iatom  = begin; iatom != iend;  ++iatom  ) {
        T begin2 = iatom;
        begin2++;
        //if( begin2 == goal.end() ) { break; }
        for( T iatom2 = begin2; iatom2 != end; ++iatom2 ) {
            Atom* a = *iatom;
            Atom* b = *iatom2;
            assert( ! fmutex( a->fluent(), b->fluent() ) );
        }
    }
}


template<class T>
void assert_noduplicate( T begin, T end ) 
{
    // check if there is no atom that has mutex in the same goal or a duplicate
    T atom_end = end;
    atom_end--;

    for( T iatom = begin; iatom != atom_end; ++iatom ) {

        T jatom = iatom;
        jatom++;
        for( T iatom2 = jatom; iatom2 != end; ++iatom2 ) {
            assert( *iatom != *iatom2 );
        }
    }
}

} // namespace daex

#endif // __DAEX_UTILS_H__
