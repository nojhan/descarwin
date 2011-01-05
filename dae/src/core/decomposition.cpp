#include <sstream>
#include <iomanip>

#include "decomposition.h"
#include "goal.h"

namespace daex
{

void Decomposition::invalidate() 
{
    this->EO<eoMinimizingDualFitness>::invalidate();

    // clear the sub_plans vector
    // because if the decomposition becomes invalid, so are its intermediate plans
    this->plans_sub_reset();

    // plan vide
    this->_plan_global = Plan();

}

void Decomposition::plan_global( Plan p )
{
    _plan_global = p;
}

void Decomposition::plans_sub_add( Plan p ) 
{
    _plans_sub.push_back( p );
}

void Decomposition::plans_sub_reset()
{
    _plans_sub.clear();

    // on prévoit un sous-plan de plus que le nombre de stations
    // pour la dernière étape
    _plans_sub.reserve( this->size() + 1 );
}

void Decomposition::printOn( std::ostream & out ) const 
{
    EO< eoMinimizingDualFitness >::printOn(out);
    out << " ";

    out << "(Decomposition[" << this->size() << "]: ";

    std::copy( this->begin(), this->end(), std::ostream_iterator<Goal>(out, " ") );

    out << ")";
}

Decomposition::iterator Decomposition::iter_at( unsigned int i )
{
    if( i >= this->size() ) {
        std::ostringstream msg;
        msg << "asked for element " << i << " but size of the Decomposition is " << this->size();
        throw( std::out_of_range( msg.str() ) );
    }

    Decomposition::iterator it = this->begin();

    std::advance( it, i );
    /*
    for( unsigned int j=0; j < i; ++i  ) {
            ++it;
    }
    */

    return it;
}

void simplePrint( std::ostream & out, Decomposition & decompo )
{
    out << "Decomposition " 
        << std::right << std::setfill(' ') << std::setw(3) 
        << "[" << decompo.size() << "]: ";

    for( Decomposition::iterator igoal = decompo.begin(), end = decompo.end(); igoal != end; ++igoal ) {
        out << "\t" << std::right << std::setfill(' ') << std::setw(3)
            << igoal->earliest_start_time() << "(" << igoal->size() << ")";
    }

    out << std::endl;
}

} // namespace daex

