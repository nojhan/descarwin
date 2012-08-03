# ifndef __MAKE_PARALLEL_EVAL_DAE_H__
# define __MAKE_PARALLEL_EVAL_DAE_H__

# include <mpi/eoMpi.h>
# include <mpi/eoParallelApply.h>
# include <mpi/eoTerminateJob.h>

# include <string> // std::string
# include <fstream> // std::ofstream

# include <utils/evalBestPlanDump.h>

namespace daex
{
    /**
     * @brief Handle response wrapper which saves the best solution's plan into a file, after receving it.
     *
     * Dump of the best solution should be done by the master, and only by the master, otherwise workers would erase the
     * solution files by replacing them, and the files would possibly not contain the best solutions.
     *
     * Filename follow the same configuration that the present one in make_eval :
     * <afilename><sep><file_count>
     */
    struct HandleResponseBestPlanDump : public eo::mpi::HandleResponseParallelApply< daex::Decomposition >, public BestPlanDumpper<daex::Decomposition::Fitness>
    {
        /**
         * @brief Main constructor.
         *
         * @param afilename Base filename
         * @param worst Worst value possible for makespan
         * @param single_file Do we have to save the solutions in a single file ?
         * @param file_count At how many solutions are we ?
         * @param sep Separator between parts of the file name.
         * @param metadata Metadata which will be written at top of the dump files.
         */
        HandleResponseBestPlanDump(
                std::string afilename,
                daex::Decomposition::Fitness worst,
                bool single_file = false,
                unsigned int file_count = 0,
                std::string sep = ".",
                std::string metadata = ""
                ) :
            BestPlanDumpper<daex::Decomposition::Fitness>( afilename, worst, single_file, file_count, sep, metadata )
        {
            // empty
        }

        /**
         * @brief Reimplementation of operator()
         *
         * After having applied the wrapped functor, which will effectively retrieve the response, we have to look at
         * the evaluated individuals to see if one of them has a better makespan than the former best solution.
         */
        void operator()( int wrkRank )
        {
            (*_wrapped)( wrkRank );
            int index = _data->assignedTasks[ wrkRank ].index;
            int size = _data->assignedTasks[ wrkRank ].size;
            for( int i = 0; i < size; ++i )
            {
                daex::Decomposition & decompo = _data->table()[ index + i ];
                if( decompo.is_feasible() && decompo.plan().makespan() < _best )
                {
                    _best = decompo.plan().makespan() ;
                    dump( decompo );
                }
            }
        }
    };

    /**
     * @brief Wrapper to stop the execution if a set amount of time is reached.
     *
     * Don't forget that once the exception is thrown, the master will wait for the last responses, which could take
     * time. The real termination time could be slightly bigger than the given one.
     */
    template< class IsFinishedJob >
    struct IsFinishedBeforeTime : public IsFinishedJob
    {
        /**
         * @brief Main ctor
         *
         * @param maxTime Time (in seconds) after which we want to stop the process.
         */
        IsFinishedBeforeTime( long maxTime ) : _maxTime( maxTime )
        {
            getrusage( RUSAGE_SELF , &_usage );
            _current = _usage.ru_utime.tv_sec + _usage.ru_stime.tv_sec;
        }

        /**
         * @brief Reimplementation of operator()()
         *
         * If we've reached the time limit, throw an exception. Otherwise, returns wrapped result.
         */
        bool operator()()
        {
            getrusage( RUSAGE_SELF , &_usage );
            _current = _usage.ru_utime.tv_sec + _usage.ru_stime.tv_sec;
            if ( _current > _maxTime )
            {
                throw eoMaxTimeException( _current );
            }
            return (*this->_wrapped)();
        }

        protected:
        const long _maxTime;
        long _current;

        struct rusage _usage;
    };

    eoPopEvalFunc<daex::Decomposition>* do_make_parallel_eval(
            eoEvalFunc<daex::Decomposition>& eval,
            std::string & plan_file,
            TimeVal best_makespan,
            unsigned int dump_file_count,
            std::string & dump_sep,
            std::string & metadata,
            unsigned int max_seconds,
            /* multistart parameters */
            bool with_multistart,                       // do we use multi start?
            int masterRank,                             // what is the master rank?
            const std::vector<int>& workers )           // what are the workers for this parallel eval?
    {
        eoPopEvalFunc<daex::Decomposition>* p_pop_eval;

        bool parallelLoopEval = eo::parallel.isEnabled();
        // We should return a parallel evaluation operator if:
        // - parallel loop eval is activated
        // and
        // - in multistart mode, we're not the general master (all the other will use parallel evaluation).
        if( parallelLoopEval && ( !with_multistart || eo::mpi::Node::comm().rank() != eo::mpi::DEFAULT_MASTER ) )
        {
            // Type of scheduling
            eo::mpi::AssignmentAlgorithm* assign;

            if ( eo::parallel.isDynamic() )
            {
                assign = new eo::mpi::DynamicAssignmentAlgorithm( workers ) ;
            } else
            {
                assign = new eo::mpi::StaticAssignmentAlgorithm( workers, 0 ) ; // remark: second parameter doesn't mean anything here.
            }

            unsigned int packet_size = eo::parallel.packetSize();

            eo::mpi::ParallelApplyStore<daex::Decomposition>* store
                = new eo::mpi::ParallelApplyStore<daex::Decomposition>( eval, masterRank, packet_size );

            // In multistart mode, it's the general master who dumps the solution and stops after a certain amount of
            // time.
            // TODO: the master waits for the last responses to come. If a start has a long duration, the program will
            // leave long after the max_seconds limit. The evaluators should also stop at this time and send their best
            // solution to master at this moment: this would require an IsFinished handler which sends the solution if
            // the limit of time is reached.
            if( !with_multistart )
            {
                // Wrap handle response to include the dump file saving.
                store->wrapHandleResponse( new HandleResponseBestPlanDump(
                            plan_file,
                            best_makespan,
                            false,
                            dump_file_count,
                            dump_sep,
                            metadata
                            )
                        );

                if( max_seconds > 0 )
                {
                    store->wrapIsFinished( new IsFinishedBeforeTime< eo::mpi::IsFinishedParallelApply< daex::Decomposition > >( max_seconds ) );
                }
            }

            // Add wrappers
            p_pop_eval = new eoParallelPopLoopEval<daex::Decomposition>(
                    *assign,
                    masterRank,
                    store
                    );
        } else
        {
            p_pop_eval = new eoPopLoopEval<daex::Decomposition>( eval );
        }
        return p_pop_eval;
    }

} // namespace daex

# endif //__MAKE_PARALLEL_EVAL_DAE_H__
