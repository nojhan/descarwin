# include <map>
# include <string>
# include <sstream>
# include <vector>

# include "Parser.h"

# include "Array.h"
# include "Object.h"
# include "String.h"

// in debug mode only
// # define DEBUG(x) std::cout << x << std::endl;
# define DEBUG(x)

using namespace json;

namespace json
{

/**
 * @brief Parses a string contained between double quotes.
 *
 * Strings can contain escaped double quotes.
 * @param str The string we're parsing.
 * @param pos The index of current position in parsed string.
 * This index will be updated so as to allow the parser to
 * continue.
 */
static std::string parseString(const std::string& str, size_t & pos)
{
    // example : "hello"
    // example 2 : "\"world\""
    // for hello:
    // firstQuote == 0, secondQuote == 6
    // sub string should be from firstQuote+1 to secondQuote-1
    // so its size should be (secondQuote-1 -(firstQuote+1) + 1)
    std::string value;
    size_t firstQuote = str.find( '"', pos );
    size_t secondQuote;
    // instead of just seeking the second quote, we need to ensure
    // that there is no escaped quote before this one.
    do {
        ++pos;
        secondQuote = str.find( '"', pos );
    } while( str[ secondQuote - 1 ] == '\\' );
    value = str.substr( firstQuote+1, secondQuote-firstQuote-1 );
    pos = secondQuote + 1;
    return value;
}

/**
 * @brief Moves the given index pos to the next character which is
 * neither a coma, a space nor a new line.
 *
 * @param str The string in which we want to ignores those characters.
 * @param pos The index of current position in parsed string.
 */
static void ignoreChars(const std::string& str, size_t & pos)
{
    // ignore white spaces and comas
    for (char current = str[ pos ];
        current == ',' || current == ' ' || current == '\n';
        current = str[ ++pos ]);
}

String* Parser::parseJsonString(const std::string & str, size_t & pos)
{
    String* json = new String;
    json->content( parseString( str, pos ) );
    return json;
}

Object* Parser::parse(const std::string & str)
{
    size_t initial(0); // we begin at position 0
    return static_cast<Object*>( parseRight(str, initial) );
}

Entity* Parser::parseRight(const std::string & str, size_t & pos)
{
    Entity* value = 0;

    if ( str[ pos ] == '{' )
    {
        // next one is an object
        DEBUG("We read an object.")
        Object* obj = new Object;
        pos += 1;
        while( pos < str.size() && str[ pos ] != '}' )
        {
            parseLeft( str, pos, obj );
            ignoreChars( str, pos );
        }
        DEBUG("We just finished to read an object ! ")
        pos += 1; // we're on the }, go to the next char
        value = obj;
    }
    else if ( str[ pos ] == '"' )
    {
        // next one is a string
        DEBUG("We read a string")
        value = parseJsonString( str, pos );
    }
    else if ( str[ pos ] == '[' )
    {
        // next one is an array
        DEBUG("We read an array")
        Array* array = new Array;
        pos += 1;
        while( pos < str.size() && str[ pos ] != ']' )
        {
            Entity* child = parseRight( str, pos );
            if ( child )
                array->push_back( child );
        }
        DEBUG("We've finished to read our array.")
        pos += 1; // we're on the ], go to the next char
        value = array;
    }
    ignoreChars( str, pos );
    return value;
}

void Parser::parseLeft(const std::string & str, size_t & pos, Object* json)
{
    std::string key = parseString(str, pos);
    ++pos; // the colon
    DEBUG("We've read the key ")
    json->values()[ key ] = parseRight( str, pos ); 
}

} // namespace json

