#include <sys/time.h>
#include <sys/resource.h>

#include <utility>
#include <sstream>
#include <stdexcept>
#include <numeric>

#include "yahsp.h"
#include "utils/utils.h"

std::ostream & operator<<( std::ostream & out, BitArray bitarray )
{
    //out << bitarray_get( bitarray, fluents[0] );
    for( unsigned int i = 1; i < fluents_nb; ++i ) {
        unsigned long bit = bitarray_get( bitarray, fluents[i] );
        //out << " " << bit;
        if( bit != 0 ) {
            out << " " << fluent_name(fluents[i]);
        }
    }

    return out;
}


daeYahspEval::daeYahspEval( 
            unsigned int l_max_ /*= 20*/, 
            unsigned int b_max_in /*= 10*/, 
            unsigned int b_max_last /*=30*/, 
            double fitness_weight /*= 10*/,
            double fitness_penalty /*= 1e6*/
        ) :
        daeCptYahspEval( l_max_,b_max_in, b_max_last, fitness_weight, fitness_penalty ),
        _previous_state( NULL ), _intermediate_goal_state(NULL), _intermediate_goal_state_nb(0)
{
    // some init steps are not done here, but in pddl_load.cpp
    // notably the call to cpt_main
    // because it can only be called once
        
    _previous_state = bitarray_create( fluents_nb );
    
    _intermediate_goal_state = (Fluent**) malloc( fluents_nb * sizeof( Fluent* ) );

    if( _intermediate_goal_state == NULL ) {
        std::ostringstream msg;
        msg << "Error: cannot allocate an intermediate goal state of size " << fluents_nb << std::endl;
        throw std::runtime_error( msg.str().c_str() );
    }
}


daeYahspEval::~daeYahspEval()
{
    free( _previous_state );
    free( _intermediate_goal_state );
}


/**************************************************************************************************************
 * SOLVE NEXT
 **************************************************************************************************************/
