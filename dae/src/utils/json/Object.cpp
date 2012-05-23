# include "Object.h"

using namespace json;

namespace json
{

std::ostream& Object::print( std::ostream& out )
{
    out << '{';
    bool first = true;
    for(JsonValues::iterator it = _values.begin(), end = _values.end();
            it != end;
          ++it)
    {
        if ( first )
        {
            first = false;
        } else {
            out << ", ";
        }

        out << '"' << it->first << "\":";   // key
        it->second->print( out );           // value
        }
    out << '}';
    return out;
}

Object::~Object()
{
    for(JsonValues::iterator it = _values.begin(), end = _values.end();
            it != end;
          ++it)
    {
        delete it->second;
    }
}

Entity*& Object::operator[]( const std::string& str )
{
    return _values[ str ];
}

Entity*& Object::operator[]( const char* str )
{
    return _values[ str ];
}

} // namespace json
