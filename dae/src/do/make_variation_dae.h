
#ifndef _MAKE_VARIATION_DAE_
#define _MAKE_VARIATION_DAE_

namespace daex {

void do_make_variation_param( eoParser & parser, unsigned int pop_size )
{
    // Selection
    unsigned int toursize = parser.createParam( (unsigned int)5, "tournament", 
            "Size of the deterministic tournament for the selection", 't', "Selection" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "toursize" << toursize << std::endl;

    unsigned int offsprings = parser.createParam( (unsigned int)pop_size*7, "offsprings", 
            "Number of offsprings to produces", 'f', "Selection" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "offsprings" << offsprings << std::endl;


    // Variation
    unsigned int radius = parser.createParam( (unsigned int)2, "radius", 
            "Number of neighbour goals to consider for the addGoal mutation", 'R', "Variation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "radius" << radius << std::endl;

    double proba_change = parser.createParam( (double)0.8, "proba-change", 
            "Probability to change an atom for the changeAtom mutation", 'c', "Variation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "proba_change" << proba_change << std::endl;

    double proba_del_atom = parser.createParam( (double)0.8, "proba-del-atom", 
            "Average probability to delete an atom for the delAtom mutation", 'd', "Variation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "proba_del_atom" << proba_del_atom << std::endl;

    double w_delgoal = parser.createParam( (double)1, "w-delgoal", 
            "Relative weight defining the probability to call the delGoal mutation", 'a', "Variation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "w_delgoal" << w_delgoal << std::endl;

    double w_addgoal = parser.createParam( (double)3, "w-addgoal", 
            "Relative weight defining the probability to call the addGoal mutation", 'A', "Variation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "w_addgoal" << w_addgoal << std::endl;

    double w_delatom = parser.createParam( (double)1, "w-delatom", 
            "Relative weight defining the probability to call the delAtom mutation", 'g', "Variation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "w_delatom" << w_delatom << std::endl;

    double w_addatom = parser.createParam( (double)1, "w-addatom", 
            "Relative weight defining the probability to call the addAtom mutation", 'G', "Variation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "w_addatom" << w_addatom << std::endl;

    double proba_cross = parser.createParam( (double)0.2, "proba-cross", 
            "Probability to apply a cross-over", 'c', "Variation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "proba_cross" << proba_cross << std::endl;

    double proba_mut = parser.createParam( (double)0.8, "proba-mut", 
            "Probability to apply one of the mutation", 'm', "Variation" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "proba_mut" << proba_mut << std::endl;
    
    /*
    unsigned int maxtry_candidate = parser.createParam( (unsigned int)11, "maxtry-candidate", 
            "Maximum number of atoms to try when searching for a candidate in the changeAtom mutation", 'y', "Misc" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "maxtry_candidate" << maxtry_candidate << std::endl;


    unsigned int maxtry_mutex = parser.createParam( (unsigned int)11, "maxtry-mutex", 
            "Maximum number of atoms to try when searching for mutexes in the changeAtom mutation", 'z', "Misc" ).value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "maxtry_mutex" << maxtry_mutex << std::endl;
    */
}

template<class EOT>
eoGeneralBreeder<EOT> & do_make_variation_op( eoParser & parser, eoState & state, pddlLoad & pddl, MutationDelGoal<EOT> * ext_delgoal = NULL )
{
    unsigned int toursize = parser.valueOf<unsigned int>("tournament");
    unsigned int offsprings = parser.valueOf<unsigned int>("offsprings");

    unsigned int radius = parser.valueOf<unsigned int>("radius");
    double proba_change = parser.valueOf<double>("proba-change");
    double proba_del_atom = parser.valueOf<double>("proba-del-atom");
    double w_delgoal = parser.valueOf<double>("w-delgoal");
    double w_addgoal = parser.valueOf<double>("w-addgoal");
    double w_delatom = parser.valueOf<double>("w-delatom");
    double w_addatom = parser.valueOf<double>("w-addatom");
    double proba_cross = parser.valueOf<double>("proba-cross");
    double proba_mut = parser.valueOf<double>("proba-mut");

    unsigned int maxtry_candidate = 0; // deactivated by default: should try every candidates
    unsigned int maxtry_mutex = 0;     // deactivated by default: should try every candidates

    // SELECTION
    // TODO cet opérateur, fait soit un tri de la pop (true), soit un shuffle (false), idéalement, on ne voudrait ni l'un ni l'autre, car on parcours tout, peu importe l'ordre
    
    // JACK the article indicate that tere is no selection, but the article use a deterministic tournament

    eoSelectOne<EOT> * p_selectone;
    if ( toursize == 1 ) {
    // L'article indique qu'il n'y a pas de sélection, on aurait alors ça :
      p_selectone = (eoSelectOne<EOT> *) ( new eoSequentialSelect<EOT> ( true ) );
    }
    else {
    /// MAIS le code utilise un tournoi déterministe, on a donc ça :
      p_selectone = (eoSelectOne<EOT> *) ( new eoDetTournamentSelect<EOT> ( toursize ) );
    }
    state.storeFunctor( p_selectone );
    
    // VARIATION

    // mutations
//    MutationDelOneAtom delatom;
    MutationDelAtom<EOT> * delatom = new MutationDelAtom<EOT>( proba_del_atom );
    state.storeFunctor( delatom );

    // if we want to use an extern delgoal
    MutationDelGoal<EOT> * delgoal;
    if( ext_delgoal ) {
        delgoal = ext_delgoal;
    } else {
        delgoal = new MutationDelGoal<EOT>;
    }
    state.storeFunctor( delgoal );

    // partition, radius, l_max
    MutationAddGoal<EOT> * addgoal = new MutationAddGoal<EOT>( pddl.chronoPartitionAtom(), radius /*, init.l_max()*/ );
    state.storeFunctor( addgoal );

    // partition, proba_change, proba_add, maxtry_search_candidate, maxtry_search_mutex 
    // (maxtry à 0 pour essayer tous les atomes)
    //    MutationAddAtom addatom( pddl.chronoPartitionAtom(), 0.8, 0.5, 11, 11 );
    //    MutationAddAtom addatom( pddl.chronoPartitionAtom(), proba_change, maxtry_candidate, maxtry_mutex );
    MutationChangeAddAtom<EOT> * addatom = new MutationChangeAddAtom<EOT>( pddl.chronoPartitionAtom(), proba_change, maxtry_candidate, maxtry_mutex );
    state.storeFunctor( addatom );

    // call one of operator, chosen randomly
    eoPropCombinedMonOp<EOT> * mutator = new eoPropCombinedMonOp<EOT>( *delgoal, w_delgoal );
    state.storeFunctor( mutator );
    mutator->add( *delatom, w_delatom );
    mutator->add( *addgoal, w_addgoal );
    mutator->add( *addatom, w_addatom );

    // crossover
    // JACK in the crossover, filter out the right half of the decomposition from goals that have a greater date than the date at which we cut
    CrossOverTimeFilterHalf<EOT> * crossover = new CrossOverTimeFilterHalf<EOT>;
    state.storeFunctor( crossover );

    // first call the crossover with the given proba, then call the mutator with the given proba
    // FIXME most of the mutation use the last_reached information, what if they are called after a crossover?
    eoSGAGenOp<EOT> * variator = new eoSGAGenOp<EOT>( *crossover, proba_cross, *mutator, proba_mut);
    state.storeFunctor( variator );

    // selector, variator, rate (for selection), interpret_as_rate
    eoGeneralBreeder<EOT> * breed = new eoGeneralBreeder<EOT>( *p_selectone, *variator, (double)offsprings, false );
    state.storeFunctor( breed );
    // FIXME tester si on veut 700% ou 700

    return *breed;
}

} // namespace daex

#endif // _MAKE_VARIATION_DAE_

