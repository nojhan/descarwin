# include "String.h"

namespace json
{

std::ostream& String::print( std::ostream& out ) const
{
    out << '"' << _content << '"';
    return out;
}

} // namespace json

