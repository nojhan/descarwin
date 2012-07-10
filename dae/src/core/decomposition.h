#ifndef __DECOMPOSITION_H__
#define __DECOMPOSITION_H__

#include <iterator>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <functional>

#include <eo>
#include <utils/eoTimer.h>

#include "goal.h"
#include "plan.h"
#include <src/globs.h>
#include <src/yahsp.h>

#ifdef WITH_MPI
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/split_member.hpp>
#endif // WITH_MPI

namespace daex
{

//! A decomposition is a list of Goal objects, and we are trying to minimize a scalar fitness (e.g. time or number of actions)
class Decomposition : public std::list<Goal>, public EO< eoMinimizingFitness >, public eoserial::Persistent
{
public:

    //! At instanciation, a decomposition does not have any plan
    /*!
     * Note: and is not feasible, @see eoDualFitness
     */
     Decomposition() :_plan_global(), _plans_sub(), _b_max(0), _k(0), _u(0), _B(0){}  

     virtual ~Decomposition(){}
    
#ifdef WITH_MPI
    // Gives access to boost serialization
	friend class boost::serialization::access;

    /**
     * Serializes the decomposition in a boost archive (useful for boost::mpi)
     */
    template <class Archive>
	void save( Archive & ar, const unsigned int version ) const
	{
        eoTimerStat & t = eo::mpi::timerStat;
        t.start("decomposition_save");
        std::stringstream ss;
        printOn( ss );
        std::string asStr = ss.str();
        ar & asStr;
        t.stop("decomposition_save");

        (void) version; // avoid compilation warning
	}

    /**
     * Deserializes the decomposition from a boost archive (useful for boost:mpi)
     */
    template <class Archive>
    void load( Archive & ar, const unsigned int version )
    {
        eoTimerStat & t = eo::mpi::timerStat;
        t.start("decomposition_load");
        std::string asStr;
        ar & asStr;
        std::stringstream ss;
        ss << asStr;
        readFrom( ss );
        t.stop("decomposition_load");

        (void) version; // avoid compilation warning
    }
    
    // Indicates that boost save and load operations are not the same.
    BOOST_SERIALIZATION_SPLIT_MEMBER()

#endif // WITH_MPI

     //! After a modification of the decomposition, it needs to be re-evaluated
    //! Variation operator should use this method to indicate it
    virtual void invalidate();
    
    daex::Plan plan_copy() const
    {
        return _plan_global;
    }

    daex::Plan & plan()
    {
        return _plan_global;
    }

    daex::Plan & subplan(unsigned int i) 
    {
        return _plans_sub[i];
    }

    std::vector<daex::Plan>  subplans()  const
    {
        return _plans_sub;
    }

    daex::Plan & last_subplan()
    {
        return *(_plans_sub.end() - 1 );
    }

    unsigned int subplans_size() const
    {
        return _plans_sub.size();
    }

    //! If no goal has been reached, return -1, else return the index of the goal
    int last_reached() const
    {
        return static_cast<int>( subplans_size() ) - 1;
    }

    //! If one has reached no goal or the first one, return 0, else return the index of the goal, if the end state has been reached, return the index of the last goal in the decomposition
    /*    unsigned int last_reached_safe() const
    {
        int last_reached = static_cast<int>( subplans_size() ) - 1;

        if( last_reached < 0 ) {
            last_reached = 0;

        } else if( last_reached >= this->size() ) {
            last_reached = this->size() - 1;
        }

        return static_cast<unsigned int>( last_reached );
    }
    */

    unsigned int last_reached_safe() const
    {
      if ( subplans_size() == 0 ) return 0;
      if ( subplans_size() > this->size()) return this->size() - 1;
      return subplans_size() - 1 ;
    }

    /*
    Decomposition::iterator iter_last_reached()
    {
        return iter_at( subplans_size() - 1 );
    }
    */

    void plan_global( daex::Plan p );

    void plans_sub_add( daex::Plan p );

    void plans_sub_reset();

    virtual void printOn( std::ostream & out ) const;
    
    virtual void readFrom(std::istream & _is) ;


    Decomposition::iterator iter_at( unsigned int i );

    // VV : getters/setters for 4 fields taken from daeCptYahspEval
    void b_max( unsigned int b ) { _b_max = b; }
    unsigned int b_max() const { return _b_max; } 

    unsigned int get_number_evaluated_goals()const { return _k; } 
    void reset_number_evaluated_goals() { _k = 0; }
    void incr_number_evaluated_goals(unsigned int k) { _k += k; }

    unsigned int get_number_useful_goals()const { return _u; } 
    void reset_number_useful_goals() { _u = 0; }
    void incr_number_useful_goals(unsigned int u) { _u += u; }

