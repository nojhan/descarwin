
#ifndef _FeasibleRatioStat_h_
#define _FeasibleRatioStat_h_

#include <algorithm>

#include <utils/eoStat.h>

namespace daex {

/** Ratio of the number of feasible individuals in the population
 *
 */

template< class EOT>
bool isFeasible ( const EOT & sol ) { return sol.is_feasible();}


template<class EOT>
class FeasibleRatioStat : public eoStat< EOT, double >
{
public:
    using eoStat<EOT, double>::value;

    FeasibleRatioStat( std::string description = "FeasibleRatio(%)" ) : eoStat<EOT,double>( 0.0, description ) {}

    virtual void operator()( const eoPop<EOT> & pop )
    {
#ifndef NDEBUG
        assert( pop.size() > 0 );

        double count = static_cast<double>( std::count_if( pop.begin(), pop.end(), isFeasible<EOT> ) );
        double size = static_cast<double>( pop.size() );
        double ratio = count/size;
        value() = ratio;
#else
        value() = static_cast<double>( std::count_if( pop.begin(), pop.end(), isFeasible<EOT> ) ) / static_cast<double>( pop.size() );
	
#endif
	 value() *=100;
		
    }

 virtual std::string className(void) const { return "FeasibleRatioStat"; }
};

} // namespace daex

#endif
