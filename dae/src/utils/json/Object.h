# ifndef __EOSERIAL_OBJECT_H__
# define __EOSERIAL_OBJECT_H__

# include <map>
# include <string>
# include <sstream>

# include "Entity.h"
# include "String.h"
# include "Serializable.h"

namespace eoserial
{

/**
 * @brief JSON Object
 *
 * This class represents a JSON object, which is basically a dictionnary
 * of keys (strings) and values (JSON entities).
 */
class Object : public eoserial::Entity, public std::map< std::string, eoserial::Entity* >
{
    public:
        typedef std::map<std::string, eoserial::Entity*> JsonValues;

        /**
         * @brief Adds a pair into the JSON object.
         * @param key The key associated with the eoserial object
         * @param eoserial The JSON object as created with framework.
         */
        void addPair( const std::string& key, eoserial::Entity* json )
        {
            (*this)[ key ] = json;
        }

        /**
         * @brief Adds a pair into the JSON object.
         * @param key The key associated with the eoserial object
         * @param obj A JSON-serializable object
         */
        void addPair( const std::string& key, const eoserial::Printable* obj )
        {
            (*this)[ key ] = obj->pack();
        }

        /**
         * @brief Deserializes a Serializable class instance from this JSON object.
         * @param obj The object we want to rebuild.
         */
        void deserialize( eoserial::Persistent & obj )
        {
            obj.unpack( this );
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
        void unpackObject( const std::string& key, Persistent & value ) const
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

} // namespace eoserial
# endif // __EOSERIAL_OBJECT_H__

