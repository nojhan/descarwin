
#ifndef __DAEX_EVAL_YAHSP_MO_H__
#define __DAEX_EVAL_YAHSP_MO_H__

#include "../core/decomposition_mo.h"
#include "../core/strategies.h"
#include "../evaluation/yahsp.h"

template<class EOT=daex::DecompositionMO>
class daemoYahspEval : public daeYahspEval<EOT>
{
public:
    typedef typename daeYahspEval<EOT>::Fitness Fitness;

    virtual Fitness objective_makespan( EOT& decompo )
    {
        return this->solve( decompo );
    }


    //! Handler to be implemented, with either additive or max costs
    virtual double objective_cost( EOT& decompo ) = 0;


    daemoYahspEval(
        unsigned int astar_weight, // FIXME default value?
        unsigned int l_max_ = 20,
        unsigned int b_max_in = 10,
        unsigned int b_max_last = 30,
        double fitness_weight = 10,
        double fitness_penalty = 1e6
    ) :
        daeYahspEval<EOT>( l_max_, b_max_in, b_max_last, fitness_weight, fitness_penalty )
    {
        yahsp_set_weight( astar_weight );
    }


    void call( EOT& decompo )
    {
        typename EOT::ObjectiveVector objVector;

        // get the fitness that correspond to the "makespan" objective
        // for unfeasible decomposition, that may be a penalized fitness
        Fitness result = objective_makespan( decompo );

        // assign value + feasibility to the first objective (makespan)
        // result contains a pair<value,feasibility>
        objVector[0] = result;

        double cost;
        // the feasability is computed only by objective_makespan (where the plan is computed)
        // costs are gathered after this computation
        if( result.is_feasible() ) {
            // if the decomposition is feasible, we have a max/add cost
            // (depending on what the user asked)
            // NOTE: should be called before objective_makespan, because it just get the costs computed by the
            // evaluation (@see solve)
            cost = objective_cost( decompo );
        } else {
            // else, we have no cost, thus we cannot discriminate,
            // thus we use the same value as for the 1st objective
            cost = result.value();
        }
        // assign value + feasibility to the second objective (cost)
        objVector[1] = Fitness( cost, result.is_feasible() );

        // assign value + feasibility to the whole objective vector
        // necessary because objective vectors implement Pareto domination (@see DualRealObjectiveVector::dominates)
        objVector.is_feasible( result.is_feasible() );

        // change the MO objectives
        decompo.objectiveVector(objVector);

        // Update the feasibility but not the fitness.
        // We use a dummy fitness (which is a diferent concept that the objectives, in MOEO)
        // because the feasibility is stored in the fitness, but the fitness is computed in IBEA
        // by the fitness assignement operator (@see moeoExpBinaryIndicatorBasedFitnessAssignment).
        // Here, we update the feasability, which will not change when the fitness will be updated
        // (@see Decomposition::fitness(double) ).
        decompo.fitness( -1.0, result.is_feasible() );
    }


    //! Set YAHSP strategy according to the current goal strategy
    void pre_step( typename EOT::AtomType& goal )
    {
        switch( goal.strategy() ) {
            case daex::Strategies::length: {
                // search for short plans
                yahsp_set_optimize_length(); break; }

            case daex::Strategies::cost: {
                // search for plans with lower (additive) costs
                // NOTE: YAHSP does only optimize additive cost,
                // but may compute max cost after compression
                yahsp_set_optimize_cost(); break; }

            case daex::Strategies::makespan_add: {
                yahsp_set_optimize_makespan_add(); break; }

            case daex::Strategies::makespan_max: {
                yahsp_set_optimize_makespan_max(); break; }

            default: {
                /* use default yahsp settings */ break;
            }
        } // switch strategy
    }
};



template<class EOT=daex::DecompositionMO>
class daemoYahspEvalAdd : public daemoYahspEval<EOT>
{
public:
    typedef typename daemoYahspEval<EOT>::Fitness Fitness;

    daemoYahspEvalAdd(
        unsigned int astar_weight, // FIXME default value?
        unsigned int l_max_ = 20,
        unsigned int b_max_in = 10,
        unsigned int b_max_last = 30,
        double fitness_weight = 10,
        double fitness_penalty = 1e6
    ) :
        daemoYahspEval<EOT>( astar_weight, l_max_, b_max_in, b_max_last, fitness_weight, fitness_penalty )
    {}

    virtual double objective_cost( EOT& decompo )
    {
        return decompo.plan().cost_add();
    }
};



template<class EOT=daex::DecompositionMO>
class daemoYahspEvalMax : public daemoYahspEval<EOT>
{
public:
    typedef typename daemoYahspEval<EOT>::Fitness Fitness;

    daemoYahspEvalMax(
        unsigned int astar_weight, // FIXME default value?
        unsigned int l_max_ = 20,
        unsigned int b_max_in = 10,
        unsigned int b_max_last = 30,
        double fitness_weight = 10,
        double fitness_penalty = 1e6
    ) :
        daemoYahspEval<EOT>( astar_weight, l_max_, b_max_in, b_max_last, fitness_weight, fitness_penalty )
    {}

    virtual double objective_cost( EOT& decompo )
    {
        return decompo.plan().cost_max();
    }
};

#endif // __DAEX_EVAL_YAHSP_MO_H__

