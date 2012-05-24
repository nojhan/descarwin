# ifndef __JSON_OBJECT_H__
# define __JSON_OBJECT_H__

# include <map>
# include <string>
# include <sstream>

# include "Entity.h"
# include "String.h"
# include "Serializable.h"

# include "UtilsSignatures.h"

namespace json
{

class Array; // to avoid recursive inclusion

/**
 * @brief JSON Object
 *
 * This class represents a JSON object, which is basically a dictionnary
 * of keys (strings) and values (JSON entities).
 */
class Object : public json::Entity
{
    public:
        typedef std::map<std::string, const json::Entity*> JsonValues;

        /**
         * @brief Returns values contained in the object.
         */
        JsonValues & values() { return _values; }

        /**
         * @brief Prints the content of a JSON object into a stream.
         */
        virtual std::ostream& print( std::ostream& out ) const;

        /**
         * @brief Implementation of operator [] to have direct access to map.
         *
         * This allows to directly retrieve values by given keys and to register
         * easily new pairs.
         */
        json::Entity*& operator[]( const std::string& str );
        json::Entity*& operator[]( const char* str );

        /**
         * @brief Adds a pair into the JSON object.
         * @param key The key associated with the json object
         * @param json The JSON object as created with framework.
         */
        void addPair( const std::string& key, const json::Entity* json )
        {
            _values[ key ] = json;
        }

        /**
         * @brief Adds a pair into the JSON object.
         * @param key The key associated with the json object
         * @param obj A JSON-serializable object
         */
        void addPair( const std::string& key, const json::Serializable* obj )
        {
            _values[ key ] = obj->toJson();
        }

        /*
         * Some cool accessors which call JsonUtils, see JsonUtils.
         */
        // FIXME Find a way to put in common this part with JsonArray.h

        template<typename T>
        T get ( const std::string& key ) const
        {
            return JsonUtils::get<T>( _values.find( key )->second );
        }

        template<typename T>
        // T should be JsonSerializable
        T getObject( const std::string& key ) const
        {
            return JsonUtils::getObject<T>( _values.find( key )->second );
        }

        template<class Type, JSON_STL_CONTAINER Container, template <typename Type> class GetAlgorithm>
        Container<Type>* getCompletedArray( const char* key ) const
        {
            return JsonUtils::getCompletedArray<Type, Container, GetAlgorithm>( _values.find( key )->second );
        }

        const json::Array* getArray( const std::string& key ) const
        {
            return JsonUtils::getArray( _values.find( key )->second );
        }

        /**
         * @brief Dtor
         */
        ~Object();

    protected:
        JsonValues _values;
};

} // namespace json

# endif // __JSON_OBJECT_H__
