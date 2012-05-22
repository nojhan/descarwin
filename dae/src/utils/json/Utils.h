# ifndef __JSON_UTILS_H__
# define __JSON_UTILS_H__

# include "Array.h"
# include "Object.h"

namespace JsonUtils
{
    /**
     * @brief Retrieves an object which has been JSON-serialized.
     *
     * The parameter T matches the type of the object we're gonna
     * deserialize. The object must implement JsonSerializable so that
     * process to be correct.
     *
     * @param json The JSON containing the object.
     * @return The object initialized.
     */
    template<typename T>
    inline T getObject( json::Entity* json )
    {
        T ret;
        ret.fromJson( static_cast<json::Object*>( json ) );
        return ret;
    }

    /**
     * @brief Converts a JSON entity into a JSON array.
     */
    inline json::Array* getArray( json::Entity* json )
    {
        return static_cast<json::Array*>( json );
    }

    /**
     * @brief Retrieves the primitive type T from the JSON string and returns it.
     *
     * In fact, T should be a primitive type OR a type which could be printed in
     * a stream.
     *
     * @param json The JSON string containing the variable we're deserializing.
     * @return The primitive value we want.
     */
    template<typename T>
    inline T get ( json::Entity* json )
    {
        std::stringstream ss;
        ss << static_cast<json::String*>( json )->content();
        T ret;
        ss >> ret;
        return ret;
    }
}
# endif //__JSON_UTILS_H__

