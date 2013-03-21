
#ifndef _MAKE_REPLACE_DAE_
#define _MAKE_REPLACE_DAE_

namespace daex {

void do_make_replace_param( eoParser & parser )
{
    // MODIFS MS START
    // unsigned replaceTourSize = parser.createParam(unsigned(1), "replaceTourSize", "Size of Replacement Tournament (1->deterministic (hum, pas logique ;-(", '\0', "Evolution Engine" ).value();
    // eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "replaceTourSize" << replaceTourSize << std::endl;

    bool weakElitism = parser.createParam(true, "weakElitism", "Weak Elitism in replacement", '\0', "Evolution Engine").value();
    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "weakElitism" << weakElitism << std::endl;

//    bool removeDuplicates = parser.createParam(false, "removeDuplicates", "Does not allow duplicates in replacement (if possible)", '\0', "Evolution Engine").value();
//    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "removeDuplicates" << removeDuplicates << std::endl;

//    std::string plusOrComma =  parser.createParam(std::string("Comma"), "plusOrComma", "Plus (parents+offspring) or Comma (only offspring) for replacement", '\0', "Evolution Engine").value();
//    eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "plusOrComma" << plusOrComma << std::endl;
}

template<class EOT>
eoReplacement<EOT> & do_make_replace_op( eoParser & parser, eoState & state )
{
    // unsigned int replaceTourSize = parser.valueOf<unsigned int>("replaceTourSize");
    bool weakElitism = parser.valueOf<bool>("weakElitism");

    // JACK : L'article indique qu'on fait un remplacement en tournoi déterministe et qu'il n'y a pas d'élistisme, on aurait alors ça :
    //eoSSGADetTournamentReplacement<EOT> replace(5);
    
    // MODIFS MS START
    // plus de paramétrage du remplacement
    eoReplacement<EOT> * pt_replace;
    // MAIS le code utilise un remplacement intégral des parents par le meilleur des parents+enfants, avec élitisme faible, on a donc 
    // Note MS: le commentaire était incohérent entre Plus et Comma ???
    
    // Use the eoMergeReduce construct
    
    // Merge: either merge (Plus strategy, parents + offspring) or only keep offspring (Comma)
    // eoMerge<EOT> * pt_merge;
    // if (plusOrComma == "Comma") {
    //   pt_merge = (eoMerge<EOT> *) (new eoNoElitism<EOT>);
    // } else { // Plus
    //   pt_merge = (eoMerge<EOT> *) (new eoPlus<EOT>);
    // }
    // // Reduce: either truncate (deterministic) or tournament
    // eoReduce<EOT> * pt_reduce;
    // if (replaceTourSize == 1) {
    //   pt_reduce = (eoReduce<EOT> *) (new eoTruncate<EOT>);
    // } else {
    //   pt_reduce = (eoReduce<EOT> *) (new eoDetTournamentTruncate<EOT> ( replaceTourSize ));
    // }
    // // the full MergeReduce object
    // eoMergeReduce<EOT> mergeReduce (*pt_merge, *pt_reduce);

    eoReplacement<EOT> * mergeReduce = new eoCommaReplacement<EOT>(); // idem à comma et replaceTourSize == 1


    //   eoReplacement<EOT> * mergeReduce = new eoEPReplacement<EOT>(replaceTourSize);
    state.storeFunctor( mergeReduce );

    // Now the weak elitism
    if (weakElitism) {
        pt_replace = static_cast<eoReplacement<EOT>*>(new eoWeakElitistReplacement<EOT>( *mergeReduce ) );
        state.storeFunctor( pt_replace );

    } else {
        pt_replace = mergeReduce;
    }

    return *pt_replace;
}


} // namespace daex

#endif // _MAKE_REPLACE_DAE_
