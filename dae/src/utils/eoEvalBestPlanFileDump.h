
#ifndef eoEvalBestPlanFileDump_H
#define eoEvalestPlanFileDump_H

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
            bool overwrite = false,
            std::string sep = ".",
            unsigned int file_count = 0)
        : _func(func), _filename(afilename),_best_fitness(worst_fitness), _sep(sep),
          _overwrite(overwrite), _file_count(file_count)
    {
        // to test if the file can be opened, we use the "app" mode, 
        // thus the file is not erased (which is the case in the default "out" mode)
        _of.open( filename(), std::ios_base::out | std::ios_base::app );

        if ( !_of.is_open() ) {
            std::string str = "Error, eoEvalBestFileDump could not open: " + _filename;
            throw std::runtime_error( str );

        } else {
            _of.close();
        }
        
    }


    virtual void operator()(Decomposition & eo)
    {
        if( eo.invalid() ) {
            // don't forget to call the embedded evaluator
            _func( eo );

            if( eo.fitness() > _best_fitness ) {
                _best_fitness = eo.fitness();

                // explicitely erase the file before writing in it
                _of.open( filename(), std::ios_base::out | std::ios_base::trunc );

                // here, we assume that the file could be opened, as it has been tested in the constructor
                // thus, we avoid a supplementary test in this costly evaluator
                _of << eo.plan() << std::endl;

                // TODO may we would want to write the decomposition in the output file also?
                
                _of.close();

                _file_count++;

            } // if better
        } // if invalid
    }


    Decomposition::Fitness best_fitness() { return _best_fitness; }
    unsigned int file_count() { return _file_count; }

protected:
    const char * filename()
    {
        if( _overwrite ) {
            return _filename.c_str();

         } else {
            std::ostringstream filename;
            filename << _filename << _sep << _file_count;
            return filename.str().c_str();
         }
    }

    eoEvalFunc<Decomposition>& _func;
    std::string _filename;
    std::ofstream _of;
    Decomposition::Fitness _best_fitness;
    bool _overwrite;
    unsigned int _file_count;
    std::string _sep;
};

} // namespace daex

#endif
