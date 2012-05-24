# ifndef __JSON_UTILS_SIGN_H__
# define __JSON_UTILS_SIGN_H__

namespace json
{
    // to avoid circular inclusions
    class Entity;
    class Array;
}

#define JSON_STL_CONTAINER template <typename T, typename alloc = std::allocator<T> > class

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
    T getObject( const json::Entity* json );
    
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
    T get ( const json::Entity* json );

    /**
     * @brief Converts a JSON entity into a JSON array.
     */
    const json::Array* getArray( const json::Entity* json );

    /**
     * @brief Retrieves the array saved, and completes a Container<Type> list with
     * the contained objects, thanks to retrieval algorithm Algorithm.
     *
     * @param Type The type of contained objects in the array
     * @param Container Which STL container among std::list or std::vector would you like
     * to use ?
     * @param Algorithm The retrieval algorithm, to choose among GetAlgorithm,
     * GetObjectAlgorithm or your own algorithm based on BaseAlgorithm.
     *
     * @param json The json array.
     * @return Container<Type> already completed with all elements contained in json.
     */
    template<typename Type, JSON_STL_CONTAINER Container, template <typename Type> class Algorithm >
    Container<Type>* getCompletedArray( const json::Entity* json );
    
    /**
     * @brief Base class for retrieval algorithms.
     *
     * This interfaces shows 2 methods for retrieve variables, which are contained
     * in the one hand in an array and in the other hand in an object.
     *
     * They must return the retrieved variable.
     */
    template<typename T>
    struct BaseAlgorithm;

    /**
     * @brief Retrieval algorithm based on method get<T>, which is for primitive and
     * printable types.
     */
    template<typename T>
    struct GetAlgorithm;

    /**
     * @brief Retrieval algorithm based on method getObject<T>, which is for 
     * json::Serializable types.
     */
    template<typename T>
    struct GetObjectAlgorithm;
}
# endif //__JSON_UTILS_SIGN_H__