unsigned int daeYahspEval::solve_next( daex::Decomposition & decompo, Fluent** next_state, unsigned int next_state_nb )
{
    // copy the current_state of yahsp in _previous_state, 
    // in order to compute the distance between those two states
    // during the bad fitness computation
    //_previous_state = bitarray_create( fluents_nb );
    bitarray_copy( _previous_state, *get_current_state(), fluents_nb );
   
#ifndef NDEBUG
    eo::log << eo::xdebug << "ok" << std::endl;
    eo::log << eo::xdebug << "\t\tcall the solver...";
    eo::log.flush();
#endif

    /* 
     * Appel au solveur:
           int cpt_search(Fluent **init, long init_nb, Fluent **goals, long goals_nb, 
                          bool compress, bool compress_causals, bool compress_orderings) 
     *
     * return codes utilisés par yahsp (définis dans plan.h, utilisés dans yahsp.c):
            PLAN_FOUND 0
            NO_PLAN 5
     * return code utilisé par cpt_search (solve.c)
            GOALS_MUTEX 1
     *
     * solution trouvée est dans la variable globale : 
            SolutionPlan* solution_plan
     *
     * avec :
            struct SolutionPlan {
              VECTOR(Step *, steps);
              TimeVal makespan;
              double search_time;
              double total_time;
              double backtracks;
            };
     * 
     * Note : _init_state n'est utilisé qu'au tout début de l'appel, 
     * si un premier yahsp_reset a été fait. yahsp_main maintient
     * un initial_bitstate et un current_state au fur et à mesure de la recherche.
     * Il n'y a donc pas besoin de mettre à jour l'état courant passé en premier
     * argument à cpt_search.
     */
    /*
    BitArray my_initial_bitstate = bitarray_create(fluents_nb);
    FOR(f, init_state) { bitarray_set(my_initial_bitstate, f); } EFOR;

    eo::log << eo::logging << "init_state=";
    for( unsigned int i = 0; i < init_state_nb; ++i ) {
      eo::log << eo::logging << fluent_name(init_state[i]);
    }

    eo::log << eo::logging << "my_initial_bitstate=";
    for( unsigned int i = 0; i < (fluents_nb - 1) / __WORDSIZE + 1; ++i ) {
      eo::log << eo::logging << my_initial_bitstate[i] << " ";
    }

    eo::log << eo::logging << "next_state=";
    for( unsigned int i = 0; i < next_state_nb; ++i ) {
      eo::log << eo::logging << fluent_name(next_state[i]);
    }
    */
    unsigned int return_code = cpt_search( init_state, init_state_nb, next_state, next_state_nb, false, false, false );

    //    eo::log << eo::logging << "\t\treturn code: " << return_code << std::endl;

#ifndef NDEBUG
    eo::log << eo::xdebug << "ok" << std::endl;
    eo::log << eo::xdebug << "\t\treturn code: " << return_code << " ";
    eo::log.flush();
#endif

    if( return_code == NO_PLAN || return_code == GOALS_MUTEX ) {

      //decompo.fitness( std::make_pair( fitness_unfeasible( decompo, _previous_state ), false ) );
      //      std::cout << "stats.evaluated_nodes = " << stats.evaluated_nodes << std::endl;
      //      eo::log << eo::logging << " FAIL: " << decompo << std::endl;

      step_recorder_fail();

#ifndef NDEBUG
        eo::log << eo::debug << "x";
        eo::log.flush();
        eo::log << eo::xdebug << std::endl;
#endif


    } else if( return_code == PLAN_FOUND ) {
        //assert( return_code == PLAN_FOUND );
        assert( solution_plan != NULL );


        if( solution_plan->steps_nb > 0 )  {

            // in ipc6/seq-sat/elevators-strips, there is actions with a null cost
            // thus, the only way to be sure that the plan is usefull is te steps_nb
            //assert(solution_plan->makespan > 0);

            // un goal utile supplémentaire
            _u++;

            // un certains nombre de NOEUDS supplémentaires
            // (stocké dans le champs "backtracks" par yahsp, à cause de cpt)
            _B += static_cast<unsigned int>( solution_plan->backtracks );
        }

        // incrémente le compteur de plan
        _k++;

        // Note : ici, l'exécution du plan est inutile avec yahsp, 
        // car il a déjà construit le prochain état initial

        // stocke le plan intermédiaire dans la variable globale "plans"
        // qui sera utilisée par yahsp lors de la compression
        // Note : yahsp_main écrase le prochain pointeur solution_plan avec 
        // une nouvelle allocation.
        plans[plans_nb] = solution_plan;
        plans_nb++;
        

        // convertit et stocke les plans intermédiaires dans des structures propres à DAEx
        decompo.plans_sub_add( daex::Plan( *solution_plan ) );

        // timer à jour pour le dernier plan inséré
        decompo.last_subplan().search_steps( _B );

        
#ifndef NDEBUG
        eo::log << eo::xdebug << "ok" << std::endl;
        eo::log << eo::xdebug << "\t\trecord steps...";
        eo::log.flush();
#endif


        // Récupère le nombre de noeuds utilisés par une résolution avec yahsp
        // utilisé uniquement par la classe daeEvalYahspInit 
        // pour le calcul de la médiane des noeuds pour l'initialisation du b_max
        // dans la classe daeEvalYahsp, la fonction est vide
        step_recorder();


#ifndef NDEBUG
        eo::log << eo::xdebug << "ok" << std::endl;
#endif

    } else { // return_code != NO_PLAN && != GOALS_MUTEX && != PLAN_FOUND
        throw std::runtime_error( "Unkonwn error code from cpt_search" );
    }

    solution_plan = NULL;

    return return_code;
}


/**************************************************************************************************************
 * COMPRESS
 **************************************************************************************************************/
void daeYahspEval::compress( daex::Decomposition & decompo )
{
#ifndef NDEBUG
        eo::log << eo::xdebug << "\t\tcompression...";
        eo::log.flush();
#endif


        //solution_plan = NULL;

        // compression, utilise la globale "plans" construire plus haut
        // et créé un plan compressé dans solution_plan
        yahsp_compress_plans();

        assert(solution_plan != NULL);

        // TODO pendant les tests, le plan ne peut pas etre vide, mais en compétition, cela peut arriver, auquel cas il faudra virer l'assert (penser à compiler en NDEBUG)

	//	std::cout << "MAKESPAAAAN =" << solution_plan->makespan;

        assert(solution_plan->makespan > 0);


#ifndef NDEBUG
        eo::log << eo::xdebug << "ok" << std::endl;
        eo::log << eo::xdebug << "\t\tsave compressed plan and fitness...";
        eo::log.flush();
#endif

        
        // sauvegarde le plan compressé global pour DAEx
        decompo.plan_global( daex::Plan( *solution_plan ) );

        decompo.last_subplan().search_steps( _B );

        // change la fitness
        //decompo.fitness( std::make_pair( fitness_feasible( decompo ), true ) );


#ifndef NDEBUG
        eo::log << eo::xdebug << "ok" << std::endl;
#endif
}


/**************************************************************************************************************
 * CALL
 **************************************************************************************************************/
