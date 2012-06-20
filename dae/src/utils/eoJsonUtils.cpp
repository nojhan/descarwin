# include "eoJsonUtils.h"

# include <serial/Object.h>
# include <serial/Parser.h>

# include <string>

namespace eoserial
{

void printOn(const eoserial::Persistent& obj, std::ostream& out)
{
    eoserial::Object* jsonThis = obj.pack();
    jsonThis->print( out );
    delete jsonThis;
}

void readFrom(eoserial::Persistent& obj, std::istream& _is)
{
    std::string str;
    char temp[1024];
    while( _is )
    {
        _is.getline( temp, 1024, '\n' );
        str += temp;
        str += '\n';
    }
    eoserial::Object* read = eoserial::Parser::parse( str );
    obj.unpack( read );
    delete read;
}

} // namespace eoserial

