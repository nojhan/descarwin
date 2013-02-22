# ifndef __MAKE_PARALLEL_MULTISTART_DAE_H__
# define __MAKE_PARALLEL_MULTISTART_DAE_H__

#include <mpi/eoMultiStart.h>

/**
 * @brief Dumper for the multi start job.
 */
struct MultiDumpBestDecompo : public eo::mpi::HandleResponseMultiStart< daex::Decomposition >, public daex::BestPlanDumpper< daex::Decomposition::Fitness >
{
    MultiDumpBestDecompo(
            std::string afilename,
            daex::Decomposition::Fitness worst,
            bool single_file = false,
            unsigned int file_count = 0,
            std::string sep = ".",
            std::string metadata = ""
            )
        : daex::BestPlanDumpper< daex::Decomposition::Fitness >( afilename, worst, single_file, file_count, sep, metadata)
    {
        // empty
    }

    void operator()( int wrkRank )
    {
        (*_wrapped)( wrkRank );
        daex::Decomposition& last = _data->bests.back();

        eo::log << eo::progress << "[General] Received a response with fit " << _data->bests.back().fitness() << std::endl;
        if( last.fitness() > _best )
        {
            _best = last.fitness();
            dump( last );
        }
    }
};

/**
 * @brief Resetter for DAE: resets the 2 continuators, steady fit and gen max, and reuses
 * the same population as the one obtained in the previous start.
 */
template< class EOT >
struct DaeResetter : public eo::mpi::ReuseSamePopEA< EOT >
{
    DaeResetter( eoCountContinue<EOT>& gen,
                    const eoPop<EOT>& originalPop,
                    eoPopEvalFunc<EOT>& pop_eval,
                    eoCountContinue<EOT>& steadyfit
                    ) :
        eo::mpi::ReuseSamePopEA< EOT >( gen, originalPop, pop_eval ),
        _steadyfit( steadyfit )
    {}

    DaeResetter(eoCountContinue<EOT>& gen,
                    const eoPop<EOT>& originalPop,
                    eoEvalFunc<EOT>& eval,
                    eoCountContinue<EOT>& steadyfit
                    ) :
        eo::mpi::ReuseSamePopEA< EOT >( gen, originalPop, eval ),
        _steadyfit( steadyfit )
    {}

    void operator()( eoPop< EOT >& pop )
    {
        eo::mpi::ReuseSamePopEA< EOT >::operator()( pop );
        _steadyfit.reset();
    }

    protected:

    eoCountContinue< EOT >& _steadyfit;
};

namespace parallel
{
    /**
     * @brief Checks if we use multi start or not.
     *
     * In a multistart, checks the size of the cluster is sufficient for the number of multistart workers
     * and evaluators, and frees the processes which have nothing to do.
     *
     * @return true if we use multi start, false otherwise.
     */
    bool check_roles_multistart( int multistart_workers, int eval_workers )
    {
        // In multistart, we have to check that the number of multi-starters and multi-workers fits the size of the cluster.
        bool with_multistart = multistart_workers > 0 && eval_workers > 0 && eo::parallel.isEnabled();

        if( with_multistart )
        {
            int rank = eo::mpi::Node::comm().rank();
            int maxRank = 1 /* general master */
                + multistart_workers /* multi starters */
                + multistart_workers * eval_workers /* eval for each multi starter */
                ;

            // If there isn't hosts enough, we leave with an exception.
            if( maxRank > eo::mpi::Node::comm().size() )
            {
                throw std::logic_error("Not enough hosts to launch the process with the given multistart-workers and eval-workers parameters!");
            }

            // If the current process is an host with the rank higher than the last forecast worker, it leaves with a message.
            if( rank >= maxRank ) // ranks begin from 0
            {
                std::cout << "Warning: process " << rank << " has nothing to do. Leaving." << std::endl;
                eo::mpi::timerStat.stop("dae_main");
                eo::mpi::Node::comm().send( eo::mpi::DEFAULT_MASTER, 0, eo::mpi::timerStat );
                exit( 0 );
            }
        }

        return with_multistart;
    }