void daeYahspEval::call( daex::Decomposition & decompo ) 
{

#ifndef NDEBUG
    eo::log << eo::xdebug << "decompo.size=" << decompo.size() << std::endl;
#endif

#ifndef NDEBUG
//    eo::log << eo::xdebug << "CHECK ASSERT" << std::endl;
//    daex::Decomposition tmp;
//    std::copy( decompo.begin(), decompo.end(), std::back_inserter( tmp ) );
//    tmp.front().push_back( tmp.front().front() );
//    for( daex::Decomposition::iterator igoal = tmp.begin(), goal_end = tmp.end(); igoal != goal_end; ++igoal ) {
//        assert_nomutex_noduplicate( igoal->begin(), igoal->end() );
//    }

    eo::log << eo::xdebug << "Check goal consistency" << std::endl;
    for( daex::Decomposition::iterator igoal = decompo.begin(), goal_end = decompo.end(); igoal != goal_end; ++igoal ) {
        assert_noduplicate( igoal->begin(), igoal->end() );
        assert_nomutex(     igoal->begin(), igoal->end() );
    }
#endif

    if( ! decompo.invalid() ) {

#ifndef NDEBUG
        eo::log << eo::debug << "-";
        eo::log.flush();
#endif

        // do nothing

    } else { // if decompo.invalid
        
      // JACK the code does not even try to evaluate decompositions that are too long 
      // FIXME what is the effect on variation operators that relies on last_reached?
      if( decompo.size() > _l_max ) {
	decompo.fitness( std::make_pair( fitness_unfeasible_too_long(), false ) );

      } else {
      
#ifndef NDEBUG
        eo::log << eo::xdebug << "malloc plans...";
        eo::log.flush();
#endif

        cpt_malloc( plans, decompo.size()+1 ); // +1 for the subplan between the last goal and the final state
        plans_nb = 0;
        
#ifndef NDEBUG
        eo::log << eo::xdebug << "ok" << std::endl;
        eo::log << eo::xdebug << "yahsp reset...";
        eo::log.flush();
#endif

        // TODO autoriser les décompositions vides ?
        // TODO garder les plans intermédiaires non compressés pour éviter de les recalculer ?

        // nécéssaire pour indiquer à yahsp que la prochaine recherche doit se faire à partir
        // de l'état initial (variable globale init_state)
        // initialise le current_state à initial_bitstate
        yahsp_reset();

#ifndef NDEBUG
        eo::log << eo::xdebug << "ok" << std::endl;
#endif

        // compteur de goals
        _k = 0;
        // compteur de goals utiles
        _u = 0;
        // compteur des tentatives de recherche
        _B = 0;


#ifndef NDEBUG
        eo::log << eo::xdebug << "for each goal:" << std::endl;
#endif

        // only use for xdebug messages
        unsigned int goal_count = 0;
           
        // return code of cpt_search
        unsigned int code;

        // set the generic b_max 
        b_max( _b_max_in );

        // parcours les goals de la décomposition
        for( daex::Decomposition::iterator igoal = decompo.begin(), iend = decompo.end(); igoal != iend; ++igoal ) {

#ifndef NDEBUG
            eo::log << eo::xdebug << "\t\tcopy of states and fluents...";
            eo::log.flush();
#endif

            // copie des goals daex dans leur equivant YAHSP
            // nouvelle allocation de tableau de goal
            assert( igoal->size() > 0 );
            _intermediate_goal_state_nb = igoal->size();

            unsigned int i = 0;
            for( daex::Goal::iterator iatom = igoal->begin(); iatom != igoal->end(); ++iatom ) {
                // le compilateur demande à expliciter le template pour fluents, 
                // car le C++ ne prend pas en compte les types de retour dans la signature (beurk).
                _intermediate_goal_state[i] =  (*iatom)->fluent();

                i++;
            }
            assert( i == _intermediate_goal_state_nb );
            // search a plan towards the current goal
            code = solve_next( decompo, _intermediate_goal_state, _intermediate_goal_state_nb  );
            if( code != PLAN_FOUND ) {
	      decompo.fitness( std::make_pair( fitness_unfeasible_intermediate(), false ) );
	      break;
            }

        } // for igoal in decompo

        // here we have reached the last goal of the decomposition
        // we are searching towards the ultimate goal

        if((decompo.size() == 0) || (code == PLAN_FOUND)) {
            // set the b_max specific to this step
            b_max( _b_max_last );
            unsigned int code = solve_next( decompo, goal_state, goal_state_nb  );
            if( code == PLAN_FOUND ) {
	      compress( decompo );
	      decompo.fitness( std::make_pair( fitness_feasible( decompo ), true ) );
#ifndef NDEBUG
                eo::log << eo::debug << "*";
                eo::log.flush();
#endif
	    } else {
	      decompo.fitness( std::make_pair( fitness_unfeasible_final(), false ) );
	    } // if PLAN_FOUND for last goal
        } // if PLAN_FOUND
      } // if size > _l_max
    } // if !decompo.invalid

    free_yahsp_structures();
}


