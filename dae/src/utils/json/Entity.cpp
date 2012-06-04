# include "Entity.h"

using namespace eoserial;

std::ostream& operator<<( std::ostream& out, const Entity* json )
{
    return ( json ) ? json->print( out ) : out;
}

