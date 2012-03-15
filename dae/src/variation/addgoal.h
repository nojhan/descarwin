
#ifndef __DAEX_MUTATION_ADDGOAL_H__
#define __DAEX_MUTATION_ADDGOAL_H__

#include <eo>

#include "utils/pddl_load.h"
#include "core/decomposition.h"
#include "utils/utils.h"

#include <algorithm>
#include <sstream>
#include <iomanip>
namespace daex {

template<class EOT>
class MutationAddGoal: public eoMonOp<EOT>
{
public:
    MutationAddGoal(
            const ChronoPartition & times,
            unsigned int radius = 1/*,
            unsigned int l_max = 20*/
    ) : _times(times), _radius( radius )/*, _l_max(l_max)*/ {}

    bool operator()( EOT & decompo ) {
    #ifndef NDEBUG
    eo::log << eo::debug << "A";
    eo::log.flush();
#endif
    //std::cout << "DECOMPO=" << decompo << std::endl;
    // TODO introduire des faux goals pour représenter l'init et le end ? Afin d'éviter de faire des branchements conditionnels
   
    /*****************************************************************************************
     * Si la décomposition est déjà à la taille max, la mutation n'ajoute aucun nouveau goal *
     * Note : décidé en réunion du 2010-06-15 de ne pas limiter la taille hors de l'init     *
     * TODO vérifier qu'on ne veut pas limiter la taille de la décomposition dans addGoal    *
     *****************************************************************************************/
    //if( decompo.size() == _l_max ) {
        // aucune modification de faite
        //return false;


    /************************************
     * aucun goal dans la décomposition *
     ************************************/
    //} else{
        // should not occurs, because delGoal cannot remove the last goal
        // TODO it is surprising that we do not want an empty decomposition, as the subsolver may find a plan on easy instances
        assert( ! decompo.empty() );
        /*
        if( decompo.empty() ) { 

            // on utiliser rbegin pour avoir la date la plus grande, ce que garantie std::map
            // il doit exister au moins un atome dans la liste
            assert( _times.rbegin()->second.size() > 0 );

            // insertion d'un goal à une date au hasard 
            // entre 0 et la date au plus tot la plus tardive possible
            Goal new_goal = random_goal(0, _times.rbegin()->first);
            decompo.insert( decompo.begin(), new_goal );
        */

        /******************************************
         * au moins un goal dans la décomposition *
         ******************************************/
        //} else {

            // NOTE : l'hypothèse ici, est que s'il y a eu crossover, alors on a pris soin de vider les sous-plans, auquel cas on aura un last_reached = 0

            // dernier goal atteint lors de l'éval
            int last_reached = decompo.last_reached();

#ifndef NDEBUG
                eo::log << eo::xdebug << " AddGoal:" << std::endl;
                eo::log << eo::xdebug << "\tbefore: ";
                simplePrint( eo::log << eo::xdebug, decompo );
                eo::log << eo::xdebug << "\tlast reached: " << last_reached << std::endl;
#endif

            /****************************************************** 
             * N'a pas réussi à atteindre le premier goal         *
             * => construit un goal entre ]0, h1(premier goal)] *
             ******************************************************/
            if( last_reached == -1 ) {
                
#ifndef NDEBUG
                eo::log << eo::xdebug << "\tgenerate random goal in ] " << 0 << ", " << decompo.iter_at( 0 )->earliest_start_time() << " ]" << std::endl;
#endif


                Goal new_goal = random_goal(
                            0, 
                            decompo.iter_at( 0 )->earliest_start_time()
                        );


#ifndef NDEBUG
                eo::log << eo::xdebug << "\tgoal: " << new_goal.earliest_start_time() << "(" << new_goal.size() << ")" << std::endl;
                eo::log << eo::xdebug << "\tpush front" << std::endl;
#endif


                //decompo.insert( decompo.begin(), new_goal );
                decompo.push_front( new_goal );


            /*************************************
             * A atteint un goal ou l'état final *
             * => tire un goal entre j et j+1    *
             *************************************/
            } else {

                // choix au hasard d'un goal parmi les évaluables
                // Note : équivaut à tirer dans [ 0, last_reached ]
                unsigned int range = last_reached + 1;
                unsigned int j = rng.random( range );
                
#ifndef NDEBUG
                eo::log << eo::xdebug << "\tgoal index chosen: " << j << std::endl;
#endif

                TimeVal t1;
                TimeVal t2;


                // JACK addgoal, the article compare j and j+1 dates, the code compares j and j-1 dates
                // JACK code is using [t1,t2] for the last goal, and ]t1,t2] as in other cases. The first case seems desirable, what about the other?
                
                // CODE VERSION:
                if( j == 0 ) {
 
                    t1 = decompo.iter_at( j )->earliest_start_time();

#ifndef NDEBUG
                    eo::log << eo::xdebug << "\tgenerate random goal in ] " << 0 << ", " << t1 << " ]" << std::endl;
#endif

                    // draw a random goal in ] t1, t2 ]
                    Goal new_goal = random_goal( 0, t1 );

#ifndef NDEBUG
                    eo::log << eo::xdebug << "\tgoal: " << new_goal.earliest_start_time() << "(" << new_goal.size() << ")" << std::endl;
                    eo::log << eo::xdebug << "\tpush front" << std::endl;
#endif

                    // insert the new goal before the 0th element
                    decompo.push_front( new_goal );


                } else if( /*0 < */ j < decompo.size() ) {
                    
                    t1 = decompo.iter_at( j - 1 )->earliest_start_time();
                    t2 = decompo.iter_at( j )->earliest_start_time(); 

                    // JACK addgoal, the article does not specify what to do when t1 >= t2, the code version returns false and do nothing
                    
                    // proposed version for following the article' spirit: try to insert the goal between the two dates, whichever is the greater one
                    //Goal new_goal = random_goal( std::min(t1,t2), std::max(t1,t2) );

                    // code version:
                    if( t1 >= t2 ) {
#ifndef NDEBUG
                        eo::log << eo::xdebug << "\t" << t1 << ">=" << t2 << ", no insertion" << std::endl << std::endl;
#endif
                        return false;
                    }

#ifndef NDEBUG
                    eo::log << eo::xdebug << "\tgenerate random goal in ] " << t1 << ", " << t2 << " ]" << std::endl;
#endif

                    // draw a random goal in ] t1, t2 ]
                    Goal new_goal = random_goal( t1, t2 );

#ifndef NDEBUG
                    eo::log << eo::xdebug << "\tgoal: " << new_goal.earliest_start_time() << "(" << new_goal.size() << ")" << std::endl;
                    eo::log << eo::xdebug << "\tinsert before index: " << j << std::endl;
#endif

                    // insert the new goal before j (and after j-1)
                    decompo.insert( decompo.iter_at( j ), new_goal );



                } else { // if j == decompo.size() 
                    // the decomposition is feasible, a plan has been found until the final state
                    
                    // get the date of the last element
                    t1 = decompo.back().earliest_start_time();

                    // get the date just before in the chrono partition
                    ChronoPartition::const_iterator it1 = _times.find(t1);

                    // t1 obviously must exists
                    assert( it1 != _times.end() );

                    // if it1 is already at begin, do not consider the previous element
                    // note: remember that the first element of the chrono partition is zero, thus we never want it
                    assert( it1 != _times.begin());

                    ChronoPartition::const_iterator it0 = it1;

                    std::advance( it0, -1 );
                    
                    // get the corresponding date
                    TimeVal t0 = it0->first;

                    // get the greater date of the chrono partition
                    // remember that the STL implementation is a RB-tree, which is guaranted to be ordered
                    t2 = _times.rbegin()->first;

#ifndef NDEBUG
                    eo::log << eo::xdebug << "\tgenerate random goal in ] " << t0 << " ([ " << t1 << "), " << t2 << " ]" << std::endl;
#endif

                    // draw a random goal in [ t1, t2 ]
                    // that is in ] t0, t2 ]
                    //     not in ] t1, t2 ] !
                    Goal new_goal = random_goal( t0, t2 );

#ifndef NDEBUG
                    eo::log << eo::xdebug << "\tgoal: " << new_goal.earliest_start_time() << "(" << new_goal.size() << ")" << std::endl;
                    eo::log << eo::xdebug << "\tpush back" << std::endl;
#endif

                    // insert the new goal at the end
                    decompo.push_back( new_goal );

                }

                // ARTICLE VERSION:
                /***************************
                 * si j+1 est l'état final *
                 ***************************/
                /*
                if( j+1 >= decompo.size() ) {

                    // construction aléatoire d'un nouveau goal
                    // entre la date au plus tot du dernier goal et la date la plus tardive possible
                    t2 = _times.rbegin()->first;

                    Goal new_goal = random_goal( t1, t2 );
                    decompo.push_back( new_goal );

                */
                /**********************************
                 * si j+1 est n'importe quel goal *
                 **********************************/
                /*
                } else {
                    // randomly construct a new goal between earliest dates of goals j and j+1
                    
                    // article version:
                    t2 = decompo.iter_at( j + 1 )->earliest_start_time(); 

                    Goal new_goal = random_goal( std::min(t1,t2), std::max(t1,t2) );
                    decompo.insert( decompo.iter_at( j+1 ), new_goal );
                }
                */

            } // if last_reached == -1
        //} // if decompo.size == 0
    //} // if decompo.size == _l_max


#ifndef NDEBUG
    eo::log << eo::xdebug << "\tafter:  ";
    simplePrint( eo::log << eo::xdebug, decompo );
    eo::log << eo::xdebug << std::endl;
    
    /* Already checked at the beginning of an eval
    eo::log << eo::xdebug << "Check goal consistency in addgoal...";
    eo::log.flush();
    for( daex::Decomposition::iterator igoal = decompo.begin(), goal_end = decompo.end(); igoal != goal_end; ++igoal ) {
        assert_noduplicate( igoal->begin(), igoal->end() );
        assert_nomutex(     igoal->begin(), igoal->end() );
    }
    eo::log << " ok" << std::endl;
    */
#endif

    // un goal a été inséré
    return true;
};

protected:

