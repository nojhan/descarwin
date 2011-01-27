/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : cpt.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#ifndef CPT_H
#define CPT_H 


#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <fenv.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <gmp.h>
#include <omp.h>

#ifndef __WORDSIZE
#define __WORDSIZE 32
#endif

#define VALUE_SHORT
#define TIME_LONG
//#define NOTRACE
#define RESOURCES

#ifdef VALUE_SHORT
typedef unsigned short Value;
#define VALUEP "d"
#define MAXVAL (USHRT_MAX - 1)
#define VAL_UNKNOWN USHRT_MAX
#endif
#ifdef VALUE_LONG
typedef unsigned long Value;
#define VALUEP "ld"
#define MAXVAL (ULONG_MAX - 1)
#define VAL_UNKNOWN ULONG_MAX
#endif

#define val_known(e) ((e) != VAL_UNKNOWN)
#define val_unknown(e) ((e) == VAL_UNKNOWN)

#ifdef TIME_SHORT
typedef short int TimeVal;
#define TIMEP "d"
#define MAXTIME 10000
#define MAXCOST MAXTIME
#define time_round __builtin_lrintl
#endif
#ifdef TIME_LONG
typedef long int TimeVal;
#define TIMEP "ld"
#if (__WORDSIZE == 32)
#define MAXTIME 1000000000
#else
#define MAXTIME 1000000000000000000LL
#endif
#define MAXCOST MAXTIME
#define time_round __builtin_lrintl
#endif
#ifdef TIME_LLONG
typedef long long int TimeVal;
#define TIMEP "lld"
#define MAXTIME 1000000000000000000LL
#define MAXCOST MAXTIME
#define time_round __builtin_llrintl
#endif

struct TimeStruct {
  TimeVal t;
  mpq_t q;
};

#define STRING_MAX 10000


#include "options.h"


#define NEST(body...) do { body } while (0)

#define maxi(a, b) ({ typeof(a) _x = (a); typeof(b) _y = (b); (_x > _y ? _x : _y); })
#define mini(a, b) ({ typeof(a) _x = (a); typeof(b) _y = (b); (_x < _y ? _x : _y); })
#define maximize(a, b) NEST( typeof(&a) _x = (&a); typeof(b) _y = (b); if (*_x < _y) *_x = _y; )
#define minimize(a, b) NEST( typeof(&a) _x = (&a); typeof(b) _y = (b); if (*_x > _y) *_x = _y; )


#define exchange(a, b) NEST( typeof(a) _tmp = a; a = b ; b = _tmp; )


#define check_allocation(ptr, x, res) ({ if (x > 0) { if (!(ptr = (typeof(ptr)) res)) error(allocation, "Memory allocation error"); } else ptr = NULL; ptr; })

#define cpt_calloc(ptr, n) ({ size_t _x = n; typeof(&(ptr)) _p = &(ptr); check_allocation(*_p, _x, calloc(_x, sizeof (*ptr))); })
#define cpt_malloc(ptr, n) ({ size_t _x = n; typeof(&(ptr)) _p = &(ptr); check_allocation(*_p, _x, malloc(_x * sizeof (*ptr))); })
#define cpt_realloc(ptr, n) ({ size_t _x = n; typeof(&(ptr)) _p = &(ptr); check_allocation(*_p, _x, realloc(*_p, _x * sizeof (*ptr))); })
#define cpt_free(ptr) NEST( free(ptr); ptr = NULL; )


/* Vector facilities */

#define VECTOR(args...) _mkvector(args)
#define EVECTOR(args...) _mkvector(args, extern)
#define SVECTOR(args...) _mkvector(args, static)
#define _mkvector(type, name, kw...) kw type *name; kw long name##_nb
#define vector_copy(dest, source) NEST( cpt_malloc(dest, (dest##_nb = source##_nb)); memcpy(dest, source, dest##_nb * sizeof(typeof(*dest))); )

/* Bit arrays */

typedef unsigned long *BitArray;

#define bitarray_create(n) ((unsigned long *) calloc(((n) - 1) / __WORDSIZE + 1, sizeof(unsigned long)))
#define bitarray_copy(dest, source, n) memcpy(dest, source, (((n) - 1) / __WORDSIZE + 1) * sizeof(unsigned long))
#define bitarray_cmp(dest, source, n) memcmp(dest, source, (((n) - 1) / __WORDSIZE + 1) * sizeof(unsigned long))
#define bitarray_set_index(x) NEST( (x)->bit_index = (x)->id / __WORDSIZE; (x)->bit_mask = 1L << ((x)->id % __WORDSIZE); )
#define bitarray_set(tab, x) NEST( tab[(x)->bit_index] |= (x)->bit_mask; )
#define bitarray_unset(tab, x) NEST( tab[(x)->bit_index] &= ~ (x)->bit_mask; )
#define bitarray_save_and_set(tab, x) NEST( save(tab[(x)->bit_index]); tab[(x)->bit_index] |= (x)->bit_mask; )
#define bitarray_get(tab, x) ( tab[(x)->bit_index] & (x)->bit_mask )


/* Loop facilities */

#define _for(x, i, tab, min, max) do { typeof(max) i; for (i = min; i < max; i++) { typeof(*(tab)) x = (tab)[i];
#define _rfor(x, i, tab, max, min) do { typeof(max) i; for (i = max; i >= min; i--) { typeof(*(tab)) x = (tab)[i];

#define FOR(x, tab) _for(x, _i, tab, 0, tab##_nb)
#define FORi(x, i, tab) _for(x, i, tab, 0, tab##_nb)
#define FORMIN(x, tab, min) _for(x, _i, tab, min, tab##_nb)
#define FORMINi(x, i, tab, min) _for(x, i, tab, min, tab##_nb)
#define FORMAX(x, tab, max) _for(x, _i, tab, 0, max)
#define FORMAXi(x, i, tab, max) _for(x, i, tab, 0, max)
#define FORMINMAX(x, tab, min, max) _for(x, _i, tab, min, max)

#define RFOR(x, tab) _rfor(x, _i, tab, tab##_nb - 1, 0)
#define RFORi(x, i, tab) _rfor(x, i, tab, tab##_nb - 1, 0)
#define RFORMIN(x, tab, min) _rfor(x, _i, tab, tab##_nb - 1, min)
#define RFORMINi(x, i, tab, min) _rfor(x, i, tab, tab##_nb - 1, min)
#define RFORMAX(x, tab, max) _rfor(x, _i, tab, max, 0)
#define RFORMAXi(x, i, tab, max) _rfor(x, i, tab, max, 0)

#define EFOR }} while (0)

#define FOR2(x, tab, y, tab2) do { typeof(*(tab)) x; typeof(*(tab2)) y; typeof(tab##_nb) _i; for(_i = 0; _i < tab##_nb; _i++) { x = tab[_i]; y = tab2[_i];
#define FORPAIR(x1, x2, t) FORi(x1, __i, t) FORMIN(x2, t, __i) 
#define FORCOUPLE(x1, t1, x2, t2) FOR(x1, t1) FOR(x2, t2) 
#define EFORPAIR EFOR; EFOR
#define EFORCOUPLE EFOR; EFOR


#define mpz_get_timeval(n) ({ char *s = mpz_get_str(NULL, 10, n); TimeVal res = atoll(s); free(s); res; })

#endif /* CPT_H */

