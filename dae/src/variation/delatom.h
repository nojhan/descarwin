
#ifndef __DAEX_MUTATION_DELATOM_H__
#define __DAEX_MUTATION_DELATOM_H__

#include <iostream>

#include <eo>

#include "core/decomposition.h"

namespace daex {


//! 
class MutationDelAtom: public eoMonOp<Decomposition>
{
public:

    MutationDelAtom( double proba_del_atom ) : _proba_del_atom( proba_del_atom ) 
    {
        if( _proba_del_atom < 0 || _proba_del_atom > 1 ) {
            std::ostringstream msg;
            msg << "Probability to delete an atom =" << _proba_del_atom << ", should be in [0,1]"; 
            throw std::range_error( msg.str() );
        }
    }

    bool operator()( Decomposition & decompo );

protected:
    double _proba_del_atom;
};


//! Delete one atom randomly chosen in [0,last_reached[
class MutationDelOneAtom: public eoMonOp<Decomposition>
{
public:
    bool operator()( Decomposition & decompo );
};


} // namespace daex

#endif // __DAEX_MUTATION_DELATOM_H__
