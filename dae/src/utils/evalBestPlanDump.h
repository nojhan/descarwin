
#ifndef eoEvalBestPlanFileDump_H
#define eoEvalestPlanFileDump_H

#include <fstream>

#include <eoEvalFunc.h>
#include <utils/eoParam.h>

#include "core/decomposition.h"

#define IPC_PLAN_COMMENT "; "

namespace daex {

/** 
Dump an evaluated plan to a given file if it has the best "value" found so far, use derived class to use either makespan or fitness as "value".

Note: test if the file could be open only in debug mode
If the file cannot be open during the calls, everything will fail in an standard exception.
*/
template<class T>
class evalBestPlanDump : public eoEvalFunc<Decomposition>
{
public:
    evalBestPlanDump(
            eoEvalFunc<Decomposition>& func, 
            std::string afilename, 
            T worst,
            bool single_file = false,
            std::string sep = ".",
            unsigned int file_count = 0,
            std::string metadata = ""
    )
        : _func(func), _filename(afilename), _single_file(single_file), _file_count(file_count), _sep(sep), _metadata(metadata), _best(worst)
    {}

    void dump( Decomposition & eo )
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

    void operator()( Decomposition & eo )
    {
        if( eo.invalid() ) {
            // don't forget to call the embedded evaluator
            _func( eo );

            if( eo.is_feasible() ) {
//#pragma omp critical
                call( eo );
            } // if feasible
        } // if invalid
    }

    T best() { return _best; }
    unsigned int file_count() { return _file_count; }


protected:
    virtual void call( Decomposition & eo ) = 0;

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

    eoEvalFunc<Decomposition>& _func;
    std::string _filename;
    bool _single_file;
    unsigned int _file_count;
    std::string _sep;
    std::string _metadata;
    T _best;
    std::ofstream _of;

}; // class evalBestPlanDump


/** 
Dump an evaluated plan to a given file if it has the best makespan found so far

Note: test if the file could be open only in debug mode
If the file cannot be open during the calls, everything will fail in an standard exception.
*/
class evalBestMakespanPlanDump : public evalBestPlanDump<TimeVal>
{
public:
    evalBestMakespanPlanDump(
            eoEvalFunc<Decomposition>& func, 
            std::string afilename, 
            TimeVal worst,
            bool single_file = false,
            std::string sep = ".",
            unsigned int file_count = 0,
            std::string metadata = ""
    )
        : evalBestPlanDump<TimeVal>( func, afilename, worst, single_file, sep, file_count, metadata )
    {
    }

protected:
    virtual void call( Decomposition & eo );
};


/** 
Dump an evaluated plan to a given file if it has the best fitness found so far

Note: test if the file could be open only in debug mode
If the file cannot be open during the calls, everything will fail in an standard exception.
*/
class evalBestFitnessPlanDump  : public evalBestPlanDump<Decomposition::Fitness>
{
public:
    evalBestFitnessPlanDump(
            eoEvalFunc<Decomposition>& func, 
            std::string afilename, 
            Decomposition::Fitness worst,
            bool single_file = false,
            std::string sep = ".",
            unsigned int file_count = 0,
            std::string metadata = ""
    )
        : evalBestPlanDump<Decomposition::Fitness>( func, afilename, worst, single_file, sep, file_count, metadata )
    {
    }

protected:
    virtual void call( Decomposition & eo );
};

} // namespace daex

#endif // eoEvalBestPlanFileDump_H


