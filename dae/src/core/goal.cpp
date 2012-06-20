
#include "goal.h"

namespace daex
{

Goal::iterator Goal::iter_at( unsigned int i )
{
    if( i >= this->size() ) {
        std::ostringstream msg;
        msg << "asked for element " << i << " but size of the Goal is " << this->size();
        throw( std::out_of_range( msg.str() ) );
    }

    Goal::iterator it = this->begin();

    std::advance(it,i);
    /*
    for( unsigned int j=0; j < i; ++i  ) {
            ++it;
    }
    */

    return it;
}

const std::vector< daex::Atom* > * Goal::_atoms;

} // namespace daex
