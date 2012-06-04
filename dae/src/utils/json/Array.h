# ifndef __JSON_ARRAY_H__
# define __JSON_ARRAY_H__

# include <vector>
# include <iostream>

# include "Entity.h"
# include "Serializable.h"

# include "Object.h"
# include "String.h"

namespace json
{

/**
 * @brief Represents a JSON array.
 *
 * Wrapper for an array, so as to be used as a JSON object.
 */
class Array : public json::Entity, public std::vector< json::Entity* >
{
    protected:
        typedef std::vector< json::Entity* > ArrayChildren;

    public:
        /**
         * @brief Adds the serializable object as a JSON object.
         * @param obj Object which implemnets JsonSerializable.
         */
        void push_back( json::Serializable* obj )
        {
            ArrayChildren::push_back( obj->toJson() );
        }

        /**
         * @brief Proxy for vector::push_back.
         */
        void push_back( json::Entity* json )
        {
            ArrayChildren::push_back( json );
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

         /**
         * @brief Unpack primitive type stocked in the given index.
         * @param index The index in the array
         * @param value Instance of the primitive type in which will be written the value.
         */
        template<class T>
        void unpack( unsigned int index, T & value ) const
        {
            static_cast<String*>( (*this)[ index ] )->deserialize( value );
        }

        /**
         * @brief Unpack Serializable type stocked in the given index.
         * @param index The index in the array
         * @param value Instance object that we'll rebuild
         */
        void unpackObject( unsigned int index, Serializable & value ) const
        {
            static_cast<Object*>( (*this)[ index ] )->deserialize( value );
        }

        /*
        // TODO see if it's useful to have generic algorithms for retrieval
        template<typename T>
        struct BaseAlgorithm
        {
            virtual void operator()( const json::Array* array, unsigned int i, T & value ) const = 0;
            virtual void operator()( const json::Object* obj, const std::string& key, T & value ) const = 0;
        };

        template<typename T, typename JsonEntity>
        struct UnpackAlgorithm : public BaseAlgorithm<T>
        {
            void operator()( const json::Array* array, unsigned int i, T & value ) const
            {
                array->unpack( i, value );
            }

            void operator()( const json::Object* obj, const std::string& key, T & value ) const
            {
                obj->unpack( key, value );
            }
        };

        template<typename Type, template <typename T, typename alloc = std::allocator<T> > class Container>
        inline Container<Type>* deserialize( const json::Array* json, const BaseAlgorithm<Type> & algo )
        {
            Container<Type>* array = new Container<Type>( json->size() );
            for( unsigned int i = 0, size = json->size();
                    i < size;
                    ++i)
            {
                Type t;
                algo( json, 
                array->push_back( t );
            }
            return array;
        }
        */
};

} // namespace json

# endif // __JSON_ARRAY_H__

