/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : cpt.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#ifndef CPT_H
#define CPT_H 

#define __STDC_CONSTANT_MACROS
#define __STDC_LIMIT_MACROS 
#define __STDC_FORMAT_MACROS

#include <ctype.h>
#include <fenv.h>
#include <gmp.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <termcap.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "../gdsl/src/gdsl.h"
#ifdef _OPENMP
#include <omp.h>
#endif

#define WORDSIZE (sizeof(long) * CHAR_BIT)

#ifndef BITS_VALUE
#define BITS_VALUE 16
#endif
#ifndef BITS_TIME
#define BITS_TIME 64
#endif

//#define NOTRACE
#define RESOURCES


#if (BITS_VALUE == 8)
typedef uint8_t Value;
#define VALUEP PRIu8
#define MAXVAL (UINT8_MAX - 1)
#define VAL_UNKNOWN UINT8_MAX
#endif
#if (BITS_VALUE == 16)
typedef uint16_t Value;
#define VALUEP PRIu16
#define MAXVAL (UINT16_MAX - 1)
#define VAL_UNKNOWN UINT16_MAX
#endif
#if (BITS_VALUE == 32)
typedef uint32_t Value;
#define VALUEP PRIu32
#define MAXVAL (UINT32_MAX - 1)
#define VAL_UNKNOWN UINT32_MAX
#endif

#define val_known(e) ((e) != VAL_UNKNOWN)
#define val_unknown(e) ((e) == VAL_UNKNOWN)

#if (BITS_TIME == 16)
typedef int16_t TimeVal;
#define TIMEP PRId16
#define MAXTIME INT16_C(10000)
#define MAXCOST MAXTIME
#define time_round lrintl
#endif
#if (BITS_TIME == 32)
typedef int32_t TimeVal;
#define TIMEP PRId32
#define MAXTIME INT32_C(1000000000)
#define MAXCOST MAXTIME
#define time_round lrintl
#endif
#if (BITS_TIME == 64)
typedef int64_t TimeVal;
#define TIMEP PRId64
#define MAXTIME INT64_C(1000000000000000000)
#define MAXCOST MAXTIME
#define time_round llrintl
#endif

struct TimeStruct {
  TimeVal t;
  mpq_t q;
};

#define STRING_MAX 10000

#define NEST(body...) do { body } while (0)

#define maxi(a, b) ({ typeof(a) _a = a, _b = b; (_a > _b ? _a : _b); })
#define mini(a, b) ({ typeof(a) _a = a, _b = b; (_a < _b ? _a : _b); })
#define maximize(a, b) NEST( typeof(b) _b = b; if (a < _b) a = _b; )
#define minimize(a, b) NEST( typeof(b) _b = b; if (a > _b) a = _b; )
#define exchange(a, b) NEST( typeof(a) _a = a; a = b ; b = _a; )

/* Allocation */

#define check_alloc(ptr, x, res) ({ if (x > 0) { if ((ptr = (typeof(ptr)) res) == NULL) error(allocation, "Memory allocation error"); } else ptr = NULL; ptr; })

#define cpt_calloc(ptr, n) ({ size_t _n = n; typeof(&ptr) _p = &ptr; check_alloc(*_p, _n, calloc(_n, sizeof (*ptr))); })
#define cpt_malloc(ptr, n) ({ size_t _n = n; typeof(&ptr) _p = &ptr; check_alloc(*_p, _n, malloc(_n * sizeof (*ptr))); })
#define cpt_realloc(ptr, n) ({ size_t _n = n; typeof(&ptr) _p = &ptr; check_alloc(*_p, _n, realloc(*_p, _n * sizeof (*ptr))); })
#define cpt_free(ptr) NEST( free(ptr); ptr = NULL; )

/* Vectors */

#define VECTOR(args...) _mkvector(args)
#define EVECTOR(args...) _mkvector(args, extern)
#define SVECTOR(args...) _mkvector(args, static)
#define _mkvector(type, name, kw...) kw type *name; kw size_t name##_nb
#define vector_copy(dest, source) memcpy(cpt_malloc(dest, (dest##_nb = source##_nb)), source, source##_nb * sizeof(typeof(*dest)))
#define vector_sort(name, cmp_func) qsort(name, name##_nb, sizeof(typeof(*name)), (int (*) (const void *, const void *)) cmp_func)

/* Bit arrays */

typedef ulong *BitArray;

