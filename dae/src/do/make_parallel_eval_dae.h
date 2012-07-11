# ifndef __MAKE_PARALLEL_EVAL_DAE_H__
# define __MAKE_PARALLEL_EVAL_DAE_H__

# include <mpi/eoMpi.h>
# include <mpi/eoParallelApply.h>
# include <mpi/eoTerminateJob.h>

# include <string>
# include <fstream>

// TODO TODOB comment
namespace daex
{
    template< class T >
        struct HandleResponseBestPlanDump : public eo::mpi::HandleResponseParallelApply< daex::Decomposition >
    {
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

        // TODO TODOB copié / collé de evalBestPlanDump.h
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

        void operator()( int wrkRank )
        {
            (*_wrapped)( wrkRank );
            int index = d->assignedTasks[ wrkRank ].index;
            int size = d->assignedTasks[ wrkRank ].size;
            for( int i = 0; i < size; ++i )
            {
                daex::Decomposition & decompo = d->data()[ index + i ];
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

    struct IsFinishedBeforeTime : public eo::mpi::IsFinishedParallelApply< daex::Decomposition >
    {
        IsFinishedBeforeTime( long maxTime ) : _maxTime( maxTime )
        {
            getrusage( RUSAGE_SELF , &_usage );
            _current = _usage.ru_utime.tv_sec + _usage.ru_stime.tv_sec;
        }

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
            unsigned int max_seconds )
    {
        eoPopEvalFunc<daex::Decomposition>* p_pop_eval;

        bool parallelLoopEval = eo::parallel.isEnabled();
        if( parallelLoopEval )
        {
            eo::mpi::AssignmentAlgorithm* assign;
            if ( eo::parallel.isDynamic() )
            {
                assign = new eo::mpi::DynamicAssignmentAlgorithm ;
            } else
            {
                assign = new eo::mpi::StaticAssignmentAlgorithm ;
            }

            unsigned int packet_size = eo::parallel.packetSize();

            eo::mpi::ParallelApplyStore<daex::Decomposition>* store
                = new eo::mpi::ParallelApplyStore<daex::Decomposition>( eval, eo::mpi::DEFAULT_MASTER, packet_size );
            store->wrapHandleResponse( new HandleResponseBestPlanDump<TimeVal>(
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
                store->wrapIsFinished( new IsFinishedBeforeTime( max_seconds ) );
            }

            // Add wrappers
            p_pop_eval = new eoParallelPopLoopEval<daex::Decomposition>(
                    *assign,
                    eo::mpi::DEFAULT_MASTER /* master rank */,
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
