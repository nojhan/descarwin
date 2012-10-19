
#ifndef __DAEX_MUTATION_DELATOM_H__
#define __DAEX_MUTATION_DELATOM_H__

#include <iostream>

#include <eo>

#include "core/decomposition.h"

#include <algorithm>
#include <ostream>
namespace daex {


template<class EOT>
class MutationDelAtom: public eoMonOp<EOT>
{
public:

    MutationDelAtom( double proba_del_atom ) : _proba_del_atom( proba_del_atom ) 
    {
        if( _proba_del_atom < 0 || _proba_del_atom > 1 ) {
            std::ostringstream msg;
            msg << "Probability to delete an atom =" << _proba_del_atom << ", should be in [0,1]"; 
            throw std::range_error( msg.str() );
        }
    }

    bool operator()( EOT & decompo )
    {
#ifndef NDEBUG
    eo::log << eo::debug << "d";
    eo::log.flush();

    eo::log << eo::xdebug << " DelAtom:" << std::endl << "\tBefore: ";
    simplePrint( eo::log << eo::xdebug, decompo );
#endif

    if( decompo.empty() ) {
        return false;
    }

    bool is_modified;

    Decomposition::iterator igoal = decompo.begin();

    // return 0 if no goal reached
    int unreach_goal = std::min( 
                static_cast<unsigned int>(decompo.size()), 
                static_cast<unsigned int>( decompo.last_reached() ) + 1 
                );

#ifndef NDEBUG
    eo::log << eo::xdebug << "\tunreached goal: " << unreach_goal << std::endl;
#endif
//    // if we found a plan to the last goal or to the end state 
//    if( taken_goal >= decompo.size() ) {
//        // do not mutate
//        return false;
//    }

    // for each reached goal in the decomposition
    int i = 0;
    while( i < unreach_goal ) {

        // try to delete and atom
        if( igoal->size() > 1 
              && 
            rng.flip( _proba_del_atom / decompo.size() ) 
          ) {

 #ifndef NDEBUG
            unsigned int j = rng.random( igoal->size() );
            eo::log << eo::xdebug << "\tdelete the " << j << "th atom in the " << i << "th goal" << std::endl;
            eo::log << eo::xdebug << "\t\tBefore: " << *igoal << std::endl;
            igoal->erase( igoal->iter_at( j ) );
            eo::log << eo::xdebug << "\t\t After: " << *igoal << std::endl;
#else
            // supprime un atome au hasard dans le goal courant
            igoal->erase(
                    igoal->iter_at(
                        rng.random( igoal->size() ) 
                    )
                );
#endif
            is_modified = true;
        }

        igoal++;
        i++;
    } // for i in unreach_goal

#ifndef NDEBUG
        eo::log << eo::xdebug << "\t After: ";
        simplePrint( eo::log << eo::xdebug, decompo );
#endif


    return is_modified;
};

protected:
    double _proba_del_atom;
};


//! Delete one atom randomly chosen in [0,last_reached[
template<class EOT>
class MutationDelOneAtom: public eoMonOp<EOT>
{
public:
    bool operator()( EOT & decompo ) 
    {
    /*if( decompo.empty() ) {
        return false;
    }*/

#ifndef NDEBUG
    eo::log << eo::debug << "d";
    eo::log.flush();
#endif

    assert( !decompo.empty() );

    int range;
    // if the decomposition has been modified since the last evaluation
    // then do not take into account the last_reached station
    if( decompo.invalid() ) {
        range = decompo.size();

    } else {
        // the decomposition has not been modified, we can consider using the last_reached goal
        // but we do not go over the size of the decomposition
        range = std::min( 
                static_cast<unsigned int>(decompo.size()), 
                static_cast<unsigned int>( decompo.last_reached() ) + 1 
                );
    }

    // random goal in [0,last_reached[ or [0,nb_goals[
    unsigned int i = rng.random( range );
    Decomposition::iterator goal_i = decompo.iter_at(i);

    // if the chosen goal has only one atom, skip the delete
    if( goal_i->size() <= 1 ) {
        return false;

    } else {
        // random atom in goal
        unsigned int a = rng.random( goal_i->size() );
    
        goal_i->erase( goal_i->iter_at( a ) );

	decompo.invalidate();

        return true;
    } // if decompo[i].size == 0
};
};


} // namespace daex

#endif // __DAEX_MUTATION_DELATOM_H__
