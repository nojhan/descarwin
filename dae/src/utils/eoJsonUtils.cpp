# include "eoJsonUtils.h"

# include "utils/json/Object.h"
# include "utils/json/Parser.h"

# include <string>

namespace json
{

void printOn(const json::Serializable* obj, std::ostream& out)
{
    json::Object* jsonThis = obj->toJson();
    out << jsonThis;
    delete jsonThis;
}

void readFrom(json::Serializable* obj, std::istream& _is)
{
    std::string str;
    char temp[1024]; // FIXME taille max de buffer
    while( _is )
    {
        _is.getline( temp, 1024, '\n' );
        str += temp;
        str += '\n';
    }
    json::Object* read = json::Parser::parse( str );
    obj->fromJson( read );
    delete read;
}

} // namespace json

