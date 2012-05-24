#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <sys/time.h>
#include <sys/resource.h>

#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <cfloat>
#include <sstream>

#include <eo>
#include <ga.h>
#include <utils/eoFeasibleRatioStat.h>

#include "daex.h"
#include "evaluation/cpt-yahsp.h"
#include "evaluation/yahsp.h"

int main ( int argc, char* argv[] )
{
    // WALLOCK TIME COUNTER
    time_t time_start = std::time(NULL);

    /**************
     * PARAMETERS *
     **************/

    // EO
    eoParser parser(argc, argv);
    make_verbose(parser);
    make_parallel(parser);
    
    eoState state;
    
    // log some EO parameters
    eo::log << eo::logging << "Parameters:" << std::endl;
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "verbose" << eo::log.getLevelSelected() << std::endl;
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "parallelize-loop" << eo::parallel.isEnabled() << std::endl;
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "parallelize-dynamic" << eo::parallel.isDynamic() << std::endl;
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "parallelize-prefix" << eo::parallel.prefix() << std::endl;
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "parallelize-nthreads" << eo::parallel.nthreads() << std::endl;


    // GENERAL PARAMETERS
    // createParam (ValueType _defaultValue, std::string _longName, std::string _description, char _shortHand=0, std::string _section="", bool _required=false)
    std::string domain = parser.createParam( (std::string)"domain-zeno-time.pddl", "domain", "PDDL domain file", 'D', "Problem", true ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "domain" << domain << std::endl;

    std::string instance = parser.createParam( (std::string)"zeno10.pddl", "instance", "PDDL instance file", 'I', "Problem", true ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "instance" << instance << std::endl;

    std::string plan_file = parser.createParam( (std::string)"plan.soln", "plan-file", "Plan file backup", 'F', "Misc" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "plan-file" << plan_file << std::endl;

    // pop size
    unsigned int pop_size = parser.createParam( (unsigned int)100, "popSize", "Population Size", 'P', "Evolution Engine").value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "pop_size" << pop_size << std::endl;

    // multi-start
    unsigned int maxruns = parser.createParam( (unsigned int)0, "runs-max", 
            "Maximum number of runs, if x==0: unlimited multi-starts, if x>1: will do <x> multi-start", 'r', "Stopping criterions" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "maxruns" << maxruns << std::endl;

    // b_max estimation
    bool insemination = parser.createParam(true, "insemination", "Use the insemination heuristic to estimate b_max at init", '\0', "Initialization").value();

    // seed
    eoValueParam<unsigned int> & param_seed = parser.createParam( (unsigned int)0, "seed", "Random number seed", 'S' );
    // if one want to initialize on current time
    if ( param_seed.value() == 0) {
        // change the parameter itself, that will be dumped in the status file
      //        param_seed.value( time(0) );
      param_seed.value()=time(0); // EO compatibility fixed by CC on 2010.12.24
    }
    unsigned int seed = param_seed.value();
    rng.reseed( seed );
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "seed" << seed << std::endl;

    // Parameters makers
    daex::do_make_eval_param( parser );
    daex::do_make_init_param( parser );
    daex::do_make_variation_param( parser, pop_size);
    daex::do_make_checkpoint_param( parser );
    daex::do_make_replace_param( parser );

    // special case of stopping criteria parameters
    daex::do_make_continue_param( parser )    ;
    // Those parameters are needed during restarts (see below)
    unsigned int mingen = parser.valueOf<unsigned int>("gen-min");
    unsigned int steadygen = parser.valueOf<unsigned int>("gen-steady");
    unsigned int maxgens = parser.valueOf<unsigned int>("gen-max");

    make_help( parser );


    /***********
     * PARSING *
     ***********/

    // PDDL
#ifndef NDEBUG
    eo::log << eo::progress << "Load the instance..." << std::endl;
    eo::log.flush();
#endif
    
    daex::pddlLoad pddl( domain, instance, SOLVER_YAHSP, HEURISTIC_H1, eo::parallel.nthreads(), std::vector<std::string>());
   
#ifndef NDEBUG
    eo::log << eo::progress << "Load the instance...OK" << std::endl;
    eo::log << eo::progress << "Initialization...";
    eo::log.flush();
#endif


    /******************
     * INITIALIZATION *
     ******************/

    daex::Init<daex::Decomposition>& init = daex::do_make_init_op<daex::Decomposition>( parser, state, pddl );

    // randomly generate the population with the init operator
    // eoPop<daex::Decomposition> pop = eoPop<daex::Decomposition>( pop_size, init );
    eoPop<daex::Decomposition> pop = eoPop<daex::Decomposition>( 10, init );

    // used to pass the eval count through the several eoEvalFuncCounter evaluators
    unsigned int eval_count = 0;
    
    TimeVal best_makespan = MAXTIME;

#ifndef SINGLE_EVAL_ITER_DUMP
    std::string dump_sep = ".";
    unsigned int dump_file_count = 1;
    std::string metadata = "domain " + domain + "\n" + IPC_PLAN_COMMENT + "instance " + instance;
#endif

    unsigned int b_max_fixed = parser.valueOf<unsigned int>("bmax-fixed");
    if( b_max_fixed == 0 ) {
        // b_max_fixed = 2048; // 128
        b_max_fixed = 128; 
    }

    unsigned b_max_in = b_max_fixed;
    double b_max_last_weight = parser.valueOf<double>("bmax-last-weight");
    unsigned int b_max_last = static_cast<unsigned int>( std::floor( b_max_in * b_max_last_weight ) );
#ifndef NDEBUG
    eo::log << eo::logging << std::endl << "\tb_max for intermediate goals, b_max_in: "   << b_max_in   << std::endl;
    eo::log << eo::logging              << "\tb_max for        final goal,  b_max_last: " << b_max_last << std::endl;
#endif

    /**************
     * EVALUATION *
     **************/

#ifndef NDEBUG
    eo::log << eo::progress << "Creating evaluators...";
    eo::log.flush();
#endif

    // do_make_eval returns a pair: the evaluator instance 
    // and a pointer on a func counter that may be null of we are in release mode
    std::pair< eoEvalFunc<daex::Decomposition>&, eoEvalFuncCounter<daex::Decomposition>* > eval_pair
        = daex::do_make_eval_op<daex::Decomposition>(
                parser, state, init.l_max(), eval_count, b_max_in, b_max_last, plan_file, best_makespan, dump_sep, dump_file_count, metadata
            );
    eoEvalFunc<daex::Decomposition>& eval = eval_pair.first;

#ifndef NDEBUG
    // in debug mode, we should have a func counter
    assert( eval_pair.second != NULL );
    eoEvalFuncCounter<daex::Decomposition>& eval_counter = * eval_pair.second;

    eo::log << eo::progress << "OK" << std::endl;
    
    eo::log << eo::progress << "Evaluating the first population...";
    eo::log.flush();
#endif

    // a first evaluation of generated pop
    eoPopLoopEval<daex::Decomposition> pop_eval( eval );
    pop_eval( pop, pop );

#ifndef NDEBUG
    eo::log << eo::progress << "OK" << std::endl;
#endif


    /********************
     * EVOLUTION ENGINE *
     ********************/

#ifndef NDEBUG
    eo::log << eo::progress << "Algorithm instanciation...";
    eo::log.flush();
#endif

    // STOPPING CRITERIA
    eoCombinedContinue<daex::Decomposition> continuator = daex::do_make_continue_op<daex::Decomposition>( parser, state );
    
    // Direct access to continuators are needed during restarts (see below)
    eoSteadyFitContinue<daex::Decomposition> & steadyfit 
        = *( dynamic_cast<eoSteadyFitContinue<daex::Decomposition>* >( continuator[0] ) );
    eoGenContinue<daex::Decomposition> & maxgen 
        = *( dynamic_cast< eoGenContinue<daex::Decomposition>* >( continuator[1] ) );


    // CHECKPOINTING 
    eoCheckPoint<daex::Decomposition> & checkpoint = daex::do_make_checkpoint_op( continuator, parser, state, pop
#ifndef NDEBUG
        , eval_counter
#endif
    );

    // SELECTION AND VARIATION
    // daex::MutationDelGoal<daex::Decomposition> delgoal; // FIXME Erreur de segmentation ici : delgoal devrait être un pointeur alloué.
    daex::MutationDelGoal<daex::Decomposition>* delgoal = new daex::MutationDelGoal<daex::Decomposition>;
    eoGeneralBreeder<daex::Decomposition> & breed = daex::do_make_variation_op<daex::Decomposition>( parser, state, pddl, delgoal );

    // REPLACEMENT
    eoReplacement<daex::Decomposition> & replacor = daex::do_make_replace_op<daex::Decomposition>( parser, state );
    unsigned int offsprings = parser.valueOf<unsigned int>("offsprings");

    // ALGORITHM
    eoEasyEA<daex::Decomposition> dae( checkpoint, eval, breed, replacor, offsprings );

#ifndef NDEBUG
    eo::log << eo::progress << "OK" << std::endl;
    //eo::log << eo::progress << "Note: dual fitness is printed as two numbers: a value followed by a boolean (0=unfeasible, 1=feasible)" << std::endl;
    eo::log.flush();
    eo::log << eo::debug << "Legend: \n\t- already valid, no eval\n\tx plan not found\n\t* plan found\n\ta add atom\n\tA add goal\n\td delete atom\n\tD delete goal\n\tC crossover" << std::endl;
#endif


    /********************
     * MULTI-START RUNS *
     ********************/

    // best decomposition of all the runs, in case of multi-start
    // start at the best element of the init
    daex::Decomposition best = pop.best_element();
    daex::Decomposition read;

    /*
    // Serializes in json
    json::Object* jsonBest = best.toJson();

    std::stringstream ss;
    ss << jsonBest;

    std::cout<< "\n\n" << jsonBest << std::endl;
    
    std::cout<< "\n\nParsing..." << std::endl;

    json::Object* received = json::Parser::parse( ss.str() );

    // Deserializes from json
    read.fromJson( received );
    delete jsonBest;

    std::cout<<"\n\nParsing finished, this is what I read :" << std::endl;
    std::cout << received << std::endl;
    */

    // /*
    // Shows decomposition found in cout.
    best.printOn( std::cout );

    std::ofstream ofile("decomposition.dump.eo");
    boost::archive::text_oarchive oa(ofile);
    oa << best;
    ofile.close();

    std::ifstream ifile("decomposition.dump.eo");
    boost::archive::text_iarchive oi(ifile);
    oi >> read;
    ifile.close();

    // Shows decomposition read in cout.
    std::ofstream ofile2("decomposition.new.dump.eo");
    boost::archive::text_oarchive oa2(ofile2);
    oa2 << read;
    ofile2.close();

    read.printOn( std::cout );
    // */
    
    std::cout << "\n\nEnd of main, test is finished.\n" << std::endl;
}