#define bitarray_create(n) ((BitArray) calloc(((n) - 1) / WORDSIZE + 1, sizeof(BitArray*)))
#define bitarray_malloc(n) ((BitArray) malloc((((n) - 1) / WORDSIZE + 1) * sizeof(BitArray*)))
#define bitarray_copy(dest, source, n) memcpy(dest, source, (((n) - 1) / WORDSIZE + 1) * sizeof(BitArray*))
#define bitarray_clone(dest, source, n) bitarray_copy(dest = bitarray_malloc(n), source, n)
#define bitarray_cmp(dest, source, n) memcmp(dest, source, (((n) - 1) / WORDSIZE + 1) * sizeof(BitArray*))
#define bitarray_set_index(x) NEST( (x)->bit_index = (x)->id / WORDSIZE; (x)->bit_mask = 1UL << ((x)->id % WORDSIZE); )
#define bitarray_set(tab, x) (tab[(x)->bit_index] |= (x)->bit_mask)
#define bitarray_unset(tab, x) (tab[(x)->bit_index] &= ~(x)->bit_mask)
#define bitarray_get(tab, x) (tab[(x)->bit_index] & (x)->bit_mask)
#define bitarray_save_and_set(tab, x) NEST( save(tab[(x)->bit_index]); tab[(x)->bit_index] |= (x)->bit_mask; )

/* Loops on vectors */

#define _for(x, i, tab, min, max) do { typeof(max) i; typeof(*(tab)) x; for (i = min; i < max; i++) { x = (tab)[i];
#define _rfor(x, i, tab, max, min) do { typeof(max) i, _j; typeof(*(tab)) x; for (_j = min, i = max - 1; _j < max; i--, _j++) { x = (tab)[i];

#define FOR(x, tab) _for(x, _i, tab, 0, tab##_nb)
#define FORi(x, i, tab) _for(x, i, tab, 0, tab##_nb)
#define FORMIN(x, tab, min) _for(x, _i, tab, min, tab##_nb)
#define FORMINi(x, i, tab, min) _for(x, i, tab, min, tab##_nb)
#define FORMAX(x, tab, max) _for(x, _i, tab, 0, max)
#define FORMAXi(x, i, tab, max) _for(x, i, tab, 0, max)
#define FORMINMAX(x, tab, min, max) _for(x, _i, tab, min, max)

#define RFOR(x, tab) _rfor(x, _i, tab, tab##_nb, 0)
#define RFORi(x, i, tab) _rfor(x, i, tab, tab##_nb, 0)
#define RFORMIN(x, tab, min) _rfor(x, _i, tab, tab##_nb, min)
#define RFORMINi(x, i, tab, min) _rfor(x, i, tab, tab##_nb, min)
#define RFORMAX(x, tab, max) _rfor(x, _i, tab, max, 0)
#define RFORMAXi(x, i, tab, max) _rfor(x, i, tab, max, 0)

#define FOR2(x, tab1, y, tab2) do { typeof(*(tab1)) x; typeof(*(tab2)) y; typeof(tab1##_nb) _i; for(_i = 0; _i < tab1##_nb; _i++) { x = tab1[_i]; y = tab2[_i];
#define FORPAIR(x1, x2, tab) FORi(x1, __i, tab) FORMIN(x2, tab, __i) 
#define FORCOUPLE(x1, tab1, x2, tab2) FOR(x1, tab1) FOR(x2, tab2) 

#define EFOR }} while (0)
#define EFORPAIR EFOR; EFOR
#define EFORCOUPLE EFOR; EFOR

/* Random numbers */

#define cpt_srand(seed) srand48_r(seed, &random_buffer)
#define cpt_rand() ({ long _x; lrand48_r(&random_buffer, &_x); _x; })

/* Comparisons */

typedef enum {Better = -1, Equal = 0, Worse = 1} Comparison;

#define LESS(x, y) NEST( typeof(x) _x = x, _y = y; if (_x < _y) return Better; if (_x > _y) return Worse; )
#define GREATER(x, y) NEST( typeof(x) _x = x, _y = y; if (_x > _y) return Better; if (_x < _y) return Worse; )
#define PREFER(x, y) NEST( bool _x = x, _y = y; if (_x && _y) return Better; if (!_x && !_y) return Worse; )

#define preferred(comp, rand, ties) ({ Comparison _c = comp; _c == Worse ? false : !rand ? _c == Better : _c == Better ? (ties = 1) : cpt_rand() % ++ties == 0; })

#define mpz_get_timeval(n) ({ char *_s = mpz_get_str(NULL, 10, n); TimeVal res = atoll(_s); free(_s); res; })


#endif /* CPT_H */

