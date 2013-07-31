
#ifndef __DAEX_EVAL_YAHSP_MO_H__
#define __DAEX_EVAL_YAHSP_MO_H__

#include "../core/decomposition_mo.h"
#include "../core/strategies.h"
#include "../evaluation/yahsp.h"

template<class EOT>
class daemoYahspEval : public daeYahspEval<EOT>
{public:
    typedef typename EOT::Fitness Fitness;
    typedef typename EOT::ObjectiveVector::Base OVBase;

    virtual OVBase objective_makespan(EOT& decompo)
    {
      Fitness f = this->solve(decompo);
      return OVBase( f.value(), f.is_feasible() ); // Front calculé avec la fitness : penser à extraire la solution en fonction des objectifs.
      //return OVBase(convertMakespan(decompo.plan().makespan()), f.is_feasible()); // Front calculé avec l'objectif.
    }

    //! Handler to be implemented, with either additive or max costs
    virtual double objective_cost( EOT& decompo ) = 0;

    daemoYahspEval(unsigned int astar_weight, // FIXME default value?
		   unsigned int l_max_ = 20,
		   unsigned int b_max_in = 10,
		   unsigned int b_max_last = 30,
		   double fitness_weight = 10,
		   double fitness_penalty = 1e6
    ) : daeYahspEval<EOT>( l_max_, b_max_in, b_max_last, fitness_weight, fitness_penalty ) {yahsp_set_weight( astar_weight );}

    void call( EOT& decompo )
    {
        typename EOT::ObjectiveVector objVector;
        // get the fitness that correspond to the "makespan" objective
        // for unfeasible decomposition, that may be a penalized fitness
        OVBase result = objective_makespan( decompo );

        // assign value + feasibility to the first objective (makespan)
        // result contains a pair<value,feasibility>
        objVector[0] = result; // Sans faire de reserve ??

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
        objVector[1] = OVBase( cost, result.is_feasible() );

        // assign value + feasibility to the whole objective vector
        // necessary because objective vectors implement Pareto domination (@see DualRealObjectiveVector::dominates)
        objVector.is_feasible( result.is_feasible() );
                                                eo::log << eo::xdebug << "OBJECTIVE VECTOR =" << objVector << std::endl;
        // change the MO objectives
        decompo.objectiveVector(objVector);

        // Update the feasibility but not the fitness.
        // We use a dummy fitness (which is a different concept that the objectives, in MOEO)
        // because the feasibility is stored in the fitness, but the fitness is computed in IBEA
        // by the fitness assignement operator (@see moeoExpBinaryIndicatorBasedFitnessAssignment).
        // Here, we update the feasability, which will not change when the fitness will be updated
        // (@see Decomposition::fitness(double) ).
        decompo.fitness( -1.0, result.is_feasible() );
    }

    //! Set YAHSP strategy according to the current decomposition strategy
    void pre_call(EOT& decompo)
    {//eo::log << eo::debug << "Strategy=" << decompo.strategy();
      if (decompo.strategy() == "random") {eo::log << eo::debug << " Init Strategy: random" << std::endl;
	  std::vector<daex::Strategies::Type> default_strategies;
	  default_strategies.push_back(daex::Strategies::length);
	  default_strategies.push_back(daex::Strategies::cost);
	  default_strategies.push_back(daex::Strategies::makespan_max);
	  default_strategies.push_back(daex::Strategies::makespan_add);
	  std::vector<double> rates;
	  double rate = 1.0/default_strategies.size();
	  for (unsigned int i=0; i < default_strategies.size(); ++i) {rates.push_back(rate);}
	  switch (default_strategies[rng.roulette_wheel(rates)]) {
	  case daex::Strategies::length: {eo::log << eo::debug << " = length (random)";
	      yahsp_set_optimize_length(); break; } // search for short plans
	  case daex::Strategies::cost: {eo::log << eo::debug << " = cost (random)";
	      // search for plans with lower (additive) costs
	      // NOTE: YAHSP does only optimize additive cost, but may compute max cost after compression.
	      yahsp_set_optimize_cost(); break; }
	  case daex::Strategies::makespan_add: {eo::log << eo::debug << " = makespan_add (random)";
	      yahsp_set_optimize_makespan_add(); break; }
	  case daex::Strategies::makespan_max: {eo::log << eo::debug << " = makespan_max (random)";
	      yahsp_set_optimize_makespan_max(); break; }
	  default: {eo::log << eo::debug << " = default (random)";
	      /* use default yahsp settings */ break;}
	  }
	}
      else if (decompo.strategy() == "flip-decomposition") {
	if (rng.flip(decompo.proba_strategy())) {yahsp_set_optimize_makespan_add(); eo::log << eo::debug << " = makespan-add (after flip)" << std::endl;}
	else  {yahsp_set_optimize_cost(); eo::log << eo::debug << " = cost (after flip)" << std::endl;}}
      else if (decompo.strategy() == "length") {eo::log << eo::debug << " = length" << std::endl;
	yahsp_set_optimize_length();}
      else if (decompo.strategy() == "cost") {eo::log << eo::debug << " = cost" << std::endl;
	yahsp_set_optimize_cost();}
      else if (decompo.strategy() == "makespan-max") {eo::log << eo::debug << " = makespan-max" << std::endl;
	yahsp_set_optimize_makespan_max();}
      else if (decompo.strategy() == "makespan-add") {eo::log << eo::debug << " = makespan-add" << std::endl;
	yahsp_set_optimize_makespan_add();}
      else {throw std::runtime_error("Unknown MO search strategy");}
    }
};

template<class EOT=daex::DecompositionMO>
class daemoYahspEvalAdd : public daemoYahspEval<EOT>
{
public:// typedef typename EOT::Fitness Fitness;
    daemoYahspEvalAdd(unsigned int astar_weight, // FIXME default value?
		      unsigned int l_max_ = 20,
		      unsigned int b_max_in = 10,
		      unsigned int b_max_last = 30,
		      double fitness_weight = 10,
		      double fitness_penalty = 1e6
    ) : daemoYahspEval<EOT>( astar_weight, l_max_, b_max_in, b_max_last, fitness_weight, fitness_penalty )
    {}
    virtual double objective_cost( EOT& decompo ) {
      return decompo.plan().cost_add(); // objective
      //return fitness_feasible(decompo, (double) decompo.plan().cost_add()); // fitness
    }
};

template<class EOT=daex::DecompositionMO>
class daemoYahspEvalMax : public daemoYahspEval<EOT>
{
public:// typedef typename EOT::Fitness Fitness;
    daemoYahspEvalMax(unsigned int astar_weight, // FIXME default value?
		      unsigned int l_max_ = 20,
		      unsigned int b_max_in = 10,
		      unsigned int b_max_last = 30,
		      double fitness_weight = 10,
		      double fitness_penalty = 1e6
    ) : daemoYahspEval<EOT>( astar_weight, l_max_, b_max_in, b_max_last, fitness_weight, fitness_penalty )
    {}
    virtual double objective_cost( EOT& decompo ) {
      return decompo.plan().cost_max(); // objective
      //return fitness_feasible(decompo, (double) decompo.plan().cost_max()); // fitness
    }
};
#endif // __DAEX_EVAL_YAHSP_MO_H__

