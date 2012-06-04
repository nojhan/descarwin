# ifndef __EOSERIAL_ARRAY_H__
# define __EOSERIAL_ARRAY_H__

# include <vector>
# include <iostream>

# include "Entity.h"
# include "Serializable.h"

# include "Object.h"
# include "String.h"

namespace eoserial
{

/**
 * @brief Represents a JSON array.
 *
 * Wrapper for an array, so as to be used as a JSON object.
 */
class Array : public eoserial::Entity, public std::vector< eoserial::Entity* >
{
    protected:
        typedef std::vector< eoserial::Entity* > ArrayChildren;

    public:
        /**
         * @brief Adds the serializable object as a JSON object.
         * @param obj Object which implemnets JsonSerializable.
         */
        void push_back( eoserial::Serializable* obj )
        {
            ArrayChildren::push_back( obj->toJson() );
        }

        /**
         * @brief Proxy for vector::push_back.
         */
        void push_back( eoserial::Entity* json )
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
            virtual void operator()( const eoserial::Array* array, unsigned int i, T & value ) const = 0;
            virtual void operator()( const eoserial::Object* obj, const std::string& key, T & value ) const = 0;
        };

        template<typename T, typename JsonEntity>
        struct UnpackAlgorithm : public BaseAlgorithm<T>
        {
            void operator()( const eoserial::Array* array, unsigned int i, T & value ) const
            {
                array->unpack( i, value );
            }

            void operator()( const eoserial::Object* obj, const std::string& key, T & value ) const
            {
                obj->unpack( key, value );
            }
        };

        template<typename Type, template <typename T, typename alloc = std::allocator<T> > class Container>
        inline Container<Type>* deserialize( const eoserial::Array* json, const BaseAlgorithm<Type> & algo )
        {
            Container<Type>* array = new Container<Type>( eoserial->size() );
            for( unsigned int i = 0, size = eoserial->size();
                    i < size;
                    ++i)
            {
                Type t;
                algo( eoserial, 
                array->push_back( t );
            }
            return array;
        }
        */
};

} // namespace eoserial

# endif // __EOSERIAL_ARRAY_H__

