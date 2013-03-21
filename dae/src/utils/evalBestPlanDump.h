
#ifndef eoEvalBestPlanFileDump_H
#define eoEvalBestPlanFileDump_H

#include <fstream>

#include <eoEvalFunc.h>
#include <utils/eoParam.h>

#include "core/decomposition.h"

# ifdef WITH_MPI
# include <mpi/eoMpi.h>
# endif // WITH_mpi

#define IPC_PLAN_COMMENT "; "

namespace daex {

template< class T, class EOT >
class BestPlanDumpper
{
    public:

        BestPlanDumpper(
                std::string afilename,
                T worst,
                bool single_file = false,
                unsigned int file_count = 0,
                std::string sep = ".",
                std::string metadata = ""
                )
            : _filename(afilename), _best(worst), _single_file(single_file), _file_count(file_count), _sep(sep), _metadata(metadata)
        {}

    protected:

        std::string _filename;
        T _best;
        bool _single_file;
        unsigned int _file_count;
        std::string _sep;
        std::string _metadata;

        std::ofstream _of;

    public:
        void dump( EOT & eo )
        {
            if( _single_file ) {
                // explicitely erase the file before writing in it
                _of.open( _filename.c_str(), std::ios_base::out | std::ios_base::trunc );

            } else {
                std::ostringstream afilename;
                afilename << _filename << _sep << _file_count;
                //            _of.open( afilename.str().c_str(), std::ios_base::out | std::ios_base::trunc ); //  valeurs par défaut.
                _of.open( afilename.str().c_str());
            }

            // BUG 
            //_of.open( this->filename(), std::ios_base::out | std::ios_base::trunc );

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

        unsigned int file_count() { return _file_count; }

        T best() { return _best; }
};

/**
Dump an evaluated plan to a given file if it has the best "value" found so far, use derived class to use either makespan or fitness as "value".

Note: test if the file could be open only in debug mode
If the file cannot be open during the calls, everything will fail in an standard exception.
*/
template<class T, class EOT>
class evalBestPlanDump : public eoEvalFunc<EOT>, public BestPlanDumpper<T,EOT>
{
public:
    evalBestPlanDump(
            eoEvalFunc<EOT>& func, 
            std::string afilename, 
            T worst,
            bool single_file = false,
            std::string sep = ".",
            unsigned int file_count = 0,
            std::string metadata = ""
    )
        : BestPlanDumpper<T,EOT>( afilename, worst, single_file, file_count, sep, metadata ), _func(func)
    {}

        void operator()( EOT & eo )
    {
        if( eo.invalid() ) {
            // don't forget to call the embedded evaluator
            _func( eo );

            if( eo.is_feasible() ) {
//#pragma omp critical
# ifdef WITH_MPI
                if( eo::mpi::Node::comm().rank() == eo::mpi::DEFAULT_MASTER )
# endif
                call( eo );
            } // if feasible
        } // if invalid
    }

protected:
    virtual void call( EOT & eo ) = 0;

    /* TODO on x86-64, when called inside a ofstream::open, this functional call returns a corrupted string
    const char * filename()
    {
        if( _single_file ) {
            return _filename.c_str();

         } else {
            std::ostringstream afilename;
            afilename << _filename << _sep << _file_count;
            return afilename.str().c_str();
         }
    }
    */

    eoEvalFunc<EOT>& _func;
}; // class evalBestPlanDump


/** 
Dump an evaluated plan to a given file if it has the best makespan found so far

Note: test if the file could be open only in debug mode
If the file cannot be open during the calls, everything will fail in an standard exception.
*/
template<class EOT>
class evalBestMakespanPlanDump : public evalBestPlanDump<TimeVal,EOT>
{
public:
    evalBestMakespanPlanDump(
            eoEvalFunc<EOT>& func, 
            std::string afilename, 
            TimeVal worst,
            bool single_file = false,
            std::string sep = ".",
            unsigned int file_count = 0,
            std::string metadata = ""
    )
        : evalBestPlanDump<TimeVal,EOT>( func, afilename, worst, single_file, sep, file_count, metadata )
    {
    }

protected:
    virtual void call( EOT & eo )
    {
        if( eo.plan().makespan() < this->_best ) {
# ifdef WITH_OMP
#pragma omp critical
# endif // WITH_OMP
            this->_best = eo.plan().makespan();
            this->dump( eo );
        }// if better
    }
};


/** 
Dump an evaluated plan to a given file if it has the best fitness found so far

Note: test if the file could be open only in debug mode
If the file cannot be open during the calls, everything will fail in an standard exception.
*/
template<class EOT>
class evalBestFitnessPlanDump  : public evalBestPlanDump<typename EOT::Fitness, EOT>
{
public:
    evalBestFitnessPlanDump(
            eoEvalFunc<EOT>& func,
            std::string afilename,
            typename EOT::Fitness worst,
            bool single_file = false,
            std::string sep = ".",
            unsigned int file_count = 0,
            std::string metadata = ""
    )
        : evalBestPlanDump<typename EOT::Fitness,EOT>( func, afilename, worst, single_file, sep, file_count, metadata )
    {
    }

protected:
    virtual void call( EOT & eo )
    {
        // Note: in EO, maximizing by default, later overloaded for minimizing
        // OMP DIRTY - first comparison is unprotected, in order to be non-blocking
        if( eo.fitness() > this->_best ) {
# ifdef WITH_OMP
#pragma omp critical
# endif // WITH_OMP
            if( eo.fitness() > this->_best ) {
                this->_best = eo.fitness();
                this->dump( eo );
            }
        } // if better
    }
};

} // namespace daex

#endif // eoEvalBestPlanFileDump_H


