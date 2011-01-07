
#ifndef eoEvalBestPlanFileDump_H
#define eoEvalestPlanFileDump_H

#include <fstream>

#include <eoEvalFunc.h>
#include <utils/eoParam.h>

#include "core/decomposition.h"

namespace daex {

/** 
Dump the individual to a given file if it has the best fitness found so far

Note: test if the file could be open only at the instanciation. 
If the file cannot be open during the calls, everything will fail in an standard exception.
*/
class eoEvalBestPlanFileDump : public eoEvalFunc<Decomposition>
{
public:
    eoEvalBestPlanFileDump(
            eoEvalFunc<Decomposition>& func, 
            std::string afilename, 
            Decomposition::Fitness worst_fitness,
            bool single_file = false,
            std::string sep = ".",
            unsigned int file_count = 0)
        : _func(func), _filename(afilename),_best_fitness(worst_fitness), _sep(sep),
          _single_file(single_file), _file_count(file_count)
    {
        /*
        // to test if the file can be opened, we use the "app" mode, 
        // thus the file is not erased (which is the case in the default "out" mode)
        _of.open( filename(), std::ios_base::out | std::ios_base::app );

        if ( !_of.is_open() ) {
            std::string str = "Error, eoEvalBestFileDump could not open: " + _filename;
            throw std::runtime_error( str );

        } else {
            _of.close();
        }
        */
    }


    virtual void operator()(Decomposition & eo)
    {
        if( eo.invalid() ) {
            // don't forget to call the embedded evaluator
            _func( eo );

            // FIXME use the real cost instead of the fitness?
            if( eo.fitness().is_feasible() ) {
              
                if( eo.fitness() > _best_fitness ) {

                    _best_fitness = eo.fitness();

                    if( _single_file ) {
                         // explicitely erase the file before writing in it
                         _of.open( _filename.c_str(), std::ios_base::out | std::ios_base::trunc );

                     } else {
                        std::ostringstream afilename;
                        afilename << _filename << _sep << _file_count;
                        _of.open( afilename.str().c_str(), std::ios_base::out | std::ios_base::trunc );
                     }

                    // BUG 
                    //_of.open( this->filename(), std::ios_base::out | std::ios_base::trunc );

#ifndef NDEBUG
                    if ( !_of.is_open() ) {
                        std::string str = "Error, eoEvalBestFileDump could not open: " + _filename;
                        throw std::runtime_error( str );

                    }
                    _of << "; " << eo << std::endl;
#endif

                    // here, we assume that the file could be opened, as it has been tested in the constructor
                    // thus, we avoid a supplementary test in this costly evaluator
                    _of << eo.plan() << std::endl;

                    // TODO may we would want to write the decomposition in the output file also?
                    
                    _of.close();

                    _file_count++;

                } // if better
            } // if feasible
        } // if invalid
    }


    Decomposition::Fitness best_fitness() { return _best_fitness; }
    unsigned int file_count() { return _file_count; }

protected:
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
    std::ofstream _of;
    Decomposition::Fitness _best_fitness;
    bool _single_file;
    unsigned int _file_count;
    std::string _sep;
};

} // namespace daex

#endif
