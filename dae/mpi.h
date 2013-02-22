# ifndef __MPI_DAE_H__
# define __MPI_DAE_H__

#undef error // FIXME il y a un define error très embêtant quelque part là haut...

# include <boost/mpi.hpp>
namespace mpi = boost::mpi;

namespace MpiMessage
{
    const int WRK_NEW_RESULT = 0;
    const int WRK_NO_RESULT = 1;

    const int MSTR_NO_MORE_ATTRIBUTIONS = 2;
    const int MSTR_CONTINUE = 3;
}

namespace MpiChannel
{
    const int COMMANDS = 0;
    const int DATA = 1;
}

struct AssignmentAlgorithm
{
    AssignmentAlgorithm( int runs, mpi::communicator & world ) :
        _runs( runs ),
        _world( world ),
        _confirmedRuns( 0 )
    {
        _infiniteRuns = runs == 0;
    }

    virtual void assign( int wrkRank ) = 0;

    virtual void confirm( int wrkRank )
    {
        ++_confirmedRuns;
    }

    void operator()( int wrkRank )
    {
        confirm( wrkRank );
        assign( wrkRank );
    }
    
    virtual bool finished()
    {
        return !_infiniteRuns && _confirmedRuns == _runs;
    }

protected:
    int _runs;
    int _confirmedRuns;

    bool _infiniteRuns;
    mpi::communicator & _world;
};

struct StaticAssignmentAlgorithm : public AssignmentAlgorithm
{
    StaticAssignmentAlgorithm( int runs, mpi::communicator & world ) : AssignmentAlgorithm( runs, world )
    {
        unsigned int nbWorkers = world.size() - 1;
        attributions = new int[ nbWorkers ];

        if ( runs > 0 )
        {
            // Let be the euclidean division of runs by nbWorkers :
            // runs == q * nbWorkers + r, 0 <= r < nbWorkers
            // This one liner affects q requests to each worker
            for (unsigned int i = 0; i < nbWorkers; attributions[i++] = runs / nbWorkers) ;
            // The first line computes r and the one liner affects the remaining 
            // r requests to workers, in ascending order
            unsigned int diff = runs - (runs / nbWorkers) * nbWorkers;
            for (unsigned int i = 0; i < diff; ++attributions[i++]);
        }

        for (unsigned int i = 0; i < nbWorkers; ++i)
        {
            // rank of node is i+1, as workers' ranks begin at 1.
            eo::log << "[Master] Assignments for process " << i+1 << " : " << attributions[i] << std::endl;
        }
    }

    void confirm( int wrkRank )
    {
        AssignmentAlgorithm::confirm( wrkRank );
        --attributions[ wrkRank - 1 ];
    }

    void assign( int wrkRank )
    {
        int order;

        if ( _infiniteRuns || attributions[ wrkRank - 1 ] > 0 )
        {
            order = MpiMessage::MSTR_CONTINUE;
            eo::log << "[Master] Worker " << wrkRank << " has still " << attributions[ wrkRank - 1 ] << " runs to launch." << std::endl;
        } else
        {
            order = MpiMessage::MSTR_NO_MORE_ATTRIBUTIONS;
            eo::log << "[Master] No more attributions for worker " << wrkRank << std::endl;
        }

        _world.send( wrkRank, MpiChannel::COMMANDS, order );
    }

    ~StaticAssignmentAlgorithm()
    {
        delete attributions;
    }

private:
    int* attributions;
};

struct DynamicAssignmentAlgorithm : public AssignmentAlgorithm
{
    DynamicAssignmentAlgorithm( int runs, mpi::communicator & world ) :
        AssignmentAlgorithm( runs, world ),
        _assignedRuns( 0 )
    {
        // empty
    }

    void assign( int wrkRank )
    {
        int order;
        if ( _infiniteRuns || _assignedRuns < _runs )
        {
            order = MpiMessage::MSTR_CONTINUE;
            ++_assignedRuns;
            eo::log << "[DynamicAssignmentAlgorithm] Run assigned to " << wrkRank << std::endl;
        } else
        {
            order = MpiMessage::MSTR_NO_MORE_ATTRIBUTIONS;
        }
        _world.send( wrkRank, MpiChannel::COMMANDS, order );
    }

private:
    int _assignedRuns;
};

