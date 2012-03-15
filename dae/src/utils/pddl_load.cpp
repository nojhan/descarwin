

#include "pddl_load.h"
#include <utils/eoLogger.h>
#include <utils/eoParallel.h>

 
namespace daex
{

void pddlLoad::load_pddl( std::string solver /*= SOLVER_YAHSP*/, 
                          unsigned int heuristic_start_times /*= HEURISTIC_H1*/,
                          unsigned int nthreads /* = 1 */,
                          std::vector<std::string> solver_args /* = std::vector<std::string>()*/
        )
{
    assert( solver == SOLVER_YAHSP || solver == SOLVER_CPT );
    assert( heuristic_start_times == HEURISTIC_H1 || heuristic_start_times == HEURISTIC_H2 );

    // yahsp + seq + heuristique 1 + nodebug
    // vecteur contenant l'ensemble des options à passer à CPT
    std::vector<std::string> cpt_call;
    cpt_call.push_back( "cpt" );

    // TODO use the -v verbosity option ?

    // Divide-and-Evolve approach ( dae / NULL / E )
    cpt_call.push_back( "-dae" );

    // number of threads
    std::stringstream nbthreads;
    nbthreads << nthreads;
    cpt_call.push_back( nbthreads.str().c_str() );

    // Domain file (typed/untyped PDDL) ( ops / NULL / o )
    cpt_call.push_back( "-ops" );
        cpt_call.push_back( (char *) _domain.c_str() );

    // Problem file (typed/untyped PDDL) ( facts / NULL / f )
    cpt_call.push_back( "-facts" ); // instance
        cpt_call.push_back( (char *) _problem.c_str() );


    // options spécifiques aux solveurs
    if( solver == SOLVER_YAHSP ) {

        // use suboptimal yahsp search (yahsp / yahsp / y)
        cpt_call.push_back( "-yahsp" );

        // -yashp now has one argument (for multicore)
        cpt_call.push_back( "64" );

        // précision pour la planif temporelle. Pour IPC : 0.01 (cf. mail Carlos)
        // à vérifier.
        cpt_call.push_back( "-K" ); 
        cpt_call.push_back( "0.01,0.01" ); 
	
        // Enables h2-based fluent mutexes in yahsp (fluent-mutexes / fm / F)
        cpt_call.push_back( "-fluent-mutexes" );

        // Maximum number of backtracks <x> allowed.
        // TODO vérifier que yahsp ne différencie pas -maxbb et -maxb
        // il utilise indifférement la valeur passé à l'un ou l'autre
        // ( max-backtracks / maxb / X )
        // ( max-backtracks-bound / maxbb / x )
        cpt_call.push_back( "-max-backtracks" ); // nb maximal de noeuds (search steps)

        // normalement, le nb de noeuds n'est pas utilisé lors du chargement, 
        // mais uniquement lors de l'eval, cependant, comme il faut l'initialiser
        // on le met à zéro
        // la valeur est changée par la suite, par les opérateurs d'évaluation
        cpt_call.push_back( "0" );

        // Heuristic for computing a lower bound of the makespan. 
        // Possible values are :
        //   0 (no heuristic, default), 
        //   1 (h1 heuristic), 
        //   2 (h2 heuristic, similar to temporal planning graph).
        //   ( initial-heuristic / ih / i )
        cpt_call.push_back( "-initial-heuristic" );

        if( heuristic_start_times == HEURISTIC_H1 ) {
            cpt_call.push_back( "1" );

        } else if( heuristic_start_times == HEURISTIC_H2 ) {
            cpt_call.push_back( "2" );

        } else {
            cpt_call.push_back( "0" );
        }

    } else if( solver == SOLVER_CPT ) {
        // Heuristic for computing a lower bound of the makespan. 
        // Possible values are :
        //   0 (no heuristic, default), 
        //   1 (h1 heuristic), 
        //   2 (h2 heuristic, similar to temporal planning graph).
        cpt_call.push_back( "-initial-heuristic" );

        if( heuristic_start_times == HEURISTIC_H1 ) {
            cpt_call.push_back( "1" );

        } else if( heuristic_start_times == HEURISTIC_H2 ) {
            cpt_call.push_back( "2" );

        } else {
            cpt_call.push_back( "0" );
        }

        // Gives suboptimal solution when dichotomic search is used 
        // when limiting the number of backtracks. 
        // ( give-suboptimal / gs / u )
        cpt_call.push_back( "-give-suboptimal" );

        // nb maximal de backtracks
        cpt_call.push_back( "-max-backtracks" );
            cpt_call.push_back( "1000000" ); 
            // TODO valider qu'on ne veut pas limiter les backtracks à chaque borne (-maxbb)
            
    } // if solver == 


    // append the supplementary args
    cpt_call.insert( cpt_call.end(), solver_args.begin(), solver_args.end() );

    // prépare un tableau de char* de la bonne taille, à passer à CPT
    const char** params = (const char**) calloc( cpt_call.size(), sizeof(const char*) );
    if( params == NULL ) {
        throw( std::runtime_error( "Parameter array allocation error" ) );
    }

    for( unsigned int i = 0; i < cpt_call.size(); ++i ) {
        params[i] = (const char*) cpt_call[i].c_str();
    }
   
    /* Initialisation générale de CPT :
     *   initialisation des options passées avec params
     *   initialisation des heuristiques
     *   parsing du pddl
     *   initialisation de yahsp
     */
#ifndef NDEBUG
    eo::log << eo::debug << "\tcpt_main options: ";
    std::copy( cpt_call.begin(), cpt_call.end(), std::ostream_iterator<std::string>( eo::log << eo::debug, " " ) );
    eo::log << eo::debug << std::endl;
#endif

    cpt_main( cpt_call.size(), params );


    // TODO restreindre les prédicats à ceux qui sont utilisés


    // fluents et fluents_nb sont des variables globales initialisées par CPT à partir de pddl_domain, 
    // puis modifiées pour ne garder que les fluents atteignables

    // pour chaque Atom
    for( unsigned int i=0; i < (unsigned int) fluents_nb; ++i) {

        // prédicat, objet, date au plus tot, pointeur vers le fluent CPT
        daex::Atom * atom = new daex::Atom( fluents[i]->init, fluents[i] );

        //std::cout << *atom << std::endl;

        _atoms.push_back( atom );

    } // for i atom
}


void pddlLoad::compute_chrono_partition()
{
    assert( _atoms.size() != 0 );

    std::vector<daex::Atom*>::iterator it_atom = _atoms.begin();

    // pour tous les atomes existants
    while( it_atom != _atoms.end() ) {

        TimeVal start_time = (*it_atom)->earliest_start_time();

        // associer l'atome à sa date au plus tot
        _chrono_partition_atom[ start_time ].push_back( *it_atom );
        
        it_atom++;
    }

    // les atomes dans l'init ont leurs dates au plus tot à 0
    // il existe donc au moins cette date dans la partition
    assert( _chrono_partition_atom.count( 0 ) != 0 );

    // FIXME supprimer la date 0 et vérifier que ça ne pète rien

    _chrono_partition_atom.compute_goal_max_date();
}


pddlLoad::pddlLoad( 
          std::string domain, 
          std::string problem, 
          std::string solver /*= SOLVER_YAHSP*/, 
          unsigned int heuristic_start_times /*= HEURISTIC_H1*/,
          unsigned int nthreads /*= 1*/,
          std::vector<std::string> solver_args /* = std::vector<std::string>()*/
        ) : _domain(domain), _problem(problem)
{
    load_pddl( solver, heuristic_start_times, nthreads, solver_args);
    compute_chrono_partition();
}

pddlLoad::~pddlLoad()
{
    for( unsigned int i=0; i<_atoms.size(); ++i) {
        delete _atoms[i];
    }
}

} // namespace daex

