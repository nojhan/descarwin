# ifndef __EOSERIAL_ENTITY_H__
# define __EOSERIAL_ENTITY_H__

# include <iostream>
# include <sstream>

namespace eoserial
{

/**
 * @brief JSON entity
 *
 * This class represents a JSON entity, which can be JSON objects,
 * strings or arrays. It is the base class for the JSON hierarchy.
 */
class Entity
{

public:

    /**
     * Virtual dtor (base class).
     */
    virtual ~Entity() { /* empty */ }

    /**
     * @brief Prints the content of a JSON object into a stream.
     * @param out The stream in which we're printing.
     */
    virtual std::ostream& print( std::ostream& out ) const = 0;

protected:
};

} // namespace eoserial

/**
 * @brief Prints the content of the eoserial object into the stream.
 * @param out The stream in which we're printing
 * @param json The json object (as a pointer) we want to print
 */
std::ostream& operator<< ( std::ostream& out, const eoserial::Entity * json );

# endif // __ENTITY_H__
