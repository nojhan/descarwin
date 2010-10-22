
#ifndef __DAEX_INIT_H__
#define __DAEX_INIT_H__

#include <map>
#include <algorithm>

#include "core/decomposition.h"
#include "utils/pddl_load.h"
#include "utils/utils.h"

namespace daex {

//template< class EOT = daex::Decomposition>
class Init : public eoInit<Decomposition>
{
public:
    // TODO autoriser les décomposition vides ? (_min_nb = 0 )
    Init( const ChronoPartition & times, unsigned int l_max_init_coef = 2, unsigned int min_nb = 1 );

    void operator()( Decomposition & decompo );

public:
    void l_max( unsigned int l ) { _l_max = l; }
    unsigned int l_max() const { return _l_max; }


protected:
    // TODO faire des tests pour vérifier si doit etre à 0 ou à 1 par défaut
    const unsigned int _min_nb;

    const unsigned int _l_max_init_coef;

    const ChronoPartition & _times;

    //! Taille maximale d'une décomposition
    unsigned int _l_max;
};

} // namespace daex

#endif // __DAEX_INIT_H_

