# ifndef __JSON_UTILS_H__
# define __JSON_UTILS_H__

# include "Array.h"
# include "Object.h"

# include "UtilsSignatures.h"

namespace JsonUtils
{

    template<typename T>
    inline T getObject( const json::Entity* json )
    {
        T ret;
        ret.fromJson( static_cast<const json::Object*>( json ) );
        return ret;
    }

        template<typename T>
    inline T get ( const json::Entity* json )
    {
        std::stringstream ss;
        ss << static_cast<const json::String*>( json )->content();
        T ret;
        ss >> ret;
        return ret;
    }

    inline const json::Array* getArray( const json::Entity* json )
    {
        return static_cast<const json::Array*>( json );
    }

    template<typename Type, JSON_STL_CONTAINER Container, template <typename Type> class Algorithm >
    inline Container<Type>* getCompletedArray( const json::Entity* json )
    {
        // return static_cast<const json::Array*>( json );
        const json::Array* jsonArray = static_cast<const json::Array*>( json );
        Container<Type>* array = new Container<Type>( jsonArray->size() );
        Algorithm<Type> algo;
        for( unsigned int i = 0, size = jsonArray->size();
                i < size;
                ++i)
        {
            array->push_back( algo( jsonArray, i ) );
        }
        return array;
    }
    
    template<typename T>
    struct GetAlgorithm : public BaseAlgorithm<T>
    {
        T operator()( const json::Array* array, unsigned int i )
        {
            return array->get<T>( i );
        }

        T operator()( const json::Object* obj, const std::string& key )
        {
            return obj->get<T>( key );
        }
    };
    
    template<typename T>
    struct BaseAlgorithm
    {
        virtual T operator()( const json::Array* array, unsigned int i ) = 0;
        virtual T operator()( const json::Object* obj, const std::string& key ) = 0;
    };

    template<typename T>
    struct GetObjectAlgorithm : public BaseAlgorithm<T>
    {
        T operator()( const json::Array* array, unsigned int i )
        {
            return array->getObject<T>( i );
        }

        T operator()( const json::Object* obj, const std::string& key )
        {
            return obj->getObject<T>( key );
        }
    };
}
# endif //__JSON_UTILS_H__

