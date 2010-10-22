
#ifndef __stats_h__
#define __stats_h__

#include <eo>

template< class EOT >
class eoBestIndividualStat : public eoSortedStat<EOT, std::string> 
{
public:
    using eoSortedStat<EOT, std::string>::value;

    eoBestIndividualStat( std::string description = "Best individual" ) : 
        eoSortedStat<EOT, std::string>("", description) // param value, description
    {}
    
    void operator()(const std::vector<const EOT*>& pop)  {
        std::ostringstream os;
        // use default formating printOn functions
        os << *(pop[0]);
        value() = os.str();
    }
};


template< class EOT >
class eoBestPlanStat : public eoBestIndividualStat<EOT> 
{
public: 
    using eoSortedStat<EOT, std::string>::value;

    eoBestPlanStat( std::string description = "Best plan" ) : 
        eoBestIndividualStat<EOT>( description ) // param value, description
    {}
    
    void operator()(const std::vector<const EOT*>& pop)  {
        std::ostringstream os;
        // use default formating printOn functions
        daex::Plan plan = (*(pop[0])).plan_copy(); 
        os << plan;
        value() = os.str();
    }
};


template< class EOT >
class eoBestPlanImprovedStat : public eoBestPlanStat< EOT >
{
public:
    typedef typename EOT::Fitness FitnessT;
    using eoSortedStat<EOT, std::string>::value;

    eoBestPlanImprovedStat( FitnessT worst_fitness, std::string description = "Best improved plan" ) : 
        _best_fitness( worst_fitness ), // badest fitness you can think of
        eoBestPlanStat<EOT>( description ) // param value, description
    {}
    
    void operator()(const std::vector<const EOT*>& pop)  
    {
        // get the best decomposition
        daex::Decomposition best_decompo = *( pop[0] );

        // if its fitness is better
        if( best_decompo.fitness() > _best_fitness ) {

            // save the best fitness
            _best_fitness = best_decompo.fitness();

            // print the corresponding compressed plan
            std::ostringstream os;
            daex::Plan plan = best_decompo.plan_copy(); 
            os << plan;
            value() = os.str();
        }
        // else do nothing
    }

protected:
    FitnessT _best_fitness;

};

#endif // __stats_h__
