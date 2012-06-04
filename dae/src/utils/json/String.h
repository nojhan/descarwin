# ifndef __EOSERIAL_STRING_H__
# define __EOSERIAL_STRING_H__

# include <string>
# include <sstream>

# include "Entity.h"

namespace eoserial
{

/**
 * @brief JSON String
 *
 * Wrapper for string, so as to be used as a JSON object.
 */
class String : public eoserial::Entity, public std::string
{
    public:

        /**
         * @brief Default ctor.
         * @param str The string we want to wrap.
         */
        String( const std::string& str ) : std::string( str ) {}

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
        virtual std::ostream& print( std::ostream& out ) const;

        /**
         * @brief Deserializes the current String into a given primitive type value.
         * @param value The value in which we're writing.
         */
        template<class T>
        inline void deserialize( T & value );

    protected:
        // Copy and reaffectation are forbidden
        explicit String( const String& _ );
        String& operator=( const String& _ );
};

template<class T>
inline void String::deserialize( T & value )
{
    std::stringstream ss;
    ss << *this;
    ss >> value;
}

/**
 * @brief Specialization for strings, which don't need to be converted through
 * a stringstream.
 */
template<>
inline void String::deserialize( std::string & value )
{
    value = *this;
}

template <typename T>
String* String::make( const T & value )
{
    std::stringstream ss;
    ss << value;
    return new String( ss.str() );
}

/**
 * @brief Specialization for strings : no need to convert as they're still
 * usable as strings.
 */
template<>
inline String* String::make( const std::string & value )
{
    return new String( value );
}

} // namespace eoserial

# endif // __EOSERIAL_STRING_H__