    /*
     * In multistart mode, we need to compute how to map the different hosts to their
     * roles. The general map is the following:
     * - 0 ( == eo::mpi::DEFAULT_MASTER ) is the general master, i.e. the master of 
     *   multi-starts. It's the only process which dumps the best solutions.
     * - 1 to multistarts_workers (i.e. multistarts_workers processes) : workers of
     *   multi-starts, master of evaluation. These processes launch the algorithm
     *   with different seeds and compute the best solutions, that they send to the
     *   general master.
     * - multistarts_workers + 1 until rest of the world (i.e.
     *   multistarts_workers * eval_workers processes) : they perform the evaluations
     *   for each of the multistart workers.
     *
     * If multi-start isn't activated, all processes but general master are evaluators.
     *
     * Multistart worker i has eval_workers workers. In the following algorithm, i
     * stands for the multistart worker's rank, k for a loop index and j is the rank
     * of the last unaffected worker (which will become an eval_worker). Eval workers'
     * ranks begin from multistart_workers + 1.
     *
     * Eval master stands for the master rank of the evaluation process. If there is no
     * multistart, it's the default master. When using multi-start, there are 2 ways of
     * finding it:
     * - either the current process is a multistart worker, then it is the master of the
     *   evaluation, and there is the affectation "eval_master = rank" for this case.
     * - or the current process is an eval worker, then when we're computing the workers
     *   used for each parallel eval, we can find the rank of the master by comparing
     *   the current added worker to current process' rank (inner if).
     */
    void compute_roles(
            // in parameters
            bool with_multistart,
            int multistart_workers,
            int eval_workers,
            // in / out parameters
            int & eval_master,
            std::vector< std::vector<int> > & evaluators_affectations
            )
    {
        int rank = eo::mpi::Node::comm().rank();
        if( with_multistart )
        {
            eval_master = rank;
            int j = multistart_workers + 1;
            for( int i = 1; i <= multistart_workers; ++i )
            {
                std::vector<int> evaluators;
                for( int k = 0; k < eval_workers; ++k )
                {
                    if( rank == j )
                    {
                        eval_master = i;
                    }
                    evaluators.push_back( j++ );
                }
                evaluators_affectations.push_back( evaluators );
            }
        } else
        {
            std::vector<int> evaluators;
            for( int i = 1; i < eo::mpi::Node::comm().size(); ++i )
            {
                evaluators.push_back( i );
            }
            evaluators_affectations.push_back( evaluators );
        }
    }

    /*
     * @brief Creation and running of the multi start job
     * ms stands for MultiStart.
     */
    void run_multistart(
            // parallel params
            int multistart_workers,
            unsigned int maxruns,
            // algo params
            eoCountContinue< daex::Decomposition >& maxgen,
            eoCountContinue< daex::Decomposition >& steadyfit,
            eoPop< daex::Decomposition >& pop,
            eoPopEvalFunc< daex::Decomposition >& pop_eval,
            eoAlgo< daex::Decomposition >& dae,
            // dump file params
            const std::string& plan_file,
            daex::Decomposition::Fitness best_makespan,
            unsigned int & dump_file_count,
            const std::string& dump_sep,
            std::string& metadata,
            // out of time params
            unsigned int max_seconds
            )
    {
        int rank = eo::mpi::Node::comm().rank();

        eo::mpi::DynamicAssignmentAlgorithm msAssign( 1, multistart_workers );
        DaeResetter< daex::Decomposition > resetAlgo( maxgen, pop, pop_eval, steadyfit );

        eo::mpi::GetRandomSeeds< daex::Decomposition > seeds( eo::rng.rand() );
        eo::mpi::MultiStartStore< daex::Decomposition > store( dae, eo::mpi::DEFAULT_MASTER, resetAlgo, seeds );

        MultiDumpBestDecompo dumpBestDecompo( plan_file, best_makespan, false, dump_file_count, dump_sep, metadata );
        store.wrapHandleResponse( &dumpBestDecompo );

        if( max_seconds > 0 )
        {
            store.wrapIsFinished( new parallel::IsFinishedBeforeTime< eo::mpi::IsFinishedMultiStart< daex::Decomposition > >( max_seconds ) );
        }

        eo::mpi::MultiStart< daex::Decomposition > msjob( msAssign, eo::mpi::DEFAULT_MASTER, store, maxruns );

        eo::log << eo::progress << "[" << rank << "] Multi start job created." << std::endl;
        msjob.run();
    }
}

# endif // __MAKE_PARALLEL_MULTISTART_DAE_H__

