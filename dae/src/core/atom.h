
#ifndef __ATOM_H__
#define __ATOM_H__

#include <string>
#include <vector>
#include <iterator>
#include <list>
#include <cassert>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>


#include <gmpxx.h>
extern "C" {
#include <src/cpt.h>
#include <src/branching.h>
#include <src/structs.h>
}

//#include <eo>

#include "utils/pddl.h"

namespace daex
{

class Atom
{
public:
    //! Constructeur prédicats + objets
    Atom( pddlPredicate * predicate, std::vector< pddlObject * > objects ) :
        _predicate(predicate), _objects(objects), _earliest_start_time(0)
    { }
    

    //! Constructeur prédicats + objets + date au plus tot
    Atom( pddlPredicate * predicate, std::vector< pddlObject * > objects, TimeVal start_time ) :
        _predicate(predicate), _objects(objects), _earliest_start_time(start_time)
    { }

    //! Constructeur prédicats + objets + date au plus tot 
    //               + pointeur vers structure interne CPT/YAHSP
    /*Atom( pddlPredicate * predicate, std::vector< pddlObject * > objects, TimeVal start_time, Fluent * p_fluent ) :
        _predicate(predicate), _objects(objects), _earliest_start_time(start_time), _cpt_fluent(p_fluent)
    { }*/
    
    //! Accesseurs
    pddlPredicate *             predicate()           const { return _predicate; }
    std::vector< pddlObject * > objects()             const { return _objects; }
    TimeVal                     earliest_start_time() const { return _earliest_start_time; }


    /*! Renvoi le pointeur vers la structure de donnée du solveur indiqué,
     *  en faisant un casting vers le type de pointeur demandé.
     *
     * Attention, le type renvoyé est casté à partir d'un void* et doit etre un pointeur.
     *
     * Attention, le C++ ne peut inférer le type du template à partir du type de retour
     * le type de renvoi doit donc etre demandé  explicitement via le template
     * et doit etre le meme que celui qui est placé dans le dictionnaire lors de l'initialisation.
     *
     * Exemple :
     *   Fluent f;
     *   Atom atom( ...blabla... );
     *   atom.add_fluent("yahsp", &f);
     *   Fluent* pf = atom.fluent_of<Fluent*>("yahsp");
     */
    template<class PT>
    PT fluent_of( const std::string solver ) const
    { 
        // si aucune clef correspondante
        if( _fluents.count( solver ) == 0 ) {
            std::ostringstream msg;
            msg << "There is no fluent registered for the solver \"" << solver << "\" in atom " << *this;
            throw std::range_error( msg.str() );
        }
        return static_cast<PT>( _fluents.find(solver)->second ); 
    }
   

    //! Ajoute un pointeur vers la structure de donnée propre au solveur.
    template<class PT>
    void store_fluent_of( const std::string solver, PT p_fluent ) 
    {
        assert( p_fluent != NULL );
        _fluents[ solver ] = static_cast<void*>( p_fluent );
    }


protected:
    pddlPredicate *             _predicate;
    std::vector< pddlObject * > _objects;

    TimeVal _earliest_start_time;

    //Fluent * _cpt_fluent;

    /*! Dictionnaire reliant le nom d'un solveur 
     * à un pointeur vers sa structure de donnée maintenant les fluents
     *
     * Le dico contient un pointeur sans type, qu'il faut caster à la demande.
     */
    std::map<const std::string, void*> _fluents;

public:

    friend std::ostream& operator<<( std::ostream& out, const Atom & atom )
    {
        // (_predicate, _objects ...)
        out << "(Atom[" << atom.earliest_start_time() << "]: " << *atom._predicate << " ";
         
        std::vector<pddlObject*>::const_iterator it = atom._objects.begin();
        out << *(*it);
        it++;
        while( it != atom._objects.end() ) {
            out << " " << *(*it);
            it++;
        }

        out << ")";
    };
};

} // namespace daex

#endif

