 
#ifndef _MAKE_CONTINUE_DAE_H_
#define _MAKE_CONTINUE_DAE_H_

#include <vector>

#include <eo>
#include <moeo>

#include <daex.h>
//#include <cli.h>

namespace daex {

void do_make_continue_param( eoParser & parser )
{
    unsigned int mingen = parser.createParam( (unsigned int)10, "gen-min", 
            "Minimum number of iterations", 'n', "Stopping criterions" ).value();
        eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "mingen" << mingen << std::endl;

    unsigned int steadygen = parser.createParam( (unsigned int)50, "gen-steady", 
            "Number of iterations without improvement", 's', "Stopping criterions" ).value();
        eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "steadygen" << steadygen << std::endl;

    unsigned int maxgens = parser.createParam( (unsigned int)1000, "gen-max", 
            "Maximum number of iterations", 'x', "Stopping criterions" ).value();
        eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "maxgens" << maxgens << std::endl;

#ifdef DAE_MO
    eoValueParam< std::vector<double> > & pareto_target = parser.createParam((std::vector<double> )(4,1,2,3,4), "pareto-target", // FIXME better unreachable default value?
            "Stop when Pareto set reaches",'p', "Stopping criterion");
        eo::log << eo::logging << FORMAT_LEFT_FILL_W_PARAM << "pareto-target";
            for( unsigned int i=0; i<pareto_target.value().size(); ++i){ eo::log << pareto_target.value().at(i) << ", "; }
            eo::log << std::endl;
#endif

}


template <class EOT>
eoCombinedContinue<EOT> & do_make_continue_op( eoParser & parser, eoState & state
#ifdef DAE_MO
        , moeoArchive<EOT>& archive
#endif
    )
{
    unsigned int mingen = parser.valueOf<unsigned int>("gen-min");
    unsigned int steadygen = parser.valueOf<unsigned int>("gen-steady");
    unsigned int maxgens = parser.valueOf<unsigned int>("gen-max");

    eoSteadyFitContinue<EOT>* steadyfit = new eoSteadyFitContinue<EOT>( mingen, steadygen );
    state.storeFunctor( steadyfit );

    eoGenContinue<EOT>* maxgen = new eoGenContinue<EOT>( maxgens );
    state.storeFunctor( maxgen );

    // combine the continuators
    eoCombinedContinue<EOT>* continuator = new eoCombinedContinue<EOT>( *steadyfit );
    state.storeFunctor( continuator );

    continuator->add(*maxgen);

#ifdef DAE_MO
    // if the user asked for a pareto-target continuator
    if ( parser.isItThere("pareto-target") ) {
        std::vector<double> pareto_target = parser.valueOf< std::vector<double> >("pareto-target");
        moeoHypContinue<EOT>* hypcont = new moeoHypContinue<EOT>( pareto_target, archive, true, 1.1 );
        state.storeFunctor(hypcont);
        continuator->add(*hypcont);
    }
#endif
    return *continuator;
}


template <class EOT>
eoContinue<EOT> & do_make_continue( eoParser & parser, eoState & state
#ifdef DAE_MO
        , moeoArchive<EOT>& archive
#endif
    )
{
    do_make_continue_param( parser );
    return do_make_continue_op<EOT>( parser, state
#ifdef DAE_MO
        , archive
#endif
        );
}



} // namespace daex

#endif // _MAKE_CONTINUE_DAE_H_
