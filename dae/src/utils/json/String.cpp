# include "String.h"

namespace json
{
    std::ostream& String::print( std::ostream& out ) const
    {
        out << '"' << *this << '"';
        return out;
    }
} // namespace json

