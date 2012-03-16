

#ifndef __monitoring_h__
#define __monitoring_h__

#include <eo>

namespace daex {

template< class EOT >
class eoSolutionsDump : public eoSortedStat<EOT, std::string> 
{
public:
    using eoSortedStat<EOT, std::string>::value;

    eoSolutionsDump( std::string description = "Solutions Dump" ) : 
        eoSortedStat<EOT, std::string>("", description), // param value, description
        _iteration(0), _solution(0)
    {}

    std::string format( const EOT& decompo, bool best_in_iteration )
    {
        std::ostringstream os;

        // JSON formating
        os << "{"; 
        os << "\"id\":\"" << _solution << "\",";
        os << "\"plan\":\"" << decompo.plan_copy() << "\",";
        os << "\"makespan\":\"" << decompo.plan().makespan() << "\",";
        os << "\"decomposition\":\"" << decompo << "\",";
        os << "\"expanded_nodes\":\"" << decompo.plan().search_steps() << "\",";
    
        // FIXME utiliser un operateur de comptage d'iteration externe au lieu de maintenir un compteur interne
        os << "\"at_iteration\":\"" << _iteration << "\",";

        // FIXME faux numéro d'évaluation, utiliser un eval_counter externe
        os << "\"at_evaluation\":\"" << _solution << "\",";
        
        // first, the best decomposition
        // because we are in a _sorted_ stat
        os << "\"is_best_in_iteration\":\"" << best_in_iteration << "\"";
        os << "}" << std::endl;

        _solution++;

        return os.str();
    }
    
    void operator()(const std::vector<const EOT*>& pop)
    {
        std::ostringstream os;
       
        os << format( pop[0], true );
        
        for( unsigned int i=1; i<pop.size(); ++i ) {
            os << format( pop[i], false );
        }

        value() = os.str();
    }

private:
    /** An intern iteration counter,
     * each call to operator() is supposed to be a new iteration
     */
    unsigned int _iteration;

    //! An intern solution counter
    unsigned int _solution;
};

} // namespace daex

#endif

