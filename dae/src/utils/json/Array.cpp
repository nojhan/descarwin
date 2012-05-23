# include "Array.h"

namespace json
{

std::ostream& Array::print( std::ostream& out )
{
    out << "[\n";
    bool first = true;
    for (ArrayChildren::iterator it = children.begin(),
            end = children.end();
            it != end;
            ++it)
    {
        if ( first )
        {
            first = false;
        } else {
            out << ",\n";
        }
        (*it)->print( out );
    }
    out << ']';
    return out;
}

Array::~Array()
{
    for (ArrayChildren::iterator it = children.begin(),
            end = children.end();
            it != end;
            ++it)
    {
        delete *it;
    }
}

} // namespace json
