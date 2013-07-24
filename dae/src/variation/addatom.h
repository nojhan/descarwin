
#ifndef __DAEX_MUTATION_ADDATOM_H__
#define __DAEX_MUTATION_ADDATOM_H__

#include <algorithm>

#include "utils/pddl_load.h"
#include "core/atom.h"
#include "core/goal.h"
#include "core/decomposition.h"

namespace daex {
  // JACK: Conforme au code et pas à l'article.
  
template<class EOT>
class MutationChangeAddAtom: public eoMonOp<EOT>
{
public:
    MutationChangeAddAtom( 
            const ChronoPartition & times, 
            double proba_change, /*double proba_add, */
            unsigned int maxtry_search_candidate = 0, unsigned int maxtry_search_mutex = 0
        ) 
      :_proba_change(proba_change), /*_proba_add(proba_add), */ _times(times), _maxtry_search_candidate(maxtry_search_candidate), _maxtry_search_mutex(maxtry_search_mutex)
    {}

    bool operator()( EOT & decompo )
    {
                                   #ifndef NDEBUG
                                           eo::log << eo::debug << "a"; eo::log.flush();
                                           eo::log << eo::xdebug << " ChangeAddAtom:" << std::endl << "\tBefore: ";
                                           simplePrint( eo::log << eo::xdebug, decompo );
                                   #endif
        bool is_modified = false;
        for (unsigned int k=0; k < decompo.size(); ++k) {
	    double r = rng.uniform();
                                   assert( decompo.iter_at(k)->size() != 0);
            if (r < _proba_change) {
                                   #ifndef NDEBUG
                                                   eo::log << eo::xdebug << "\tTry to modify the " << k << "th goal";
                                   #endif
                is_modified =  false;
                //typename EOT::AtomType goal=*decompo.iter_at(k);

                typename EOT::AtomType::iterator candy = decompo.iter_at(k)->begin();
                //std::advance(candy, rng.random(goal.size()));
                std::advance(candy, rng.random( decompo.iter_at(k)->size() ) );

                // création de la liste des atomes à la date du candidat privée du candidat lui-même
                                   assert(_times.find(candy->earliest_start_time()) != _times.end()); // on vérifie que la map est bien constituée
                std::vector<Atom> at_date = _times.find(candy->earliest_start_time())->second;
                at_date.erase(std::find(at_date.begin(), at_date.end(), *candy));

                // tentative d'ajout d'un nouvel atome non mutex avec aucun des atomes du goal privé du candidat
                //std::vector<Atom>::const_iterator icandidate = draw_until_nomutex( at_date, goal, std::min( _maxtry_search_mutex, at_date.size() ) );
                std::vector<Atom>::const_iterator icandidate = draw_until_nomutex( at_date, decompo.iter_at(k)->begin(), decompo.iter_at(k)->end(), std::min( _maxtry_search_mutex, static_cast<unsigned int>(at_date.size()) ) );
                if (icandidate != at_date.end()) {
                                   #ifndef NDEBUG
                                                       eo::log << eo::xdebug << std::endl << "\tbefore:" << decompo << std::endl;
                                                       eo::log << eo::xdebug << ", replaced: " << *candy << " by " << *icandidate;
                                   #endif
                    *candy=*icandidate;
                    is_modified = true;
                                   #ifndef NDEBUG
                                                       eo::log << eo::xdebug << std::endl << "\t after:" << decompo << std::endl;
                                   #endif
                }
                                   #ifndef NDEBUG
                                                   else {eo::log << eo::xdebug << " cannot replace (all atoms are mutex)" ;}
                                   #endif
                r = rng.uniform();
                if (r < _proba_change) {
                    //icandidate = draw_until_nomutex( at_date, goal, std::min( _maxtry_search_mutex, at_date.size() ) );
                    icandidate = draw_until_nomutex( at_date, decompo.iter_at(k)->begin(), decompo.iter_at(k)->end(), std::min( _maxtry_search_mutex, static_cast<unsigned int>(at_date.size()) ) );
                    if (icandidate != at_date.end()) {
                                   #ifndef NDEBUG
                                                           eo::log << eo::xdebug << std::endl << "\tbefore:" << decompo << std::endl;
                                                           eo::log << eo::xdebug << ", added: " <<  *icandidate;
                                   #endif
                        decompo.iter_at(k)->push_back( *icandidate );
                        is_modified = true;
                                   #ifndef NDEBUG
                                                           eo::log << eo::xdebug << std::endl << "\t after:" << decompo << std::endl;
                                   #endif
                    }
                                   #ifndef NDEBUG
                                                       else {eo::log << eo::xdebug << " cannot add (all atoms are mutex) " ;}
                                   #endif
                } // if r < proba_change
                                   #ifndef NDEBUG
                                                   eo::log << eo::xdebug << std::endl;
                                   #endif
            } // if r < proba_change
        } // for k in decompo.size

#ifndef NDEBUG
        eo::log << eo::xdebug << "\tAfter: ";
        simplePrint( eo::log << eo::xdebug, decompo );

        /* Already checked at the beginning of an eval
        eo::log << eo::xdebug << "Check goal consistency in addatom...";
        eo::log.flush();
        for( typename EOT::iterator igoal = decompo.begin(), goal_end = decompo.end(); igoal != goal_end; ++igoal ) {
            assert_noduplicate( igoal->begin(), igoal->end() );
            assert_nomutex(     igoal->begin(), igoal->end() );
        }
        eo::log << " ok" << std::endl;
        */
#endif
	decompo.invalidate();
        return is_modified;
    }

 private:
    double _proba_change;
    
    //    double _proba_add;

    const ChronoPartition & _times;

    unsigned int _maxtry_search_candidate;
    unsigned int _maxtry_search_mutex;
};
}
#endif
