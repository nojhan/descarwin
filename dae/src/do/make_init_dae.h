
#ifndef _MAKE_INIT_DAE_H_
#define _MAKE_INIT_DAE_H_

/** FIXME essai d'évaluation du temps d'un individu pour limiter le temps total d'exécution du programme, dans la version parallèle (Pierre et Benjamin)
# include <sys/resource.h> // rusage()
*/

namespace daex {

void do_make_init_param( eoParser & parser )
{
    double b_max_ratio = parser.createParam( 0.01, "bmax-ratio","Satisfying proportion of feasible individuals for the computation of b_max", 'J', "Evaluation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "b_max_ratio" << b_max_ratio << std::endl;

    unsigned int l_max_init_coef = parser.createParam( (unsigned int)2, "lmax-initcoef", 
            "l_max will be set to the size of the chrono partition * this coefficient", 'C', "Initialization" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "l_max_init_coef" << l_max_init_coef << std::endl;

    unsigned int l_min = parser.createParam( (unsigned int)1, "lmin", 
            "Minimum number of goals in a decomposition", 'L', "Initialization" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "l_min" << l_min << std::endl;

    // Evaluation

    double b_max_increase_coef = parser.createParam( (double)2, "bmax-increase-coef", "Multiplier increment for the computation of b_max", 'K', "Evaluation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "b_max_increase_coef" << b_max_increase_coef << std::endl;

#ifdef DAE_MO
    // the flip-decomposition seems to be the best option, as stated in the article:
    // "Pareto-based Multiobjective AI Planning", IJCAI 2013
    std::string strategy = parser.createParam( (std::string)"flip-decomposition", "strategy",
            "How to change the search strategy for DAEMO_YAHSP. Either a fixed one, for all goals, among: length, cost, makespan-max or makespan-add; Either a different one for each goal: random, flip-goal (random makespan-add or cost at each goal) or flip-decomposition (random makespan-add or cost for the whole individual)",
            'y', "Multi-Objective").value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "strategy" << strategy << std::endl;

    double proba_strategy = parser.createParam( (double)0.5, "proba-strategy",
            "Probability to choose makespan-add metric when using the flip-goal or flip-decomposition strategies", 'p', "Multi-Objective" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "proba_strategy" << proba_strategy << std::endl;
#endif
}


template<class EOT>
daex::Init<EOT> & do_make_init_op( eoParser & parser, eoState & state, daex::pddlLoad & pddl )
{
    unsigned int l_max_init_coef = parser.valueOf<unsigned int>("lmax-initcoef");
    unsigned int l_min = parser.valueOf<unsigned int>("lmin");
#ifdef DAE_MO
    std::string strategy = parser.valueOf<std::string>("strategy");
    double proba_strategy = parser.valueOf<double>("proba-strategy");
    daex::Init<EOT>* init = new daex::Init<EOT>(pddl.chronoPartitionAtom(), strategy, proba_strategy, l_max_init_coef, l_min);
#else
    // l'initialisation se fait en fonction de la liste des dates au plus tot possibles (start time set)
    // Note : dans le init, l_max est réglé au double du nombre de dates dans la partition
    daex::Init<EOT>* init = new daex::Init<EOT>( pddl.chronoPartitionAtom(), l_max_init_coef, l_min );
#endif   
    state.storeFunctor( init );
                           #ifndef NDEBUG
                               eo::log << eo::logging << std::endl;
                               eo::log << eo::logging << "\tChrono partition size: " << pddl.chronoPartitionAtom().size() << std::endl;
                               eo::log << eo::logging << "\tl_max: " << init->l_max() << std::endl;

                               eo::log << eo::debug << "\tChrono partition dates(#atoms): ";
                               for( daex::ChronoPartition::const_iterator it = pddl.chronoPartitionAtom().begin(), end = pddl.chronoPartitionAtom().end(); it != end; ++it ) {
                                   eo::log << eo::debug << it->first << "(" << it->second.size() << ") ";
                               }
                               eo::log << eo::debug << std::endl;
                           #endif
    return *init;
}


template<class EOT>
unsigned int estimate_bmax_insemination( eoParser & parser, daex::pddlLoad & pddl, eoPop<EOT>& pop, unsigned int l_max )
{
    double b_max_ratio = parser.valueOf<double>("bmax-ratio");
    unsigned int fitness_weight = parser.valueOf<unsigned int>("fitness-weight");
    unsigned int fitness_penalty = parser.valueOf<unsigned int>("fitness-penalty");
    unsigned int b_max_init = parser.valueOf<unsigned int>("bmax-init");
    double b_max_increase_coef = parser.valueOf<double>("bmax-increase-coef");

    unsigned int b_max_in=1, b_max_last=1;

    eo::log << eo::logging << "Evaluate a first empty plan" << std::endl; 

    //Let's find a feasible plan with default yashp parameter settings

#ifndef NDEBUG // avoid warning about unused variable
    unsigned int return_code =
#endif
    cpt_basic_search();
    //std::cout << "return_code : " << return_code << std::endl;
    assert( return_code == PLAN_FOUND );
    eo::log << eo::debug << "Found a plan" << std::endl;

    eo::log << eo::logging << "Build Adam from the flat plan" << std::endl; 

    // get the flat plan from yahsp and build a complete decomposition with it
    // we expect intermediate goals/states to be present
    Adam yahsp_adam = yahsp_create_adam( solution_plan );
    //std::cout << "solution_plan->backtrack : " << solution_plan->backtracks << std::endl;
#ifndef NDEBUG
    eo::log << eo::debug << "YAHSP Adam:" << std::endl;
    yahsp_print_adam( yahsp_adam );
#endif
    //Data processing: from Yahsp to dae
    EOT adam;
    for( unsigned int i=0; i < yahsp_adam.states_nb; ++i) {

        typename EOT::AtomType goal;
        TimeVal goal_time = 0;
        for( unsigned int j=0; j < yahsp_adam.states[i].fluents_nb; ++j ) {
            unsigned int common_id = yahsp_adam.states[i].fluents[j]->id;
            daex::Atom atom = pddl.atoms()[common_id];
            goal.push_back( atom );
            goal_time = std::max( goal_time, atom.earliest_start_time() );
        }
        goal.earliest_start_time( goal_time );

        adam.push_back( goal );
    }
    yahsp_free_adam(yahsp_adam);
    eo::log << eo::debug << "Adam:" << std::endl << adam << std::endl;

    //daeYahspEval<EOT> eval_yahsp(l_max, b_max_in, b_max_last, fitness_weight, fitness_penalty );
    daeYahspEval<EOT> eval_yahsp(yahsp_adam.states_nb, solution_plan->backtracks, solution_plan->backtracks, fitness_weight, fitness_penalty );
    eval_yahsp(adam);
    //std::cout << "EVAL ADAM : " << adam.is_feasible() << std::endl;
    assert(adam.is_feasible());

    eo::log << eo::logging << "Create a population of Adam" << std::endl; 
    // Remember the pop size before clearing it
    unsigned int pop_size = pop.size(); 
    pop.clear(); // FIXME si insémination, ne pas faire l'init plus haut

    //Uniform initialization
    for( unsigned int i = 0; i < pop_size; ++i ) {
        pop.push_back( adam );
    }

    eo::log << eo::logging << "Try to del goals and to increase b_max" << std::endl; 
    // global parameters
    unsigned int bmax_iters = 10;


    // FIXME insérer le compteur et le dump du meilleur plan

    // while the pop is largely feasible, try to del goals
    //    double feasibility_ratio = 1.0;

    unsigned int goodguys_feasible = pop_size;
    unsigned int goodguys_min = b_max_ratio * pop_size;
    daex::MutationDelGoal<EOT> delgoal;
    //While too many feasible individuals:
    //	Two steps :
    //		1) perturbation with goal suppression
    //  	2) fixing by increasing bmax
    //std::cout << "BEFOORE : goodguys_feasible : " << goodguys_feasible << std::endl;
    for (unsigned int i = 0; i < pop_size; ++i) {
        eval_yahsp(pop[i]);
        std::cout << pop[i].is_feasible() << pop[i].fitness();
    }
    while (goodguys_feasible > goodguys_min) {

        //Step 1
        unsigned int feasibles = 0;
        for (unsigned int i = 0; i < pop_size; ++i) {
            delgoal(pop[i]);
            eval_yahsp(pop[i]);
            if (pop[i].is_feasible()) {
                feasibles++;
            }
        }
        goodguys_feasible = feasibles;
        //std::cout << "STEP 1 : goodguys_feasible : " << goodguys_feasible << std::endl;

        //Step 2
        unsigned int iters = 0;
        while (goodguys_feasible < goodguys_min && iters <= bmax_iters
                && b_max_in <= b_max_init) {
            unsigned int feasibles = 0;

            for (unsigned int i = 0; i < pop_size; ++i) {
                eval_yahsp(pop[i]);
                if (pop[i].is_feasible()) {
                    feasibles++;
                }
            }

            b_max_in = b_max_in * b_max_increase_coef;
            iters++;
            goodguys_feasible = feasibles;
        }// while feasible & iter & bmax
        //std::cout << "STEP 2 : goodguys_feasible : " << goodguys_feasible << std::flush;
        daeYahspEval<EOT> eval_yahsp(l_max, b_max_in, b_max_last, fitness_weight, fitness_penalty );
    } // while goodguys_feasible > goodguys_min

    eo::log << eo::logging << "After insemination, b_max=" << b_max_in << std::endl;

    return b_max_in;
}


template<class EOT>
unsigned int estimate_bmax_incremental(
        eoPop<EOT>& pop,
        eoParser & parser, unsigned int l_max,
        unsigned int
#ifndef NDEBUG // avoid warning about unused variable
        eval_count
#endif
        ,
        std::string plan_file, TimeVal & best_makespan, 
        std::string dump_sep, unsigned int & dump_file_count, std::string metadata
        )
{
    unsigned int popsize = parser.valueOf<unsigned int>("popSize");
    unsigned int fitness_weight = parser.valueOf<unsigned int>("fitness-weight");
    unsigned int fitness_penalty = parser.valueOf<unsigned int>("fitness-weight");
    double b_max_increase_coef = parser.valueOf<double>("bmax-increase-coef");
    double b_max_ratio = parser.valueOf<double>("bmax-ratio");
    unsigned int b_max_init = parser.valueOf<unsigned int>("bmax-init");
    double b_max_last_weight = parser.valueOf<double>("bmax-last-weight");
    unsigned int b_max_fixed = parser.valueOf<unsigned int>("bmax-fixed");

    bool found = false;
    unsigned int goodguys = 0;
    unsigned int b_max_in=1, b_max_last=0;
    EOT empty_decompo;
    unsigned int goodguys_min = b_max_ratio * popsize;

#ifndef NDEBUG
    eo::log << eo::progress << "Apply an incremental computation strategy to fix bmax with a minimum of " << goodguys_min << " good individuals" << std::endl;
#endif
    /* FIXME essai d'évaluation du temps d'un individu pour limiter le temps total d'exécution du programme, dans la version parallèle (Pierre et Benjamin)
       bool hasTimedEval = false;
       */
    //    while( (((double)goodguys/(double)popsize) < b_max_ratio) && (b_max_in < b_max_init) ) {
    while( (goodguys < goodguys_min) && (b_max_in < b_max_init) ) {

        goodguys=0;
        b_max_last = static_cast<unsigned int>( std::floor( b_max_in * b_max_last_weight ) );

        daeYahspEval<EOT> eval_yahsp( l_max, b_max_in, b_max_last, fitness_weight, fitness_penalty );

        // in non multi-threaded version, use the plan dumper
        //#ifndef SINGLE_EVAL_ITER_DUMP
        daex::evalBestMakespanPlanDump<EOT> eval_bestfile( eval_yahsp, plan_file, best_makespan, false, dump_sep, dump_file_count, metadata );

        // if we do not want MT, but we want debug, add more eval wrappers
#ifndef NDEBUG
        eoEvalFuncCounter<EOT> eval_counter( eval_bestfile, "Eval.\t" );
        eval_counter.value( eval_count );
        eoPopLoopEval<EOT> eval_pop( eval_counter );

        // else, only the plan dumper
#else // ifdef NDEBUG
        eoPopLoopEval<EOT> eval_pop( eval_bestfile );
#endif

        // if we want to compile a multi-threaded version with OpenMP, 
        // we only want the basic evaluator, not the other wrappers, 
        // even the one that dump plans
        //#else // ifdef SINGLE_EVAL_ITER_DUMP
        //                eoPopLoopEval<EOT> eval_pop( eval_yahsp );
        //#endif
        eval_pop( pop, pop );

#ifndef DAE_MO
#ifndef NDEBUG
        eoBestFitnessStat<EOT> best_statTEST("Best");
        best_statTEST(pop);
        //        eo::log << eo::logging << "\tbest_fitness " << timeValToString(best_statTEST.value());
        eo::log << eo::logging << "\tbest_fitness " << best_statTEST.value();
#endif
#endif

        for (size_t i = 0; i < popsize; ++i) {
            // unfeasible individuals are invalidated in order to be re-evaluated 
            // with a larger bmax at the next iteration but we keep the good guys.
            if (pop[i].is_feasible()) 
            {
                goodguys++;
                //FIXME essai d'évaluation du temps d'un individu pour limiter le temps total d'exécution du programme, dans la version parallèle (Pierre et Benjamin)
                /*
                   if( ! hasTimedEval )
                   {
                   hasTimedEval = true;
                   pop[i].invalidate();
                   struct rusage _start;
                   struct rusage _stop;
                   getrusage( RUSAGE_SELF, &_start );
                   eval_bestfile( pop[i] );
                   getrusage( RUSAGE_SELF, &_stop );
                   std::cout << "[Estimating eval time] "
                   << "User time (s / ms): "
                   << _start.ru_utime.tv_sec - _stop.ru_utime.tv_sec
                   << " / "
                   << _start.ru_utime.tv_usec - _stop.ru_utime.tv_usec
                   << "\n"
                   << "System time (s / ms): "
                   << _start.ru_stime.tv_sec - _stop.ru_stime.tv_sec
                   << " / "
                   << _start.ru_stime.tv_usec - _stop.ru_stime.tv_usec
                   << std::endl;
                   }
                   */
            }
            else pop[i].invalidate();
        }
        // If no individual haven't yet been found, then try a direct call to YAHSP (i.e. the empty decomposition evaluation)
        if ((goodguys == 0) && (!found)) {
            empty_decompo.invalidate();
            //#ifdef SINGLE_EVAL_ITER_DUMP
            //                eval_yahsp( empty_decompo );
            //#else
#ifndef NDEBUG
            eval_counter(empty_decompo);
#else
            eval_bestfile(empty_decompo);
#endif
            //#endif
            if (empty_decompo.is_feasible()){
                found = true;
                /*
                   std::ofstream of(plan_file.c_str());
                   of << empty_decompo.plan();
                   of.close();
                   */
            }
        } // if ! goodguys && ! found

        //#ifdef SINGLE_EVAL_ITER_DUMP
        //            best_makespan = pop.best_element().plan_copy().makespan();
        //#else
        best_makespan = eval_bestfile.best();
        dump_file_count = eval_bestfile.file_count();
        //#endif

#ifndef NDEBUG
        eo::log << eo::logging << "\tb_max_in "   << b_max_in << "\tfeasible_ratio " <<  ((double)goodguys/(double)popsize);
        eo::log << "\tbest_makespan " << best_makespan;
        if(found) {
            eo::log << "\tfeasible empty decomposition";
        }
        eo::log << std::endl;
        eval_count = eval_counter.value();
#endif
        b_max_fixed = b_max_in;
        b_max_in = (unsigned int)ceil(b_max_in*b_max_increase_coef);
    } // while


    // evaluate pop before leaving
    daeYahspEval<EOT> eval_yahsp( 
            l_max, b_max_in, b_max_last, fitness_weight, fitness_penalty );
    daex::evalBestMakespanPlanDump<EOT> eval_bestfile( 
            eval_yahsp, plan_file, best_makespan, false, dump_sep, dump_file_count, metadata );
#ifndef NDEBUG
    eoEvalFuncCounter<EOT> eval_counter( eval_bestfile, "Eval.\t" );
    eval_counter.value( eval_count );
    eoPopLoopEval<EOT> eval_pop( eval_counter );
#else
    eoPopLoopEval<EOT> eval_pop( eval_bestfile );
#endif
    eval_pop( pop, pop );

    best_makespan = eval_bestfile.best();
    dump_file_count = eval_bestfile.file_count();

    return b_max_fixed;
}

} // namespace daex

#endif // _MAKE_INIT_DAE_H_