class MpiNode
{
protected:
    mpi::environment& env;
    mpi::communicator& world;

    int _rank;
    int _size;

    int _argc;
    char** _argv;

public:
    virtual int run() = 0;

    MpiNode(int argc, char** argv, mpi::environment& a_env, mpi::communicator& a_world) :
        env(a_env),
        world(a_world),
        _rank(world.rank()),
        _size(world.size()),
        _argc(argc),
        _argv(argv)
    {
        // empty
    }

    virtual ~MpiNode()
    {
        // empty
    }
};

class DaeNode : public MpiNode
{
public:    
    
    DaeNode(int argc, char** argv, mpi::environment& a_env, mpi::communicator& a_world) :
        MpiNode(argc, argv, a_env, a_world)
    {
        // empty
    }

    virtual int run()
    {
        /**************
         * PARAMETERS *
         **************/

        // EO
        _parser = new eoParser(_argc, _argv);
        eoParser& parser = *_parser;

        make_verbose(parser);
        make_parallel(parser);
        
        _plan_file = parser.createParam( (std::string)"plan.soln", "plan-file", "Plan file backup", 'F', "Misc" ).value();

        // Redefining file name by adding rank number in front of it.
        std::stringstream ss;
        ss << _plan_file << '.' << _rank;
        _plan_file = ss.str();

        return 0;
    }

    virtual ~DaeNode()
    {
        delete _parser;
    }

protected:
    eoParser* _parser;
    eoState _state;

    std::string _plan_file;
};

class MasterNode : public DaeNode
{
public:
    
    MasterNode(int argc, char** argv, mpi::environment& a_env, mpi::communicator& a_world) :
        DaeNode(argc, argv, a_env, a_world)
    {
        // empty
    }

    virtual int run()
    {
        DaeNode::run();

        // Retrieving parent variables
        eoParser& parser = *_parser;
        int size = _size;

        if ( world.size() < 2 )
        {
            eo::log << "[Master] Error : no worker found. Aborting." << std::endl;
            return 1;
        }

        // multi-start
        unsigned int maxruns = parser.createParam( (unsigned int)0, "runs-max", 
                "Maximum number of runs, if x==0: unlimited multi-starts, if x>1: will do <x> multi-start", 'r', "Stopping criterions" ).value();

        daex::Decomposition best;
        best.fitness( 200000. ); // TODO put a dirty high value

        AssignmentAlgorithm* assignmentAlgorithm =
            new StaticAssignmentAlgorithm( maxruns, world );

        for( int i = 1; i < world.size(); ++i )
        {
            assignmentAlgorithm->assign( i );
        }

        while( true )
        {
            mpi::status status = world.probe(mpi::any_source, MpiChannel::COMMANDS);
            int wrkRank = status.source();

            eo::log << "Showing status :\nError : " << status.error()
                      << "\nTag : " << status.tag()
                      <<"\nSource : " << status.source()
                      << std::endl
                      ;

            int answer = -1;
            world.recv( wrkRank, MpiChannel::COMMANDS, answer );
            // 0 == nothing better
            // 1 == better fitness found, I send it to you.
            eo::log << "[Master] Node " << wrkRank << " tells : " << answer << std::endl;

            if ( answer == MpiMessage::WRK_NO_RESULT)
            {
                eo::log << "[Master] Nothing better.\n" << std::endl ;
            } else if ( answer == MpiMessage::WRK_NEW_RESULT)
            {
                eo::log << "[Master] Brace yourself, solution is coming...\n" << std::endl ;
                daex::Decomposition received;
                world.recv( wrkRank, MpiChannel::DATA, received );
                if (received.fitness() > best.fitness())
                {
                    best = received;
                    double fitnessValue = best.fitness();
                    eo::log << "[Master] Better solution found ! Now best fitness is " << fitnessValue << std::endl ;
                } else {
                    eo::log << "[Master] Solution sent from " << wrkRank << " was not as good as the one I had." << std::endl ;
                }
            } else
            {
                eo::log << "[Master] Unknown answer received : " << answer << std::endl;
            }

            (*assignmentAlgorithm)( wrkRank );
            if ( assignmentAlgorithm->finished() )
            {
                delete assignmentAlgorithm;
                break;
            }
        }
        eo::log << "[Master] Global process is over." << std::endl ;

        // TODO should write best solution here

        return 0;
    }
};

