# ifndef __JSON_SERIALIZABLE_H__
# define __JSON_SERIALIZABLE_H__

# include <string>

namespace json
{

class Object; // to avoid recursive inclusion with JsonObject

/**
 * @brief Interface showing that class which implements it can be serialized
 * in JSON.
 *
 * Note : JsonSerializable objects should have a default constructor taking
 * no parameters.
 */
class Serializable
{
    public:
    /**
     * @brief Serializes the object to JSON format.
     * @return A JSON object created with new.
     */
    virtual json::Object* toJson() const = 0;

    /**
     * @brief Loads class fields from a JSON object.
     * @param json A JSON object. Programmer doesn't have to delete it, it
     * is automatically done.
     */
    virtual void fromJson(const json::Object* json) = 0;
};

} // namespace json

# endif // __JSON_SERIALIZABLE_H__
