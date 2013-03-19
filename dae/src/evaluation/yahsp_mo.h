
#ifndef __DAEX_EVAL_YAHSP_MO_H__
#define __DAEX_EVAL_YAHSP_MO_H__

#include "../core/decomposition_mo.h"
#include "../evaluation/yahsp.h"

template<class EOT=daex::DecompositionMO>
class daemoYahspEval : public daeYahspEval<EOT>
{
public:
    typedef typename daeYahspEval<EOT>::FitT FitT;

    virtual FitT objective_makespan( EOT& decompo )
    {
        return solve( decompo );
    }


    //! Handler to be implemented, with either additive or max costs
    virtual FitT objective_cost( EOT& decompo ) = 0;


    daemoYahspEval(
        unsigned int astar_weigth, // FIXME default value?
        unsigned int l_max_ = 20,
        unsigned int b_max_in = 10,
        unsigned int b_max_last = 30,
        double fitness_weight = 10,
        double fitness_penalty = 1e6
    ) :
        daeYahspEval<EOT>( l_max_, b_max_in, b_max_last, fitness_weight, fitness_penalty )
    {
        yahsp_set_weight( astar_weigth );
    }


    void call( EOT& decompo )
    {
        daex::DecompoMOObjectives objVector;
        FitT makespan = objective_makespan( decompo );
        objVector[0] = makespan.first;

        if( makespan.second ) {
            // if the decomposition is feasible, we have a max/add cost
            FitT cost = objective_cost( decompo );
            assert( makespan.second == cost.second ); // equal feasibilities
            objVector[1] = cost.first;
        } else {
            // else, we have no cost, thus we cannot discriminate,
            // thus we use the same value as for the 1st objective
            objVector[1] = makespan.first;
        }
        // change the MO fitness
        decompo.objectiveVector(objVector);
    }


    //! Set YAHSP strategy according to the current goal strategy
    void pre_step( typename EOT::AtomType& goal )
    {
        switch( goal.strategy() ) {
            case daex::Strategies::length: {
                yahsp_set_optimize_length(); break; }

            case daex::Strategies::cost: {
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
    typedef typename daemoYahspEval<EOT>::FitT FitT;

    daemoYahspEvalAdd(
        unsigned int astar_weigth, // FIXME default value?
        unsigned int l_max_ = 20,
        unsigned int b_max_in = 10,
        unsigned int b_max_last = 30,
        double fitness_weight = 10,
        double fitness_penalty = 1e6
    ) :
        daemoYahspEval<EOT>( astar_weigth, l_max_, b_max_in, b_max_last, fitness_weight, fitness_penalty )
    {}

    virtual FitT objective_cost( EOT& decompo )
    {
        return std::make_pair<double,bool>( decompo.plan().cost_add(), decompo.is_feasible() );
    }
};



template<class EOT=daex::DecompositionMO>
class daemoYahspEvalMax : public daemoYahspEval<EOT>
{
public:
    typedef typename daemoYahspEval<EOT>::FitT FitT;

    daemoYahspEvalMax(
        unsigned int astar_weigth, // FIXME default value?
        unsigned int l_max_ = 20,
        unsigned int b_max_in = 10,
        unsigned int b_max_last = 30,
        double fitness_weight = 10,
        double fitness_penalty = 1e6
    ) :
        daemoYahspEval<EOT>( astar_weigth, l_max_, b_max_in, b_max_last, fitness_weight, fitness_penalty )
    {}

    virtual FitT objective_cost( EOT& decompo )
    {
        return std::make_pair<double,bool>( decompo.plan().cost_max(), decompo.is_feasible() );
    }
};

#endif // __DAEX_EVAL_YAHSP_MO_H__

