
#ifndef _MAKE_EVAL_DAE_H_
#define _MAKE_EVAL_DAE_H_

#include "evaluation/cpt-yahsp.h"

#ifdef DAE_MO
#include "evaluation/yahsp_mo.h"
#else
#include "evaluation/yahsp.h"
#endif

namespace daex {

void do_make_eval_param( eoParser & parser )
{
    // FIXME use double instead of int?
    unsigned int fitness_weight = parser.createParam( (unsigned int)10, "fitness-weight", 
            "Unknown weight in the feasible and unfeasible fitness computation", 'W', "Evaluation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "fitness_weight" << fitness_weight << std::endl;

    unsigned int fitness_penalty = parser.createParam( (unsigned int)2, "fitness-penalty", 
            "Penalty in the unfeasible fitnesses computation", 'w', "Evaluation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "fitness_penalty" << fitness_penalty << std::endl;

#ifndef SINGLE_EVAL_ITER_DUMP
    unsigned int max_seconds = parser.createParam( (unsigned int)0, "max-seconds", 
            "Maximum number of user seconds in CPU for the whole search, set it to 0 to deactivate (1800 = 30 minutes)", 'i', "Stopping criterions" ).value(); // 1800 seconds = 30 minutes
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "max_seconds" << max_seconds << std::endl;
#endif
    
    unsigned int b_max_init = parser.createParam( (unsigned int)1e4, "bmax-init", "Number of allowed expanded nodes for the initial computation of b_max", 'B', "Evaluation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "b_max_init" << b_max_init << std::endl;

    /* nonsense with the incremental strategy
    double b_max_quantile = parser.createParam( (double)0.5, "bmax-quantile", 
            "Quantile to use for estimating b_max (in [0,1], 0.5=median)", 'Q', "Evaluation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "b_max_quantile" << b_max_quantile << std::endl;
    if( b_max_quantile < 0 || b_max_quantile > 1 ) {
        std::cout << "bmax-quantile must be a double in [0,1] (=" << b_max_quantile << ") type --help for usage." << std::endl;
        exit(1);
    }looser throw specifier for
    */

    unsigned int b_max_fixed = parser.createParam( (unsigned int)0, "bmax-fixed", "Fixed number of allowed expanded nodes. Overrides bmaxinit if != 0", 'b', "Evaluation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "b_max_fixed" << b_max_fixed << std::endl;

    double b_max_last_weight = parser.createParam( (double)3, "bmax-last-weight",
            "Weighting for the b_max used during the last search towards the end goal (must be strictly positive)", 'T', "Evaluation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "b_max_last_weight" << b_max_last_weight << std::endl;

    if( b_max_last_weight <= 0 ) {
        std::cout << "bmax-last-weight must be strictly positive (=" << b_max_last_weight << ") type --help for usage." << std::endl;
        exit(1);
    }

#ifdef DAE_MO
    double astar_weight = parser.createParam( (double)1, "astar-weight", // FIXME default value??
            "A* weight within YAHSP", 'H', "Evaluation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "astar-weight" << astar_weight << std::endl;

    bool cost_max = parser.createParam( (bool)false, "cost-max",
            "Use max cost instead of additive costs as the second objective", 'M', "Multi-Objective" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "cost-max" << cost_max << std::endl;
#endif
}


template<class EOT>
std::pair<  eoEvalFunc<EOT>&, eoEvalFuncCounter<EOT>*  >
    do_make_eval_op(
            eoParser & parser, eoState & state,
            unsigned int l_max,
            unsigned int eval_count, unsigned int estimated_b_max, unsigned int b_max_last,
            std::string plan_file, TimeVal best_makespan,
            std::string dump_sep, unsigned int dump_file_count, std::string metadata
    )
{
    unsigned int fitness_weight = parser.valueOf<unsigned int>("fitness-weight");
    unsigned int fitness_penalty = parser.valueOf<unsigned int>("fitness-penalty");
    unsigned int max_seconds = parser.valueOf<unsigned int>("max-seconds");

    eoEvalFunc<EOT>* eval_yahsp;
#ifdef DAE_MO
    double astar_weight = parser.valueOf<double>("astar-weight");
    bool cost_max = parser.valueOf<bool>("cost-max");
    if( cost_max ) {
        eval_yahsp = new daemoYahspEvalMax<EOT>( astar_weight, l_max, estimated_b_max, b_max_last, fitness_weight, fitness_penalty );
    } else {
        eval_yahsp = new daemoYahspEvalAdd<EOT>( astar_weight, l_max, estimated_b_max, b_max_last, fitness_weight, fitness_penalty );
    }
#else
    eval_yahsp = new daeYahspEval<EOT>( l_max, estimated_b_max, b_max_last, fitness_weight, fitness_penalty );
#endif

    state.storeFunctor( eval_yahsp );

    // nested evals:
    eoEvalFunc<EOT> * p_eval;

    //#ifndef SINGLE_EVAL_ITER_DUMP
    // dump the best solution found so far in a file
    daex::evalBestMakespanPlanDump<EOT>* eval_bestfile = new daex::evalBestMakespanPlanDump<EOT>( *eval_yahsp, plan_file, best_makespan, false, dump_sep, dump_file_count, metadata );
    state.storeFunctor( eval_bestfile );

#ifndef NDEBUG
    // counter, for checkpointing
    eoEvalFuncCounter<EOT>* eval_counter = new eoEvalFuncCounter<EOT>( *eval_bestfile, "Eval.\t" );
    eval_counter->value( eval_count );
    state.storeFunctor( eval_counter );
# else // NDEBUG
    eval_count++; // used
#endif // NDEBUG
    //#endif

    // if we do not want to add a time limit, do not add an EvalTime
    //#ifdef SINGLE_EVAL_ITER_DUMP
    //        p_eval = & eval_yahsp;
    //#else // ifndef SINGLE_EVAL_ITER_DUMP
# ifdef WITH_MPI
    // When using MPI, we don't want the workers to brutally terminate by launching an exception, as it causes deadlock.
    // Master could wait forever for a response which won't ever come. When using MPI parallelization, the termination
    // test is made in the Job loop (see make_parallel_eval_dae.h).
    if( eo::parallel.isEnabled() )
    {
#ifndef NDEBUG
        p_eval = eval_counter;
#else // NDEBUG
        p_eval = eval_bestfile;
#endif // NDEBUG
    } else // ( eo::parallel.isEnabled() )
    {
# endif // WITH_MPI
        if( max_seconds == 0 ) {
#ifndef NDEBUG
            p_eval = eval_counter;
#else // NDEBUG
            p_eval = eval_bestfile;
#endif // NDEBUG
        } else { // max_seconds == 0
            // an eval that raises an exception if maxtime is reached
            /* eoEvalTimeThrowException<EOT> * p_eval_maxtime 
               = new eoEvalTimeThrowException<EOT>( eval_counter, max_seconds ); */
            eoEvalUserTimeThrowException<EOT> * p_eval_maxtime
#ifndef NDEBUG
                = new eoEvalUserTimeThrowException<EOT>( *eval_counter,  max_seconds );
#else
            = new eoEvalUserTimeThrowException<EOT>( *eval_bestfile, max_seconds );
#endif // NDEBUG

            state.storeFunctor( p_eval_maxtime );
            p_eval = p_eval_maxtime;
        } // max_seconds == 0
# ifdef WITH_MPI
    } // eo::parallel.isEnabled()
# endif // WITH_MPI
    
#ifndef NDEBUG
    return std::make_pair<eoEvalFunc<EOT>&, eoEvalFuncCounter<EOT>*>( *p_eval, eval_counter );
#else
    return std::make_pair<eoEvalFunc<EOT>&, eoEvalFuncCounter<EOT>*>( *p_eval, NULL );
#endif
}

} // namespace daex

#endif // _MAKE_EVAL_DAE_H_