    //! Construct a random goal between the given dates
    // Atoms used are those with their earliest start time a the chosen date
    Goal random_goal(  TimeVal t1, TimeVal t2 ){
#ifndef NDEBUG
    eo::log << eo::xdebug << "\t\trandom goal: in ]" << t1 << "," << t2 << "]";
#endif

    // t1 et t2 existent dans la chrono partition 
    assert( t1 <= t2 );
    assert( t1 != t2 );

    // les clefs t1 et t2 sont présentes
    assert( _times.count(t1) == 1 );
    assert( _times.count(t2) == 1 );
    assert( _times.find(t1) != _times.end() );
    assert( _times.find(t2) != _times.end() );

    // et sont liées à au moins un atome
    assert( _times.find(t1)->second.size() > 0 );
    assert( _times.find(t2)->second.size() > 0 );


    /******************************************************
     * Tirer au hasard une date faisables dans ] t1, t2 ] *
     ******************************************************/

    std::vector<TimeVal> candidate_dates;
    candidate_dates.reserve( _times.size() );

    // l'itérateur démarre après la première date
    // on tire les t dans ] t1, t2 ]
    ChronoPartition::const_iterator it1 = _times.find(t1);
    ChronoPartition::const_iterator it2 = _times.find(t2);
    
    // intervalle ouvert 
    do {it1++;
      //      assert(it1->first!=0);
        candidate_dates.push_back(it1->first);}
    while(it1 != it2);
 
    assert( candidate_dates.size() != 0 );

    // date au hasard parmi celles disponibles dans l'intervalle
    TimeVal date = candidate_dates[rng.random(candidate_dates.size())];

#ifndef NDEBUG
    eo::log << eo::xdebug << ", considering " << candidate_dates.size() << " candidate dates" 
                          << ", chosen one: " << date;
#endif

    assert( _times.count( date ) > 0 );

    /*****************************************************************************************
     * Récupérer les atomes correspondant au voisinage (dans la Chrono Partition) de la date *
     *****************************************************************************************/

    std::list<Atom*> candidate_atoms;

    ChronoPartition::const_iterator it = _times.find(date);
    it--;
    unsigned int j=_radius;
                                                                 // _times.begin() correspond à la date zero que l'on exclue
    while ( j != 0 && it != _times.begin()){ // sans l'élément central
      std::copy(it->second.begin(), it->second.end(), std::back_inserter(candidate_atoms));
      it--; j--;
    }

    it = _times.find(date); j=_radius+1; // element central + radius
    while (j != 0 && it != _times.end()){
      std::copy(it->second.begin(), it->second.end(), std::back_inserter(candidate_atoms));
      it++; j--;
    }

#ifndef NDEBUG
    eo::log << eo::xdebug << ", with a radius: " << _radius << ", have " << candidate_atoms.size() << " candidate atoms";
#endif

    /*    std::cout << "candidate atoms = {";
    for(std::list<Atom*>::iterator ia=candidate_atoms.begin(), end=candidate_atoms.end(); ia != end; ++ia)
      {std::cout << " " << **ia;}
      std::cout << std::endl;*/

    assert( candidate_atoms.size() != 0 );

    /********************************************************
     * Ne garder que les atomes non mutuellements exclusifs *
     ********************************************************/
    /************************************************************
     * Tirer des atomes non-mutex au hasard pour former un goal *
     ************************************************************/

#ifndef NDEBUG
    // ici, nomutex_atoms contient un ensemble d'atomes non-mutex deux à deux
    std::list<Atom*> nomutex_atoms = nomutex( candidate_atoms );
    assert( nomutex_atoms.size() > 0 );

    eo::log << eo::xdebug << ", of which " << nomutex_atoms.size() << " are nomutex";

    Goal result= random_subset( nomutex_atoms );

    eo::log << eo::xdebug << ", final goal: " << result.earliest_start_time() << "(" << result.size() << ")" << std::endl;

    return result;
#else
    // note that nomutex returns the largest random subset of nomutex atom it can found
    // thus, the call to random_subset guaranty that we get one of the several random subset 
    // of atoms that are nomutex (even a set of 1 atom)
    return random_subset( nomutex( candidate_atoms ) );
#endif

};

    const ChronoPartition & _times;

    //! Number of neighbour earliest start date to consider when adding atoms
    unsigned int _radius;

    //! Maximal size of a decomposition
    /** If the decomposition is already at the max size, the mutation don't add any new goal
     */
    //unsigned int _l_max;
};


} // namespace daex

#endif // __DAEX_MUTATION_ADDGOAL_H__
