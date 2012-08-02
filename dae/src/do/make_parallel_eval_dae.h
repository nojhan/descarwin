# ifndef __MAKE_PARALLEL_EVAL_DAE_H__
# define __MAKE_PARALLEL_EVAL_DAE_H__

# include <mpi/eoMpi.h>
# include <mpi/eoParallelApply.h>
# include <mpi/eoTerminateJob.h>

# include <string> // std::string
# include <fstream> // std::ofstream

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
    template< class T >
        struct HandleResponseBestPlanDump : public eo::mpi::HandleResponseParallelApply< daex::Decomposition >
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
                T worst,
                bool single_file = false,
                unsigned int file_count = 0,
                std::string sep = ".",
                std::string metadata = ""
                ) :
            _filename( afilename ),
            best( worst ), // The best at initialization is in fact the worst value we can have.
            _single_file( single_file ),
            _file_count( file_count ),
            _sep( sep ),
            _metadata( metadata )
        {
            // empty
        }

        // TODO This part is in common with evalBestPlanDump.h
        void dump( daex::Decomposition & eo )
        {
            std::ofstream _of;

            if( _single_file ) {
                // explicitely erase the file before writing in it
                _of.open( _filename.c_str(), std::ios_base::out | std::ios_base::trunc );

            } else {
                std::ostringstream afilename;
                afilename << _filename << _sep << _file_count;
                _of.open( afilename.str().c_str() );
            }

#ifndef NDEBUG
            if ( !_of.is_open() ) {
                std::string str = "Error, eoEvalBestFileDump could not open: " + _filename;
                throw std::runtime_error( str );
            }
            _of << IPC_PLAN_COMMENT << _metadata << std::endl;
            _of << IPC_PLAN_COMMENT << eo << std::endl;
#endif
            // here, in release mode, we assume that the file could be opened
            // thus, we avoid a supplementary test in this costly evaluator
            _of << eo.plan() << std::endl;
            _of.close();

            _file_count++;
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
                if( decompo.is_feasible() && decompo.plan().makespan() < best )
                {
                    best = decompo.plan().makespan() ;
                    dump( decompo );
                }
            }
        }

        protected:
        std::string _filename;
        T best;
        bool _single_file;
        unsigned int _file_count;
        std::string _sep;
        std::string _metadata;
    };

    /**
     * @brief Wrapper to stop the execution if a set amount of time is reached.
     *
     * Don't forget that once the exception is thrown, the master will wait for the last responses, which could take
     * time. The real termination time could be slightly bigger than the given one.
     */
    struct IsFinishedBeforeTime : public eo::mpi::IsFinishedParallelApply< daex::Decomposition >
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
            return (*_wrapped)();
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
            bool with_multistart,
            int masterRank,
            std::vector<int> workers )
    {
        eoPopEvalFunc<daex::Decomposition>* p_pop_eval;

        bool parallelLoopEval = eo::parallel.isEnabled();
        if( parallelLoopEval )
        {
            // Type of scheduling
            eo::mpi::AssignmentAlgorithm* assign;

            if( ! with_multistart )
            {
                workers.clear();
                for( int i = 1, size = eo::mpi::Node::comm().size(); i < size; ++i )
                {
                    workers.push_back( i );
                }
            }

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
            // Wrap handle response to include the dump file saving.
            store->wrapHandleResponse( new HandleResponseBestPlanDump<TimeVal>(
                        plan_file,
                        best_makespan,
                        false,
                        dump_file_count,
                        dump_sep,
                        metadata
                        )
                    );

            if( !with_multistart && max_seconds > 0 )
            {
                store->wrapIsFinished( new IsFinishedBeforeTime( max_seconds ) );
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