    unsigned int get_number_evaluated_nodes() const { return _B; }
    void reset_number_evaluated_nodes() { _B = 0; }
    void incr_number_evaluated_nodes(unsigned int B) { _B += B; }
    void setFeasible(bool  b){	_is_feasible = b; }
    bool is_feasible() const { return _is_feasible; }

    /** Reimplement comparisons, as feasibility changes the meaning of it.
     * In EO, "this < other" is read "other is better than this".
     */
    bool operator<(const Decomposition& other) const 
    {
        if( this->is_feasible() && !other.is_feasible() ) { // only this is feasible
            return false;

        } else if( !this->is_feasible() && other.is_feasible() ) { // only other is feasible
            return true;

        } else {
        /* Similar to:
        if( ( this->is_feasible() && other.is_feasible() ) // both feasible
            ||
            ( !this->is_feasible() && !other.is_feasible() ) // both unfeasible
          ) {
        */
            return this->fitness() < other.fitness(); // fallback to fitness
        }
    }

    //! Counterpart, <= and >= are derived from < and > and thus do not need to be modified
    bool operator>(const Decomposition& other) const 
    { 
        return !(this->fitness() <= other.fitness()); 
    }


private:

    bool _is_feasible;


protected:

    //! daex::Plan global compressé
    daex::Plan _plan_global;

    //! Vecteur des sous-plans
    std::vector< daex::Plan > _plans_sub;

    // VV : 4 fields made local to decomposition instead of daeCptYahspEval

    //! Current number of backtracks/nodes allowed
    unsigned int _b_max;

    //! compteur de goals
    unsigned int _k;

    //! compteur de goals utiles
    unsigned int _u;

    //! compteur des tentatives de recherche
    unsigned int _B;

public:
    eoserial::Object* pack(void) const
    {
        eoserial::Object* json = new eoserial::Object;

        // list<Goal>
        eoserial::Array* listGoal = eoserial::makeArray
            < std::list<Goal>, eoserial::SerializablePushAlgorithm >
            ( *this );
        json->add( "goals", listGoal );

        // eoFitness
        bool invalidFitness = EO< eoMinimizingFitness >::invalid();
        json->add( "invalidFitness", eoserial::make(invalidFitness) );

        if ( !invalidFitness )
        {
            eoMinimizingFitness fitness = EO< eoMinimizingFitness >::fitness();
            double fitnessValue = fitness; // implicit operator cast
            json->add( "fitnessValue", eoserial::make(fitnessValue) );
        }

        // specific members
        json->add( "plan_global", &_plan_global );
        // subplans
        eoserial::Array* subplans = eoserial::makeArray
            < std::vector< daex::Plan >, eoserial::SerializablePushAlgorithm >
            ( _plans_sub );

        json->add( "subplans", subplans );
        json->add( "b_max", eoserial::make(_b_max) );
        json->add( "goal_count", eoserial::make(_k) );
        json->add( "useful_goals", eoserial::make(_u) );
        json->add( "attempts", eoserial::make(_B) );
        json->add( "is_feasible", eoserial::make(_is_feasible) );

        return json;
    }

    void unpack( const eoserial::Object* json )
    {
        // list<Goal>
        clear();
        eoserial::unpackArray
            < std::list< Goal >, eoserial::Array::UnpackObjectAlgorithm >
            ( *json, "goals", *this );

        // EO fitness
        bool invalidFitness;
        eoserial::unpack( *json, "invalidFitness", invalidFitness );
        if (invalidFitness)
        {
            EO< eoMinimizingFitness >::invalidate();
        } else
        {
            eoMinimizingFitness fitness;
            double fitnessValue;
            eoserial::unpack( *json, "fitnessValue", fitnessValue );
            fitness = fitnessValue;
            EO< eoMinimizingFitness >::fitness( fitness );
        }
        
        // specific members
        eoserial::unpackObject( *json, "plan_global", _plan_global );
        // _plans_sub
        _plans_sub.clear();
        eoserial::unpackArray
            < std::vector< daex::Plan >, eoserial::Array::UnpackObjectAlgorithm >
            ( *json, "subplans", _plans_sub );

        eoserial::unpack( *json, "b_max", _b_max );
        eoserial::unpack( *json, "goal_count", _k );
        eoserial::unpack( *json, "useful_goals", _u );
        eoserial::unpack( *json, "attempts", _B );
        eoserial::unpack( *json, "is_feasible", _is_feasible );
    }
}; // class Decomposition

//! Print a decomposition in a simple format
/** On a single line, print the date and the number of atoms of each goal, i.e.:
 *    Decomposition (size): goal(#atoms) ...
 *  e.g.:
 *    Decompositon (3): 2(5) 12(45) 123(4)
 */
void simplePrint( std::ostream & out, Decomposition & decompo );


} // namespace daex

#endif

