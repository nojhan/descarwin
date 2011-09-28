

#ifndef __DAEX_EVAL_PORTFOLIO_H__
#define __DAEX_EVAL_PORTFOLIO_H__

#include <cstdlib>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>

#include "yahsp.h"


//! Évaluateur principal, à utiliser pour les itérations normales
class daePortfolioEval : public daeYahspEval
{
public:
     daePortfolioEval( unsigned int l_max_ = 20, unsigned int b_max_in = 10, unsigned int b_max_last = 30, double fitness_weight = 10, double fitness_penalty = 1e6 ) 
         : daeYahspEval(l_max_, b_max_in, b_max_last, fitness_weight, fitness_penalty)
     {}

    virtual unsigned int solve_next( daex::Decomposition & decompo, Fluent** next_state, unsigned int next_state_nb, long max_evaluated_nodes )
    {
        return daeYahspEval::solve_next( decompo, next_state, next_state_nb, max_evaluated_nodes );
    }
};

#endif // __DAEX_EVAL_PORTFOLIO_H__
