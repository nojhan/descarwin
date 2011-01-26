
#ifndef __PDDL_DAE_H__
#define __PDDL_DAE_H__

#include <iostream>
#include <string>
#include <vector>


namespace daex
{

//! Type de l'objet dans le PDDL (cf. PDDLType)
class pddlType 
{
public:
    pddlType( std::string name ) : _name(name) { }

    std::string name() const { return this->_name; }

protected:
    /* in some context that requires _name to be assignable - possible in a container (like a map, vector or something else). This will fail, because of the implicitly defined copy assignment operator */
    /*const*/ std::string _name;

public:
    friend std::ostream& operator<<( std::ostream& out, const pddlType & type )
    {
        out << type._name;
	return out;
    }
};


/** Objet du problème (cf. PDDLTerm : is_var=False)
 *
 * Stocké dans une pair
 */

class pddlObject
{
public:
    //! Constructeur
    pddlObject( std::vector< pddlType > types, 
                std::string value 
              ) : _types(types), _value(value) { }

public:
    //! Vecteur ordonné des types ancestraux du plus spécifique au plus général
    std::vector< pddlType > types() const { return this->_types; }

    //! Valeur
    std::string value() const { return this->_value; }

protected:
    const std::vector< pddlType > _types;
    const std::string             _value;

public:
    friend std::ostream& operator<<( std::ostream& out, const pddlObject & obj )
    {
        out << obj._value;
	return out;
    }
};


//! 
class pddlPredicate
{
public:
    pddlPredicate( std::string name, unsigned int arity ) : _name(name), _arity(arity) { }

public:
    std::string name() const { return this->_name; }

    unsigned int arity() const { return this->_arity; }
    
protected:
    const std::string _name;

    const unsigned int _arity;

public:
    friend std::ostream& operator<<( std::ostream& out, const pddlPredicate & pred )
    {
        // _predicate
        out << pred._name;
	return out;
    }
};

} // namespace daex

#endif
