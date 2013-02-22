#include <sstream>
#include <iomanip>

#include "decomposition.h"
#include "goal.h"

#include "utils/eoJsonUtils.h"

namespace daex
{


void Decomposition::readFrom( std::istream & is )
{
    eoserial::readFrom( *this, is );
}

void Decomposition::printOn( std::ostream & out ) const
{
    eoserial::printOn( *this, out );
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