class WorkerNode : public DaeNode
{
public:
    WorkerNode( int argc, char** argv, mpi::environment &env, mpi::communicator &world ) : 
        DaeNode( argc, argv, env, world )
    {
        // empty
    }

    virtual int run()
    {
        DaeNode::run();

        // Retrieving parent variables
        eoParser& parser = *_parser;
        eoState& state = _state;
        std::string& plan_file = _plan_file;
        int rank = _rank;
        
        int order = 0;
        world.recv( 0, MpiChannel::COMMANDS, order );
        if ( order == MpiMessage::MSTR_NO_MORE_ATTRIBUTIONS )
        {
            eo::log << "[Worker " << rank << "] Nothing to do here, quits !" << std::endl ;
            return 0;
        }

        // TODO param runs-max is not used by workers. But if param is not created, parser complains about receiving an
        // unknown parameter.
        parser.createParam( (unsigned int)0, "runs-max", "Maximum number of runs, if x==0: unlimited multi-starts, if x>1: will do <x> multi-start", 'r', "Stopping criterions" );

        std::string domain = parser.createParam( (std::string)"domain-zeno-time.pddl", "domain", "PDDL domain file", 'D', "Problem", true ).value();
        std::string instance = parser.createParam( (std::string)"zeno10.pddl", "instance", "PDDL instance file", 'I', "Problem", true ).value();
        // pop size
        unsigned int pop_size = parser.createParam( (unsigned int)100, "popSize", "Population Size", 'P', "Evolution Engine").value();
        
        // b_max estimation
        bool insemination = parser.createParam(true, "insemination", "Use the insemination heuristic to estimate b_max at init", '\0', "Initialization").value();

        // seed
        eoValueParam<unsigned int> & param_seed = parser.createParam( (unsigned int)0, "seed", "Random number seed", 'S' );
        // if one want to initialize on current time
        if ( param_seed.value() == 0) {
          // change the parameter itself, that will be dumped in the status file
          //        param_seed.value( time(0) );
          param_seed.value() = time(0); // EO compatibility fixed by CC on 2010.12.24
        }
        param_seed.value() *= rank; // to avoid having the same seed for each process
        // no multiplication by zero as workers processes are strictly higher than 0
        eo::log << "[Worker " << rank << "] Seed : " << param_seed.value() << std::endl ;

        unsigned int seed = param_seed.value();
        rng.reseed( seed );

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
        eo::log << eo::progress << "[Worker " << rank << "] Load the instance..." << std::endl;
        eo::log.flush();
#endif
        
        daex::pddlLoad pddl( domain, instance, SOLVER_YAHSP, HEURISTIC_H1, eo::parallel.nthreads(), std::vector<std::string>());
       
#ifndef NDEBUG
        eo::log << eo::progress << "[Worker " << rank << "] Load the instance...OK" << std::endl;
        eo::log << eo::progress << "[Worker " << rank << "] Initialization..." << std::endl;
        eo::log.flush();
#endif

        /******************
         * INITIALIZATION *
         ******************/

        daex::Init<daex::Decomposition>& init = daex::do_make_init_op<daex::Decomposition>( parser, state, pddl );

        // randomly generate the population with the init operator
        eoPop<daex::Decomposition> pop = eoPop<daex::Decomposition>( pop_size, init );

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
            // Heuristics for the estimation of an optimal b_max
            if( insemination ) {
                b_max_fixed = daex::estimate_bmax_insemination( parser, pddl, pop, init.l_max() );

            } else { 
                // if not insemination, incremental search strategy
                b_max_fixed  = daex::estimate_bmax_incremental<daex::Decomposition>( 
                    pop, parser, init.l_max(), eval_count, plan_file, best_makespan, dump_sep, dump_file_count, metadata 
                ); // FIXME bug here when retrieving bmax-last-weight
            }
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
        eo::log << eo::progress << "Creating evaluators..." << std::endl;
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
        
        eo::log << eo::progress << "Evaluating the first population..." << std::endl;
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
        eo::log << eo::progress << "Algorithm instanciation..." << std::endl;
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
        
        // best decomposition of all the runs, in case of multi-start
        // start at the best element of the init
        daex::Decomposition best = pop.best_element();
        int run = 0;

        // evaluate an empty decomposition, for comparison with decomposed solutions
        daex::Decomposition empty_decompo;
        eval( empty_decompo );

        try {
            while( true ) {
#ifndef NDEBUG
                eo::log << eo::progress << "[Worker " << rank << "] Start the " << run << "th run..."  << std::endl;

                // call the checkpoint (log and stats output) on the pop from the init
                checkpoint( pop );
#endif
                // start a search
                dae( pop );

                eo::log << "[Worker " << rank << "] After dae search..." << std::endl ;

                // remember the best of all runs
                daex::Decomposition best_of_run = pop.best_element();

                // note: operator> is overloaded in EO, don't be afraid: we are minimizing
                if( best_of_run.fitness() > best.fitness() ) { 
                   best = best_of_run;
                   eo::log << "[Worker " << rank << "] Telling master I've found a solution with fitness equals to " << best.fitness() << std::endl ;

                   world.send( 0, MpiChannel::COMMANDS, MpiMessage::WRK_NEW_RESULT );
                   world.send( 0, MpiChannel::DATA, best );
                } else {
                    eo::log << "[Worker " << rank << "] Didn't find something better..." << std::endl ;
                    world.send( 0, MpiChannel::COMMANDS, MpiMessage::WRK_NO_RESULT);
                }

                // TODO handle the case when we have several best decomposition with the same fitness but different plans?
                // TODO use previous searches to re-estimate a better b_max?

                // the loop test is here, because if we've reached the number of runs, we do not want to redraw a new pop

                run++;
                int order = 0;
                world.recv( 0, MpiChannel::COMMANDS, order );
                if ( order == MpiMessage::MSTR_NO_MORE_ATTRIBUTIONS )
                {
                    break;
                } else
                {
                    eo::log << "[Worker " << rank << "] My work is not finished." << std::endl;
                }

                // Once the bmax is known, there is no need to re-estimate it,
                // thus we re-init ater the first search, because the pop has already been created before,
                // when we were trying to estimate the b_max.
                pop = eoPop<daex::Decomposition>( pop_size, init );
                
                // evaluate
                //eoPopLoopEval<daex::Decomposition> pop_eval( eval ); // FIXME useful ??
                pop_eval( pop, pop );

                // reset run's continuator counters
                steadyfit.totalGenerations( mingen, steadygen );
                maxgen.totalGenerations( maxgens );
            }
            eo::log << "[Worker " << rank << "] I'm free ! (finished)" << std::endl ;
        } catch( std::exception& e ) {
#ifndef NDEBUG
            eo::log << eo::warnings << "STOP: " << e.what() << std::endl;
            eo::log << eo::progress << "... premature end of search, current result:" << std::endl;
#endif

            // push the best result, in case it was not in the last run
            pop.push_back( best );

#ifndef NDEBUG
            // call the checkpoint, as if it was ending a generation
            checkpoint( pop );
#endif

            // Added an evaluated decomposition, in case it would be better than a decomposed one
            pop.push_back( empty_decompo );
            pop_eval( pop, pop ); // FIXME normalement inutile
            return 0;
        }

#ifndef NDEBUG
        eo::log << eo::progress << "... end of search" << std::endl;
#endif

        pop.push_back( empty_decompo );
        // push the best result, in case it was not in the last run
        pop.push_back( best );
        pop_eval( pop, pop ); // FIXME normalement inutile
        return 0;
    }
};

class MpiAlgorithm
{

private:
    mpi::environment* env;
    mpi::communicator* world;
    MpiNode* node;

public:
    MpiAlgorithm(int argc, char* argv[])
    {
        env = new mpi::environment( argc, argv );
        world = new mpi::communicator;

        if ( world->rank() == 0 )
        {
            node = new MasterNode( argc, argv, *env, *world );
        } else
        {
            node = new WorkerNode( argc, argv, *env, *world );
        }
    }


    int run()
    {
        return node->run();
    }

    ~MpiAlgorithm()
    {
        delete node;
        delete world;
        delete env;
    }
};



# endif // __MPI_DAE_H__

