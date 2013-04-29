
#ifndef __DECOMPOSITION_MO_H__
#define __DECOMPOSITION_MO_H__

#include <moeo>

#include "goal_mo.h"
#include "decomposition.h"

namespace daex {

/**
 * Definition of the objective vector traits for multi-objective  for planning problems
 */
class DecompoMOTraits : public moeoObjectiveVectorTraits
{
public:
    /**
     * Returns true if the _ith objective have to be minimzed
     * @param _i index of the objective
     */
    static bool minimizing( int ) { return true; }

    /**
     * Returns true if the _ith objective have to be maximzed
     * @param _i index of the objective
     */
    static bool maximizing( int ) { return false; }

    /**
     * Returns the number of objectives
     */
    static unsigned int nObjectives() { return 2; }
};


/**
 * Definition of the objective vector for multi-objective planning problems: a vector of doubles
 */
typedef moeoRealObjectiveVector<DecompoMOTraits> DecompoMOObjectives;


class DecompositionMO : public DecompositionBase<GoalMO>, public MOEO< DecompoMOObjectives, /*MOEOFitness*/double, /*MOEODiversity*/double >
{
public:
    typedef DecompoMOObjectives ObjectiveVector ;
    typedef double Fitness;
    typedef double Diversity;
    typedef MOEO<ObjectiveVector,Fitness,Diversity> MOEOType;
public:

    virtual void invalidate()
    {
        this->MOEOType::invalidate();
        this->DecompositionBase<GoalMO>::invalidate_plan();
    }

    /* FIXME tester s'il vaut mieux une comparaison lexicographique ou pas pour le multi-objectif
     * dans l'algo MO de Mostepha, il n'y a pas de comparaison lexicographique sur la faisabilité,
     * dans l'algo mono-objectif standard, elle est implémentée via operator> et operator<
     * Il faut vérifier laquelle utiliser dans les deux cas.
     * Pour ce faire, déplacer les operator> depuis Decomposition vers DecompositionBase.
     */

    eoserial::Object* pack(void) const
    {
        eoserial::Object* json = new eoserial::Object;

        // list<GoalMO>
        eoserial::Array* listGoal = eoserial::makeArray
            < std::list<GoalMO>, eoserial::SerializablePushAlgorithm >
            ( *this );
        json->add( "goals", listGoal );

        bool invalidObjVec = MOEOType::invalidObjectiveVector();
        json->add( "invalidObjVec", eoserial::make(invalidObjVec) );
        if( !invalidObjVec ) {
            // must be double, or else the direct serialization would not work
            double objective_makespan = this->objectiveVector(0);
            json->add( "objective_makespan", eoserial::make(objective_makespan) );

            double objective_cost = this->objectiveVector(1);
            json->add( "objective_cost", eoserial::make(objective_cost) );
        }

        bool invalidFitness = MOEOType::invalidFitness();
        json->add( "invalidFitness", eoserial::make(invalidFitness) );
        if( !invalidFitness ) {
            Fitness fitness = this->fitness();
            json->add( "fitness", eoserial::make(fitness) );
        }

        bool invalidDiversity = MOEOType::invalidDiversity();
        json->add( "invalidDiversity", eoserial::make(invalidDiversity) );
        if( !invalidDiversity ) {
            Diversity diversity = this->diversity();
            json->add( "diversity", eoserial::make(diversity) );
        }

        // specific members
        json->add( "plan_global", &_plan_global );
        // subplans
        eoserial::Array* subplans = eoserial::makeArray
            < std::vector< Plan >, eoserial::SerializablePushAlgorithm >
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
            < std::list< GoalMO >, eoserial::Array::UnpackObjectAlgorithm >
            ( *json, "goals", *this );

        bool invalidObjVec;
        eoserial::unpack( *json, "invalidObjVec", invalidObjVec );
        if (invalidObjVec) {
            MOEOType::invalidateObjectiveVector();
        } else {
            // in MOEO, the fitness is computed only by the Pareto archive
            // here, we only need the objectives and the diversity
            double objective_makespan;
            eoserial::unpack( *json, "objective_makespan", objective_makespan );

            double objective_cost;
            eoserial::unpack( *json, "objective_cost", objective_cost);

            this->objectiveVector(0, objective_makespan);
            this->objectiveVector(1, objective_cost);
        }

        bool invalidFitness;
        eoserial::unpack( *json, "invalidFitness", invalidFitness );
        if( invalidFitness ) {
            MOEOType::invalidateFitness();
        } else {
            Fitness fitness;
            eoserial::unpack( *json, "fitness", fitness );
            this->fitness( fitness );
        }

        bool invalidDiversity;
        eoserial::unpack( *json, "invalidDiversity", invalidDiversity );
        if( invalidDiversity ) {
            MOEOType::invalidateDiversity();
        } else {
            Diversity diversity;
            eoserial::unpack( *json, "diversity", diversity );
            this->diversity( diversity );
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

    virtual void readFrom(std::istream & is)
    {
        eoserial::readFrom( *this, is );
    }
    virtual void printOn(std::ostream & out) const
    {
        eoserial::printOn( *this, out );
    }
};

} // namespace daex

#endif

