#ifndef __MO_STRATEGY_H__
#define __MO_STRATEGY_H__

#include <iterator>

#include <eo>
#include <moeo>

#include "../core/strategies.h"

namespace daex {

/** Configure a random strategy for each goal.
 */
template<class EOT>
class StrategyRandom : public eoMonOp<EOT>
{
public:

    StrategyRandom( const std::vector<Strategies::Type> strategies, const std::vector<double> rates )
        : _strategies( strategies ), _rates(rates)
    {}

    StrategyRandom( const std::vector<double> rates )
    {
        default_strategies();
        assert( _strategies.size() == _rates.size() );
        _rates = rates;
    }

    StrategyRandom( const std::vector<Strategies::Type> strategies )
    {
        default_rates();
    }

    StrategyRandom()
    {
        default_strategies();
        default_rates();
    }

    bool operator()( EOT & decompo )
    {
        bool modified = false;
        for( typename EOT::iterator igoal=decompo.begin(), end=decompo.end(); igoal!=end; ++igoal) {
            unsigned int which = rng.roulette_wheel( _rates );
            if( igoal->strategy() != _strategies[which] ) {
                igoal->strategy( _strategies[which] );
                modified = true;
            }
        } // for igoal in decompo
        return modified;
    }

protected:
    void default_strategies()
    {
        // default strategies : all
        _strategies.push_back(Strategies::length);
        _strategies.push_back(Strategies::cost);
        _strategies.push_back(Strategies::makespan_max);
        _strategies.push_back(Strategies::makespan_add);
    }

    void default_rates()
    {
        double rate = 1.0/_strategies.size();
        for( unsigned int i=0; i < _strategies.size(); ++i ) {
            _rates.push_back( rate );
        }
    }

    std::vector<Strategies::Type> _strategies;
    std::vector<double> _rates;
};


/** Configure a different strategy for each goal, either makespan_add or cost.
 *  Given a pobability to use the makespan strategy.
 */
template<class EOT>
class StrategyFlipGoal : public eoMonOp<EOT>
{
public:
    //! @param proba_strategy_makespan Probability to use the makespan search strategy
    StrategyFlipGoal( double proba_strategy_makespan ) : _proba_strategy_makespan(proba_strategy_makespan) {}

    bool operator()( EOT & decompo )
    {
        bool modified=false;
        for( typename EOT::iterator igoal=decompo.begin(), end=decompo.end(); igoal!=end; ++igoal) {
            if( rng.flip(_proba_strategy_makespan) ) {
                if( igoal->strategy() != Strategies::makespan_add ) {
                    igoal->strategy( Strategies::makespan_add );
                    modified=true;
                }
            } else {
                if( igoal->strategy() != Strategies::cost ) {
                    igoal->strategy( Strategies::cost );
                    modified=true;
                }
            }
        } // for igoal in decompo
        return modified;
    }

protected:
    double _proba_strategy_makespan;
};


/** Configure a random strategy for all the goals, either makespan_add or cost.
 */
template<class EOT>
class StrategyFlipDecomposition : public eoMonOp<EOT>
{
public:
    //! @param proba_strategy_makespan Probability to use the makespan search strategy
    StrategyFlipDecomposition( double proba_strategy_makespan ) : _proba_strategy_makespan(proba_strategy_makespan) {}

    bool operator()( EOT & decompo )
    {
        Strategies::Type strategy = Strategies::cost;
        if( rng.flip(_proba_strategy_makespan) ) {
            strategy = Strategies::makespan_add;
        }

        bool modified=false;
        for( typename EOT::iterator igoal=decompo.begin(), end=decompo.end(); igoal!=end; ++igoal) {
            if( igoal->strategy() != strategy ) {
                igoal->strategy( strategy );
                modified=true;
            }
        } // for igoal in decompo
        return modified;
    }

protected:
    double _proba_strategy_makespan;
};


/** Use the given strategy for each goal in a decomposition
 */
template<class EOT>
class StrategyFixed : public eoMonOp<EOT>
{
public:
    //! @param use_makespan Use the makespan search strategy of true, else the cost one.
    StrategyFixed( Strategies::Type strategy ) : _strategy(strategy) {}

    bool operator()( EOT & decompo )
    {
        bool modified=false;
        for( typename EOT::iterator igoal=decompo.begin(), end=decompo.end(); igoal!=end; ++igoal) {
            if( igoal->strategy() != _strategy ) {
                igoal->strategy( _strategy );
                modified=true;
            }
        } // for igoal in decompo
        return modified;
    }

protected:
    Strategies::Type _strategy;
};


} // namespace daex

#endif // __MO_STRATEGY_H__

