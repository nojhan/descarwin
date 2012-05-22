# ifndef __JSON_STRING_H__
# define __JSON_STRING_H__

# include <string>
# include <sstream>

# include "Entity.h"

namespace json
{

/**
 * @brief JSON String
 *
 * Wrapper for string, so as to be used as a JSON object.
 */
class String : public json::Entity
{
    public:

        /**
         * @brief Default ctor.
         * @param str The string we want to wrap.
         */
        String( const std::string& str ) : _content( str ) {}

        /**
         * @brief Ctor used only on parsing.
         */
        String( ) {}

        /**
         * @brief Casts a value of a stream-serializable type (i.e, which implements
         * operator <<) into a JsonString.
         *
         * This is used when serializing the objects : all primitives types should be
         * converted into strings to get more easily manipulated.
         *
         * @param value The value we're converting.
         * @return JsonString wrapper for the value.
         */
        template<typename T>
        static String* make( const T & value );

        /**
         * @brief Prints out the string.
         */
        virtual std::ostream& print( std::ostream& out );

        // Getter and setter for content
        std::string content() { return _content; }
        void content( std::string str ) { _content = str; }

    protected:
        std::string _content;

        // Copy and reaffectation are forbidden
        explicit String( const String& _ );
        String& operator=( const String& _ );
};

template <typename T>
String* String::make( const T & value )
{
    std::stringstream ss;
    ss << value;
    String* json = new String;
    json->_content = ss.str();
    return json;
}

/**
 * @brief Specialization for strings : no need to convert as they're still
 * usable as strings.
 */
template<>
inline String* String::make( const std::string & value )
{
    String* json = new String;
    json->_content = value;
    return json;
}

} // namespace json

# endif // __JSON_STRING_H__
