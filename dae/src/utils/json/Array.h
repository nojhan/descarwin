# ifndef __JSON_ARRAY_H__
# define __JSON_ARRAY_H__

# include <vector>
# include <iostream>

# include "Entity.h"
# include "Serializable.h"
# include "Object.h"

# include "UtilsSignatures.h"

namespace json
{

/**
 * @brief Represents a JSON array.
 *
 * Wrapper for an array, so as to be used as a JSON object.
 */
class Array : public json::Entity
{
    public:
        /**
         * @brief Adds the child to the array.
         * @param child JSON object, string or array.
         */
        void push_back( json::Entity* child )
        {
            children.push_back( child );
        }

        /**
         * @brief Adds the serializable object as a JSON object.
         * @param obj Object which implemnets JsonSerializable.
         */
        void push_back( json::Serializable* obj )
        {
            children.push_back( obj->toJson() );
        }

        /**
         * @brief Reimplementation of operator[], so as to use JsonArray as an vector
         * @param i Index of wanted child.
         * @return Pointer to the child.
         */
        json::Entity* operator[]( int i )
        {
            return children[ i ];
        }

        /**
         * @brief Reimplementation of size(), so as to use JsonArray as a vector.
         * @return Size of the children vector.
         */
        size_t size() const
        {
            return children.size();
        }

        /**
         * @brief Prints the JSON array into the given stream.
         * @param out The stream
         */
        virtual std::ostream& print( std::ostream& out ) const;

        /**
         * @brief Dtor
         */
        ~Array();

        /*
         * Some cool accessors which call JsonUtils, see JsonUtils.
         */
        // FIXME Find a way to put in common this part with JsonObject.h

        template<typename T>
        T get ( int i ) const
        {
            return JsonUtils::get<T>( children[i] );
        }

        template<typename T>
        // T should be JsonSerializable
        T getObject( int i ) const
        {
            return JsonUtils::getObject<T>( children[i] );
        }

        template<class Type, JSON_STL_CONTAINER Container, class GetAlgorithm>
        void getCompletedArray( unsigned int i ) const
        {
            return JsonUtils::getCompletedArray<Type, Container, GetAlgorithm>( children[ i ] );
        }

        const json::Array* getArray( unsigned int i ) const
        {
            return JsonUtils::getArray( children[ i ] );
        }    
    
    protected:
        typedef std::vector<json::Entity*> ArrayChildren;
        ArrayChildren children;
};

} // namespace json

# endif // __JSON_ARRAY_H__

