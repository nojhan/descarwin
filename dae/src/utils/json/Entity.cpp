# include "Entity.h"

using namespace json;

std::ostream& operator<<( std::ostream& out, Entity* json )
{
    return ( json ) ? json->print( out ) : out;
}


