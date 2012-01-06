/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : yahsp-dae.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */

#include "cpt.h"
#include "options.h"
#include "trace.h"
#include "globs.h"
#include "yahsp.h"
#include "yahsp-dae.h"
#include "yahsp-common.h"

#define end_action (&yend_action)

typedef struct Heuristic Heuristic;

struct Heuristic {
  ulong key;
  BitArray state;
  TimeVal *inits;
};

static gdsl_rbtree_t heuristics;

#ifdef SHARED_MEMOIZATION
static omp_lock_t heuristics_lock;
#else
#pragma omp threadprivate(heuristics)
#endif

static void heuristic_free(Heuristic *h)
{
  cpt_free(h->state);
  cpt_free(h->inits);
  cpt_free(h);
}

static Comparison heuristic_cmp(Heuristic *h1, Heuristic *h2)
{
  LESS(h1->key, h2->key);
  return (Comparison) bitarray_cmp(h1->state, h2->state, fluents_nb);
}

void heuristic_create()
{
#ifdef SHARED_MEMOIZATION
  omp_init_lock(&heuristics_lock);
#pragma omp master
#endif
  heuristics = gdsl_rbtree_alloc(NULL, NULL, (gdsl_free_func_t) heuristic_free, (gdsl_compare_func_t) heuristic_cmp);
}

void heuristic_insert(Node *node, TimeVal *finit)
{
  Heuristic *h = cpt_malloc(h, 1);
  int gdsl_return;
  h->key = node->key;
  bitarray_clone(h->state, node->state, fluents_nb);
  cpt_malloc(h->inits, fluents_nb);
  memcpy(h->inits, finit, fluents_nb * sizeof(TimeVal));
#ifdef SHARED_MEMOIZATION
  omp_set_lock(&heuristics_lock);
#endif
  gdsl_rbtree_insert(heuristics, h, &gdsl_return);
#ifdef SHARED_MEMOIZATION
  omp_unset_lock(&heuristics_lock);
#endif
}

bool heuristic_search(Node *node, TimeVal *finit)
{
  Heuristic tmp;
  tmp.key = node->key;
  tmp.state = node->state;
#ifdef SHARED_MEMOIZATION
  omp_set_lock(&heuristics_lock);
#endif
  Heuristic *h = (Heuristic *) gdsl_rbtree_search(heuristics, (gdsl_compare_func_t) heuristic_cmp, &tmp);
#ifdef SHARED_MEMOIZATION
  omp_unset_lock(&heuristics_lock);
#endif
  if (h == NULL) return false;
  memcpy(finit, h->inits, fluents_nb * sizeof(TimeVal));
  return true;
}

void heuristic_flush()
{
  gdsl_rbtree_flush(heuristics);
}

