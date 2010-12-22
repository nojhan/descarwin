#ifndef COMPARISON_H
#define COMPARISON_H


typedef enum {Better = -1, Equal = 0, Worse = 1} Comparison;

#define LESS(x, y) NEST( typeof(x) _a = x; typeof(y) _b = y; if (_a < _b) return Better; if (_a > _b) return Worse; )
#define GREATER(x, y) NEST( typeof(x) _a = x; typeof(y) _b = y; if (_a > _b) return Better; if (_a < _b) return Worse; )
#define PREFER(exp1, exp2) NEST( bool _e1 = exp1, _e2 = exp2; if (_e1 && _e2) return Better; if (!_e1 && !_e2) return Worse; )

#define preferred(comp, ties) ({ Comparison test = comp; test == Worse ? false : !opt.random ? test == Better : test == Better ? (ties = 1) : rand() % ++ties == 0; })


#endif // COMPARISON_H
