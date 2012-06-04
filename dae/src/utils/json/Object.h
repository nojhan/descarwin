# ifndef __JSON_OBJECT_H__
# define __JSON_OBJECT_H__

# include <map>
# include <string>
# include <sstream>

# include "Entity.h"
# include "String.h"
# include "Serializable.h"

namespace json
{

/**
 * @brief JSON Object
 *
 * This class represents a JSON object, which is basically a dictionnary
 * of keys (strings) and values (JSON entities).
 */
class Object : public json::Entity, public std::map< std::string, json::Entity* >
{
    public:
        typedef std::map<std::string, json::Entity*> JsonValues;

        /**
         * @brief Adds a pair into the JSON object.
         * @param key The key associated with the json object
         * @param json The JSON object as created with framework.
         */
        void addPair( const std::string& key, json::Entity* json )
        {
            (*this)[ key ] = json;
        }

        /**
         * @brief Adds a pair into the JSON object.
         * @param key The key associated with the json object
         * @param obj A JSON-serializable object
         */
        void addPair( const std::string& key, json::Serializable* obj )
        {
            (*this)[ key ] = obj->toJson();
        }

        /**
         * @brief Deserializes a Serializable class instance from this JSON object.
         * @param obj The object we want to rebuild.
         */
        void deserialize( Serializable & obj )
        {
            obj.fromJson( this );
        }

        /**
         * @brief Unpack primitive type stocked in the given key.
         * @param key The key in the map
         * @param value Instance of the primitive type in which will be written the value.
         */
        template<class T>
        void unpack( const std::string& key, T & value ) const
        {
            static_cast<String*>( this->find( key )->second )->deserialize( value );
        }

        /**
         * @brief Unpack Serializable type stocked in the given key.
         * @param key The key in the map
         * @param value Instance object that we'll rebuild
         */
        void unpackObject( const std::string& key, Serializable & value ) const
        {
            static_cast<Object*>( this->find( key )->second )->deserialize( value );
        }

        /**
         * @brief Dtor
         */
        ~Object();

    protected:

        /**
         * @brief Prints the content of a JSON object into a stream.
         */
        virtual std::ostream& print( std::ostream& out ) const;
};

} // namespace json
# endif // __JSON_OBJECT_H__

