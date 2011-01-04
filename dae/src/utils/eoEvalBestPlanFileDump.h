
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
    public :
        eoEvalBestPlanFileDump(eoEvalFunc<Decomposition>& func, std::string filename, Decomposition::Fitness worst_fitness ) 
            : _func(func), _filename(filename),_best_fitness(worst_fitness)
        {
            std::ofstream os( _filename.c_str() );

            if (!os) {
                std::string str = "Error, eoEvalBestFileDump could not open: " + _filename;
                throw std::runtime_error( str );
            }
        }


        virtual void operator()(Decomposition & eo)
        {
            if( eo.invalid() ) {
                // don't forget to call the embedded evaluator
                _func( eo );

                if( eo.fitness() > _best_fitness ) {
                    _best_fitness = eo.fitness();

                    std::ofstream os( _filename.c_str() );

                    // here, we assume that the file could be opened, as it has been tested in the constructor
                    // thus, we avoid a supplementary test in this costly evaluator
                    //os << eo;
                    os << eo.plan();

                } // if better
            } // if invalid
        }

    protected :
        eoEvalFunc<Decomposition>& _func;
        std::string _filename;
        Decomposition::Fitness _best_fitness;
};

} // namespace daex

#endif
