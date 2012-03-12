/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : yahsp.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#ifndef YAHSP_H
#define YAHSP_H

typedef BitArray State;
typedef struct Node Node;
typedef struct YStep YStep;

struct YStep {
  Action *action;
  TimeVal init;
};

struct Node {
  size_t id;
  Node *parent;
  ulong key;
  State state;
  ulong fvalue;
  ulong length;
  TimeVal makespan;
  VECTOR(YStep, steps);
  VECTOR(Action *, applicable);
#ifdef YAHSP_MPI
  int rank;
#endif
};

#ifdef YAHSP_MT
#define DO_PRAGMA(x) _Pragma(#x)
#define OMP(x) x
#else
#define DO_PRAGMA(x)
#define OMP(x)
#endif

#define OMP_ATOMIC DO_PRAGMA(omp atomic)
#define OMP_MASTER DO_PRAGMA(omp master)
#define OMP_SINGLE DO_PRAGMA(omp single)
#define OMP_CRITICAL DO_PRAGMA(omp critical)
#define OMP_BARRIER DO_PRAGMA(omp barrier)
#define OMP_FLUSH(x) DO_PRAGMA(omp flush (x))
#define OMP_PARALLEL(x) DO_PRAGMA(omp parallel x)


#define state_create() bitarray_create(fluents_nb)
#define state_cmp(s1, s2) bitarray_cmp(s1, s2, fluents_nb)
#define state_clone(dest, src) bitarray_clone(dest, src, fluents_nb)
#define state_copy(dest, src) bitarray_copy(dest, src, fluents_nb)
#define state_contains(s, f) bitarray_get(s, f)
#define state_add(s, f) bitarray_set(s, f)
#define state_del(s, f) bitarray_unset(s, f)


extern void yahsp_init();
extern int yahsp_main(ulong max_evaluated_nodes);
extern void yahsp_stop_on_timer();
extern void yahsp_reset();
extern int yahsp_search(Fluent **init, long init_nb, Fluent **goals, long goals_nb);
/* Ajouté par Pierre Savéant et Johann Dréo le 2010-03-26 */
extern int yahsp_compress_plans();
extern BitArray * get_current_state();
/* PS & JD */


#endif /* YAHSP_H */
