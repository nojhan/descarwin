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
    while( _is )
    {
        std::string temp;
        _is >> temp;
        str += temp;
    }
    obj->fromJson( json::Parser::parse( str ) );
}

} // namespace json

