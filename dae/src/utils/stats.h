
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
	    /*#ifndef NEBDUG
            os << "; Decomposition " << best_decompo << std::endl;
            os << "; sub-plans :" << std::endl;
            os << "; ******************************************************* " << std::endl;
            for( unsigned int i=0, end=best_decompo.subplans_size(); i<end; ++i ) {
                os << "; subplan " << i << std::endl;

		//               std::cout << "; subplan " << i << " / decomposition_size " << best_decompo.size() << " / subplans_size " << best_decompo.subplans_size() << std::endl;

                // copy the subplan so as to avoid a reference that would crash in CPT/print_plan
                daex::Plan subplan = best_decompo.subplan(i);
                os << subplan << std::endl;
            }
            os << "; ******************************************************* " << std::endl;
            
	    #endif*/
            os << plan;
            value() = os.str();
        }
        // else do nothing // Idée à creuser : retun false et sinon true : pour éviter de réécrire à chaque géneration dans le fichier. eoFileMonitor tiendrait alors en compte le retour de la fonction et si false ne fait rien.
    }

protected:
    FitnessT _best_fitness;

};

#endif // __stats_h__
