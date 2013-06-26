
#ifndef _MAKE_CHECXPOINT_DAE_H_
#define _MAKE_CHECKPOINT_DAE_H_

#include <limits>

#include <eo>

#ifdef DAE_MO
#include <moeo>
#endif

#include <utils/checkpointing>

// at the moment, in utils/make_help.cpp
// this should become some eoUtils.cpp with corresponding eoUtils.h
bool testDirRes(std::string _dirName, bool _erase);


namespace daex {

#define DAEX_FEASIBILITY_SUFFIX "_f"
#define DAEX_UNFEASIBILITY_SUFFIX "_u"
#define DAEX_FEASIBILITY_SEP " / "
#define DAEX_LOG_SEP ", "

void do_make_checkpoint_param( eoParser &
#ifndef NDEBUG // avoid warning about unused variable
        parser
#endif
)
{
#ifndef NDEBUG
    unsigned int out_save_freq = parser.createParam((unsigned int)0, "out-save-freq", "Save every F generation (0 = only final state, absent = never)", '\0', "Persistence" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "out-save-freq" << out_save_freq << std::endl;

    // shoudl we empty it if exists
    std::string out_dir = parser.createParam(std::string("results"), "out-dir", "Directory to store DISK outputs", '\0', "Output").value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "out-dir" << out_dir << std::endl;

    bool out_dir_erase = parser.createParam(true, "out-dir-erase", "erase files in out-dir, if any", '\0', "Output").value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "out-dir-erase" << out_dir_erase << std::endl;
#endif
}

//! Wrap given statistic computation operators into a dual switch
template<class EOSTAT>
eoDualStatSwitch<EOSTAT>& make_dual_stat( std::string name, eoState& state )
{
    EOSTAT* stat_feasible = new EOSTAT(name+DAEX_FEASIBILITY_SUFFIX);
    state.storeFunctor( stat_feasible );

    EOSTAT* stat_unfeasible = new EOSTAT(name+DAEX_UNFEASIBILITY_SUFFIX);
    state.storeFunctor( stat_unfeasible );

    eoDualStatSwitch<EOSTAT>* dual_stat
        = new eoDualStatSwitch<EOSTAT>( *stat_feasible, *stat_unfeasible, DAEX_FEASIBILITY_SEP);
    state.storeFunctor( dual_stat );

    return *dual_stat;
}

template<class EOSTAT, class T>
eoDualStatSwitch<EOSTAT>& make_dual_stat_param( T& param, std::string name, eoState& state )
{
    EOSTAT* stat_feasible = new EOSTAT( param, name+DAEX_FEASIBILITY_SUFFIX);
    state.storeFunctor( stat_feasible );

    EOSTAT* stat_unfeasible = new EOSTAT( param, name+DAEX_UNFEASIBILITY_SUFFIX);
    state.storeFunctor( stat_unfeasible );

    eoDualStatSwitch<EOSTAT>* dual_stat
        = new eoDualStatSwitch<EOSTAT>( *stat_feasible, *stat_unfeasible, DAEX_FEASIBILITY_SEP);
    state.storeFunctor( dual_stat );

    return *dual_stat;
}


#ifdef DAE_MO
//! Add multi-objective (fitness-dependent) checkpoints to the checkpoint
template<class EOT>
void add_stats_multi( eoCheckPoint<EOT>& checkpoint, eoOStreamMonitor& clog_monitor, eoState & state, eoPop<EOT>& pop, moeoArchive<EOT>& archive, eoParser & parser )
{
    typedef typename EOT::ObjectiveVector OVT;

    // NOTE: this is MANDATORY to update the pareto archive
    moeoArchiveUpdater<EOT> * arch_updater = new moeoArchiveUpdater<EOT>( archive, pop);
    state.storeFunctor( arch_updater );
    checkpoint.add( *arch_updater );

    if( eo::log.getLevelSelected() >= eo::progress ) {
        // instanciate a metric comparing two Pareto front (hence a "binary" one)
        // use OVT, the type of the objective vectors

        // get best fitness
        // for us, has the form "fitness feasibility" (e.g. "722367 1")
        eoStat<EOT,std::string>& best_fit_stat = make_dual_stat< eoBestFitnessStat<EOT> >( "BestFit", state );

        // TODO implement "better" nth_element stats with different interpolations (linear and second moment?)
        // we should use a ratio here, and not a position,
        // because the size of a [un]feasible pop is unknown at runtime
        double median_ratio = 0.5;
        eoStat<EOT,std::string>& median_stat = make_dual_stat_param< eoNthElementStat<EOT> >( median_ratio, "MedFit", state );

        checkpoint.add( best_fit_stat );
        checkpoint.add( median_stat );
        clog_monitor.add( best_fit_stat );
        clog_monitor.add( median_stat );

        // typename OVT::Type m( 0.0, true);
        // typename OVT::Type c( 0.0, true);
        typename OVT::Type m( std::numeric_limits<int>::max(), false);
        typename OVT::Type c( std::numeric_limits<int>::max(), false);
        OVT ref(m,c);
        moeoDualHyperVolumeDifferenceMetric<OVT> * m_hypervolume = new moeoDualHyperVolumeDifferenceMetric<OVT>(true,ref);
        // moeoDualHyperVolumeDifferenceMetric<OVT> * m_hypervolume = new moeoDualHyperVolumeDifferenceMetric<OVT>(true,1.1);
        eoStat<EOT,std::string>& hypervolume = make_dual_stat_param< moeoBinaryMetricStat<EOT> >( *m_hypervolume, "HypVol", state );
        checkpoint.add( hypervolume );
        clog_monitor.add( hypervolume );

        // FIXME the epsilon stat cannot handle dual fitness, it must be overloaded
        // moeoVecVsVecAdditiveEpsilonBinaryMetric<OVT> * m_epsilon = new moeoVecVsVecAdditiveEpsilonBinaryMetric<OVT>;
        // eoStat<EOT,std::string>& epsilon = make_dual_stat_param< moeoBinaryMetricStat<EOT> >( *m_epsilon, "Epsi", state );
        // checkpoint.add( epsilon );
        // clog_monitor.add( epsilon );

        eoStat<EOT,std::string>& best_stat = make_dual_stat< moeoBestObjVecStat<EOT> >("Best", state );
        checkpoint.add( best_stat );
        clog_monitor.add( best_stat );

        eoStat<EOT,std::string>& average_stat = make_dual_stat< moeoAverageObjVecStat<EOT> >("Avrg", state );
        checkpoint.add( average_stat );
        clog_monitor.add( average_stat );

        moeoContributionMetric<OVT> * m_contribution = new moeoContributionMetric<OVT>;
        // state.storeFunctor( m_contribution ); // can't store this due to ambiguous base with a different template type // FIXME use smart pointers
        // wrap it in an eoStat
        eoStat<EOT,std::string>& contribution = make_dual_stat_param< moeoBinaryMetricStat<EOT> >( *m_contribution, "Cntrb", state );
        // add it to the checkpoint
        checkpoint.add( contribution );
        clog_monitor.add( contribution );

        // <JD> the moeo entropy metric segfaults for small distances, don't know why
        // moeoEntropyMetric<OVT> * m_entropy = new moeoEntropyMetric<OVT>;
        // // state.storeFunctor( m_entropy );
        // moeoBinaryMetricStat<EOT>* entropy = new moeoBinaryMetricStat<EOT>( *m_entropy, "Entropy" );
        // state.storeFunctor( entropy );
        // checkpoint.add( *entropy );
        // clog_monitor.add( *entropy );


        std::string out_dir = parser.valueOf<std::string>("out-dir");
#ifdef _MSVC
        std::string stmp = out_dir + "\arch";
#else
        std::string stmp = out_dir + "/arch"; // FIXME get the directory from the parser
#endif
        moeoArchiveObjectiveVectorSavingUpdater<EOT> * save_updater = new moeoArchiveObjectiveVectorSavingUpdater<EOT>(archive, stmp, /*count*/true);
        state.storeFunctor(save_updater);
        checkpoint.add(*save_updater);
    }

}


#else // DAE_MO


//! Add mono-objective (fitness-dependent) checkpoints to the checkpoint
template<class EOT>
void add_stats_mono( eoCheckPoint<EOT>& checkpoint, eoOStreamMonitor& clog_monitor, eoState & state )
{
#ifdef SINGLE_EVAL_ITER_DUMP
    BestMakespanStat* stat_makespan = new BestMakespanStat("BestMakespan");
    state.storeFunctor( stat_makespan );
    BestPlanStat* stat_plan = new BestPlanStat("BestPlan");
    state.storeFunctor( stat_plan );
#endif

#ifndef NDEBUG
    if( eo::log.getLevelSelected() >= eo::progress ) {
        // get best fitness
        // for us, has the form "fitness feasibility" (e.g. "722367 1")
        eoStat<EOT,std::string>& best_stat = make_dual_stat< eoBestFitnessStat<EOT> >( "Best", state );

        // TODO implement "better" nth_element stats with different interpolations (linear and second moment?)
        // we should use a ratio here, and not a position,
        // because the size of a [un]feasible pop is unknown at runtime
        double median_ratio = 0.5;
        eoStat<EOT,std::string>& median_stat = make_dual_stat_param< eoNthElementStat<EOT> >( median_ratio, "Median", state );

        checkpoint.add( best_stat );
        checkpoint.add( median_stat );
        clog_monitor.add( best_stat );
        clog_monitor.add( median_stat );
#ifdef SINGLE_EVAL_ITER_DUMP
        checkpoint.add( *stat_makespan );
        clog_monitor.add( *stat_makespan );
        checkpoint.add( *stat_plan );
        clog_monitor.add( *stat_plan );
#endif
    }

#ifdef SINGLE_EVAL_ITER_DUMP
    // Note: commented here because superseeded by the eoEvalBestPlanFileDump
    // append the plans in a file
    // std::string _filename, std::string _delim = " ", bool _keep = false, bool _header=false, bool _overwrite = false
    eoFileMonitor* file_monitor = new eoFileMonitor( plan_file, "\n", false, false, true);
    state.storeFunctor(file_monitor );
    file_monitor.add( *stat_plan );
    checkpoint.add( *file_monitor );
#endif

#endif // NDEBUG
}

#endif // DAE_MO



template <class EOT>
eoCheckPoint<EOT> & do_make_checkpoint_op( eoContinue<EOT> & continuator,
        eoParser &
#ifndef NDEBUG // avoid warning about unused variable
        parser
#endif
        ,
        eoState & state, eoPop<EOT> &
#ifndef NDEBUG
        pop
        , eoEvalFuncCounter<EOT> & eval_counter
#endif
#ifdef DAE_MO
        , moeoArchive<EOT>& archive
#endif
)
{
    // attach a continuator to the checkpoint
    // the checkpoint is here to get some stat during the search
    eoCheckPoint<EOT> * checkpoint = new eoCheckPoint<EOT>( continuator );
    state.storeFunctor( checkpoint );

#ifndef NDEBUG
    eoOStreamMonitor* clog_monitor = new eoOStreamMonitor( std::clog, DAEX_LOG_SEP, 1, ' ', true, " = ");
    state.storeFunctor( clog_monitor );

    eoGenCounter* gen_count = new eoGenCounter( 0, "Gen" );
    state.storeFunctor( gen_count );

    eoTimeCounter* time_count = new eoTimeCounter();
    state.storeFunctor( time_count );

    FeasibleRatioStat<EOT>* feasible_stat = new FeasibleRatioStat<EOT>( "Fsbl%" );
    state.storeFunctor( feasible_stat );

    eoStat<EOT,std::string>& asize_stat = make_dual_stat< eoAverageSizeStat<EOT> >( "AvSz", state );

    if( eo::log.getLevelSelected() >= eo::progress ) {

        // compute stas at each generation
        checkpoint->add( *time_count );
        checkpoint->add( *gen_count );
        checkpoint->add( *feasible_stat );
        checkpoint->add( asize_stat );
#ifndef SINGLE_EVAL_ITER_DUMP
        clog_monitor->add( eval_counter );
#endif
        clog_monitor->add( *time_count );
        clog_monitor->add( *gen_count );
        clog_monitor->add( asize_stat );
        clog_monitor->add( *feasible_stat );

        // the checkpoint should call the monitor at every generation
        checkpoint->add( *clog_monitor );
    }
    //state.formatJSON("dae_state");

#ifdef DAE_MO
    add_stats_multi( *checkpoint, *clog_monitor, state, pop, archive, parser );
#else
    add_stats_mono( *checkpoint, *clog_monitor, state );
#endif

    // pour plus d'output (recopiés de do/make_checkpoint)
    // un state, pour sauver l'état courant
    state.registerObject(parser);
    state.registerObject(pop);
    //state.registerObject(eo::rng);


    unsigned int out_save_freq = parser.valueOf<unsigned int>("out-save-freq");
        eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "out-save-freq" << out_save_freq << std::endl;

    std::string out_dir = parser.valueOf<std::string>("out-dir");
        eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "out-dir" << out_dir << std::endl;

    bool out_dir_erase = parser.valueOf<bool>("out-dir-erase");
        eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "out-dir-erase" << out_dir_erase << std::endl;

    bool dirOK = testDirRes( out_dir, out_dir_erase ) ;
    if( ! dirOK ) {
        eo::log << eo::warnings << "WARNING: Cannot write to " << out_dir  << std::endl;
    }

    unsigned freq = out_save_freq>0 ? out_save_freq : std::numeric_limits<unsigned int>::max();
    std::string stmp = out_dir + "/generation_";

    eoCountedStateSaver* state_saver = new eoCountedStateSaver(freq, state, stmp, true, "sav", -1);
    state.storeFunctor( state_saver );

    checkpoint->add( *state_saver );
#endif // NDEBUG

    return *checkpoint;
}

} // namespace daex

#endif // _MAKE_CHECKPOINT_DAE_H_