/**************************************************************************************************************
 * FREE YAHSP STRUCTURES
 **************************************************************************************************************/
void daeYahspEval::free_yahsp_structures()
{

#ifndef NDEBUG
    eo::log << eo::xdebug << "\t\tfree plans...";
    eo::log.flush();
#endif
    // On ne libère plus les sous-plans vu qu'on pointe dessus dans la solution
    // libère la variable globale "plans", utilisée par yahsp lors de la compression
    //    for( unsigned int p=0; p < plans_nb; ++p ) {
    //        plan_free( plans[p] );
    //    }
    
    plans_nb = 0;
    
    cpt_free( plans );

#ifndef NDEBUG
    eo::log << eo::xdebug << "ok" << std::endl;
#endif

    // in cpt_search, yahsp free solution_plan if it is allocated. 
    // Thus, we set it to NULL, so has to avoid a free 
    // and keep the pointer within the Decompostion>Plan class
    solution_plan = NULL;
}

/**************************************************************************************************************
 * YAHSP EVAL INIT
 **************************************************************************************************************/


void daeYahspEvalInit::call( daex::Decomposition & decompo ) 
{
#ifndef NDEBUG
    eo::log << eo::logging << std::endl << "init decompo nodes nb: ";
    eo::log.flush();
#endif

    int prev = std::accumulate( node_numbers.begin(), node_numbers.end(), 0 );

    daeYahspEval::call( decompo );
    
    int next = std::accumulate( node_numbers.begin(), node_numbers.end(), 0 );

#ifndef NDEBUG
    eo::log << eo::logging << "     (" << next - prev << ")";
#endif
}


//! Récupère le nombre de noeuds utilisés par une résolution avec yahsp
void daeYahspEvalInit::step_recorder() {
    
    node_numbers.push_back( static_cast<int>( solution_plan->backtracks ) ); // TODO SolutionPlan->backtracks est codé comme un double dans plan.h

#ifndef NDEBUG
    eo::log << eo::logging << " " << node_numbers.back();
    eo::log.flush();
#endif
}

void daeYahspEvalInit::step_recorder_fail() {
      node_numbers.push_back(stats.evaluated_nodes);
#ifndef NDEBUG
    eo::log << eo::logging << " " << node_numbers.back();
    eo::log.flush();
#endif
}


//! Le b_max est calculé comme la médiane du nombre total de noeuds parcourus sur l'ensemble de tous les appels à yahsp 
//! lors d'une première phase d'initialisation
unsigned int daeYahspEvalInit::estimate_b_max( double quantile /* = 0.5 */ )
{
    assert( node_numbers.size() > 0 );

    //unsigned int nth = node_numbers.size() / 2; // division euclidienne, indicage à 0 => prend l'élément supérieur
    unsigned int nth = static_cast<unsigned int>( ceil( static_cast<double>( node_numbers.size() ) * quantile ) );

#ifndef NDEBUG
    if( nth == 0 || nth == node_numbers.size()-1 ) {
        eo::log << eo::warnings << "WARNING: while estimating the b_max, the quantile at " << quantile << " returns the " << nth << "th element (size=" << node_numbers.size() << ")" << std::endl;
    }
#endif

    // JACK use a simple computation of the median (rounding in case of an even size)
    
    // code version:
    //return node_numbers[ nth ];

    // suggested version: use a linear interpolation in case of even size
    
    if( node_numbers.size() % 2 != 0 ) { // impair

        std::nth_element( node_numbers.begin(), node_numbers.begin() + nth,  node_numbers.end() );

        // l'element central est la médiane
        return node_numbers[nth];

    } else { // pair

        unsigned int m1, m2;

        std::nth_element( node_numbers.begin(), node_numbers.begin() + nth,  node_numbers.end() );
        m1 = node_numbers[nth];

        std::nth_element( node_numbers.begin(), node_numbers.begin() + nth - 1,  node_numbers.end() );
        m2 = node_numbers[nth-1];
        
        // mean of center elements
        // with rounding, because the b_max should be an uint
        return static_cast<unsigned int>( ceil( static_cast<double>( m1 + m2 ) / 2.0 ) );
    } 
}


