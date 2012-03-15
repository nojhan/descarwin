
#include <eo>
#include <iostream>
#include <list>
#include "core/decomposition.h"

namespace daex {

//! Modify decompo1 such that it is a cross-over of the two individuals, the choice of wether the first part comes from a decomposition or the other is made by looking at the earliest start time of the goal chosen as a cut limit.
// Note : this version does not consider if goals are ordered on earliest start times or not

template <class EOT> 

class CrossOver : public eoQuadOp<EOT>
{
public:
    // TODO create a .cpp file
    bool operator()( EOT & decompo1, EOT & decompo2 )
    {
        if( decompo1.empty() || decompo2.empty() ) {
            return false;
        }

        // choix au hasard de l'emplacement du découpage dans chaque décomposition
        std::list<daex::Goal>::iterator decompo1_middle = decompo1.begin();
        std::advance( decompo1_middle, rng.random( decompo1.size() ) );
        std::list<daex::Goal>::iterator decompo2_middle = decompo2.begin();
        std::advance( decompo2_middle, rng.random( decompo2.size() ) );

        TimeVal d1 = decompo1_middle->earliest_start_time();
        TimeVal d2 = decompo2_middle->earliest_start_time();

        // le goal avec la date la plus précoce au niveau du découpage passe en premier
        // TODO : moyenne des dates des goals précédents ? date au plus tot la plus tardive parmi les goals précédents ?
        // d'abord les goal de 1, puis les goals de 2
        if( d2 > d1 ) {
            decompo1.erase( decompo1_middle, decompo1.end() );
            std::copy( decompo2_middle, decompo2.end(), std::back_inserter( decompo1 ) );

        } else {
            decompo1.erase( decompo1.begin(), decompo1_middle );
            std::reverse_copy( decompo2.begin(), decompo2_middle, std::front_inserter( decompo1 ) ); 
        }


        // TODO soit, comme ici, un reset des sous-plan (donc une éventuelle mutation ne considérera pas comme acquis le sous-plan précédemment évalué), soit autre chose ? (évaluation intermédiaire, min( d1, last_reached ), etc. ). La solution la plus évident parait de séparer le crossover de la mutation : soit l'un soit l'autre, mais pas les deux.
        decompo1.plans_sub_reset();

        return true;   
    }
};



//! Modify decompo1 such that it is a cross-over of the two individuals, the choice of wether the first part comes from a decomposition or the other is made by looking at the earliest start time of the goal chosen as a cut limit.
// Note : this version filters out goals with earliest start times that are earlier than the goal of the cut limit (which may occurs with the addGoal mutation)
// TODO warning: only the right half of the decomposition is filtered
template <class EOT> 
class CrossOverTimeFilterHalf : public eoQuadOp<EOT>
{
public:
    bool operator()( EOT & decompo1, EOT & decompo2 )
    {
        if( decompo1.empty() || decompo2.empty() ) {
            return false;
        }

#ifndef NDEBUG
        eo::log << eo::debug << "C";
        eo::log.flush();

        eo::log << eo::xdebug << " Crossover:" << std::endl << "\tBefore: " << std::endl;
        eo::log << eo::xdebug << "\t\t";
        simplePrint( eo::log << eo::xdebug, decompo1 );
        eo::log << eo::xdebug << "\t\t";
        simplePrint( eo::log << eo::xdebug, decompo2 );
#endif

        // choix au hasard de l'emplacement du découpage dans chaque décomposition
        std::list<daex::Goal>::iterator decompo1_middle = decompo1.begin();
#ifndef NDEBUG
        unsigned int i1 = rng.random( decompo1.size() );
        std::advance( decompo1_middle, i1 );
#else
        std::advance( decompo1_middle, rng.random( decompo1.size() ) );
#endif

        std::list<daex::Goal>::iterator decompo2_middle = decompo2.begin();
#ifndef NDEBUG
        unsigned int i2 = rng.random( decompo2.size() );
        std::advance( decompo2_middle, i2 );
#else
        std::advance( decompo2_middle, rng.random( decompo2.size() ) );
#endif

        TimeVal d1 = decompo1_middle->earliest_start_time();
        TimeVal d2 = decompo2_middle->earliest_start_time();

#ifndef NDEBUG
        eo::log << eo::xdebug << "\ti1: " << i1 << ", i2: " << i2 << std::endl;
        eo::log << eo::xdebug << "\td1: " << d1 << ", d2: " << d2 << std::endl;
#endif

        // le goal avec la date la plus précoce au niveau du découpage passe en premier
        // d'abord les goal de 1, puis les goals de 2
        if( d2 > d1 ) {
            // consider the next element, to avoid erasing the middle goal
            std::list<daex::Goal>::iterator decompo1_middle_next = decompo1_middle;
            decompo1_middle_next++;

            decompo1.erase( decompo1_middle_next, decompo1.end() );

            for( std::list<daex::Goal>::iterator igoal = decompo2_middle, iend = decompo2.end(); igoal != iend; ++igoal ) {
                if( igoal->earliest_start_time() > d1 ) {
                    decompo1.push_back( *igoal );
                }
            }

        } else {
            // do not erase the middle element
            decompo1.erase( decompo1.begin(), decompo1_middle );

            // back loop and front insert
//            unsigned int c = 0;
            std::list<daex::Goal>::iterator igoal;
            std::list<daex::Goal>::iterator ibegin = decompo2.begin();
            for( igoal = decompo2_middle; igoal != ibegin; --igoal ) {
//                std::clog << " " << c++;
                if( igoal->earliest_start_time() < d2 ) {
//                    std::clog << "*";
                    decompo1.push_front( *igoal );
                }
            }
            if( igoal->earliest_start_time() < d2 ) {
//                    std::clog << "*";
                    decompo1.push_front( *igoal );
            }



        } // if d2 > d1

#ifndef NDEBUG
        eo::log << eo::xdebug << "\t After: " << std::endl;
        eo::log << eo::xdebug << "\t\t";
        simplePrint( eo::log << eo::xdebug, decompo1 );
#endif



        // TODO soit, comme ici, un reset des sous-plan (donc une éventuelle mutation ne considérera pas comme acquis le sous-plan précédemment évalué), soit autre chose ? (évaluation intermédiaire, min( d1, last_reached ), etc. ). La solution la plus évident parait de séparer le crossover de la mutation : soit l'un soit l'autre, mais pas les deux.
        decompo1.plans_sub_reset();
        return true;   
    }
};

} // namespace daex
