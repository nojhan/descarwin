
#ifndef _MAKE_CHECXPOINT_DAE_H_
#define _MAKE_CHECKPOINT_DAE_H_

#include <eo>
#include <utils/checkpointing>

// at the moment, in utils/make_help.cpp
// this should become some eoUtils.cpp with corresponding eoUtils.h
bool testDirRes(std::string _dirName, bool _erase);


namespace daex {

void do_make_checkpoint_param( eoParser & parser ) 
{  
#ifndef NDEBUG
    parser.createParam(std::string("results"), "out-dir", "Directory to store DISK outputs", '\0', "Output - Disk");

    // shoudl we empty it if exists
    parser.createParam(true, "out-dir-erase", "erase files in out-dir, if any", '\0', "Output - Disk");

    parser.createParam((unsigned int)0, "out-save-freq", "Save every F generation (0 = only final state, absent = never)", '\0', "Persistence" );
#endif
    
}

template <class EOT>
eoCheckPoint<EOT> & do_make_checkpoint_op( eoContinue<EOT> & continuator, eoParser & parser, eoState & state, eoPop<EOT> & pop
#ifndef NDEBUG
        , eoEvalFuncCounter<EOT> & eval_counter
#endif
)
{
    // attach a continuator to the checkpoint
    // the checkpoint is here to get some stat during the search
    eoCheckPoint<EOT> * checkpoint = new eoCheckPoint<EOT>( continuator );
    state.storeFunctor( checkpoint );

#ifdef SINGLE_EVAL_ITER_DUMP
    BestMakespanStat* stat_makespan = new BestMakespanStat("BestMakespan");
    state.storeFunctor( stat_makespan );
    BestPlanStat* stat_plan = new BestPlan("BestPlan");
    state.storeFunctor( stat_plan );
#endif

#ifndef NDEBUG
    // get the best plan only if it improve the fitness
    // note: fitness is different from the makespan!
    //eoBestPlanImprovedStat<EOT> best_plan( worst_fitness, "Best improved plan");
    // at each generation
    //checkpoint->add( *best_plan );
    
    // display the stats on std::cout
    // ostream & out, bool _verbose=true, std::string _delim = "\t", unsigned int _width=20, char _fill=' ' 
    eoOStreamMonitor* clog_monitor = new eoOStreamMonitor( std::clog, "\t", 10, ' '); 
    state.storeFunctor( clog_monitor );


    // get best fitness
    // for us, has the form "fitness feasibility" (e.g. "722367 1")
    eoBestFitnessStat<EOT>* best_stat = new eoBestFitnessStat<EOT>("Best");
    state.storeFunctor( best_stat );

    // FIXME désactiver la séparation faisable/non-faisable ou la faire différement
    //eoInterquartileRangeStat<EOT> iqr_stat( std::make_pair(0.0,false), "IQR" );
//    eoInterquartileRangeStat<EOT> iqr_f( std::make_pair(0.0,false), "IQR_f" );
//    eoInterquartileRangeStat<EOT> iqr_uf( std::make_pair(0.0,false), "IQR_uf" );
//    eoDualStatSwitch<EOT,eoInterquartileRangeStat<EOT> > dual_iqr( iqr_f, iqr_uf, "\t" );
// 
    // TODO implement "better" nth_element stats with different interpolations (linear and second moment?)
    eoNthElementFitnessStat<EOT>* median_stat = new eoNthElementFitnessStat<EOT>( pop.size() / 2, "Median" ); 
    state.storeFunctor( median_stat );
    /*
    eoNthElementFitnessStat<EOT> median_f( pop.size() / 2, "Median_f" ); 
    eoNthElementFitnessStat<EOT> median_uf( pop.size() / 2, "Median_uf" ); 
    eoDualStatSwitch<EOT,eoNthElementFitnessStat<EOT> > dual_median( median_f, median_uf, "\t/\t" );
    */

    FeasibleRatioStat<EOT>* feasible_stat = new FeasibleRatioStat<EOT>( "F.Ratio" );
    state.storeFunctor( feasible_stat );

    eoAverageSizeStat<EOT>* asize_stat = new eoAverageSizeStat<EOT>( "Av.Size" );
    state.storeFunctor( asize_stat );

    if( eo::log.getLevelSelected() >= eo::progress ) {

        // compute stas at each generation
        checkpoint->add( *best_stat );
        checkpoint->add( *feasible_stat );
        checkpoint->add( *asize_stat );
        checkpoint->add( *median_stat );
        //checkpoint->add( *dual_median );
        //checkpoint->add( *iqr_stat );
//        checkpoint->add( *dual_iqr );
#ifdef SINGLE_EVAL_ITER_DUMP
        checkpoint->add( *stat_makespan );
        checkpoint->add( *stat_plan );
        clog_monitor->add( *stat_makespan );
        clog_monitor->add( *stat_plan );
#else
        clog_monitor->add( eval_counter );
#endif
        clog_monitor->add( *best_stat );
        clog_monitor->add( *asize_stat );
        clog_monitor->add( *feasible_stat );
        clog_monitor->add( *median_stat );
        //clog_monitor->add( *dual_median );
        //clog_monitor->add( *iqr_stat );
//        clog_monitor->add( *dual_iqr );
        
        // the checkpoint should call the monitor at every generation
        checkpoint->add( *clog_monitor );
    }

#ifdef SINGLE_EVAL_ITER_DUMP
    // Note: commented here because superseeded by the eoEvalBestPlanFileDump
    // append the plans in a file
    // std::string _filename, std::string _delim = " ", bool _keep = false, bool _header=false, bool _overwrite = false
    eoFileMonitor* file_monitor = new eoFileMonitor( plan_file, "\n", false, false, true);
    state.storeFunctor(file_monitor );
    file_monitor->add( *stat_plan );
    checkpoint->add( *file_monitor );
#endif
    

    // MODIFS MS START 
    // pour plus d'output (recopiés de do/make_checkpoint)
    // un state, pour sauver l'état courant
    state.registerObject(parser);
    state.registerObject(pop);
    state.registerObject(eo::rng);


    unsigned int out_save_freq = parser.value<unsigned int>("out-save-freq");
        eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "out-save-freq" << out_save_freq << std::endl;

    std::string out_dir = parser.value<std::string>("out-dir");
        eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "out-dir" << out_dir << std::endl;

    bool out_dir_erase = parser.value<bool>("out-dir-erase");
        eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "out-dir-erase" << out_dir_erase << std::endl;

    bool dirOK = testDirRes( out_dir, out_dir_erase ) ;
    if( ! dirOK ) {
        eo::log << eo::warnings << "WARNNG: Cannot write to " << out_dir  << std::endl;
    }

    unsigned freq = out_save_freq>0 ? out_save_freq : std::numeric_limits<unsigned int>::max();
    std::string stmp = out_dir + "/generations";

    eoCountedStateSaver* state_saver = new eoCountedStateSaver(freq, state, stmp);
    state.storeFunctor( state_saver );

    checkpoint->add( *state_saver );
#endif // NDEBUG
    
    return *checkpoint;
}

} // namespace daex

#endif // _MAKE_CHECKPOINT_DAE_H_

