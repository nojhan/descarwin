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
  long id;
  Node *parent;
  unsigned long key;
  State state;
  long fvalue;
  long length;
  TimeVal makespan;
  VECTOR(YStep, steps);
  VECTOR(Action *, applicable);
#ifdef YAHSP_MPI
  int parent_rank;
#endif
};

#define state_create() bitarray_create(fluents_nb)
#define state_cmp(s1, s2) bitarray_cmp(s1, s2, fluents_nb)
#define state_clone(dest, src) bitarray_clone(dest, src, fluents_nb)
#define state_copy(dest, src) bitarray_copy(dest, src, fluents_nb)
#define state_contains(s, f) bitarray_get(s, f)
#define state_add(s, f) bitarray_set(s, f)
#define state_del(s, f) bitarray_unset(s, f)


#define get_ainit(a) ainit[(a)->id]
#define set_ainit(a, t) ainit[(a)->id] = t
#define get_aused(a) aused[(a)->id]
#define set_aused(a, t) aused[(a)->id] = t
#define get_finit(f) finit[(f)->id]
#define set_finit(f, t) finit[(f)->id] = t


#ifdef DAE

#define COST(a) ({ TimeVal cost = 0; FOR(f, a->prec) { if (get_finit(f) == MAXTIME) { cost = MAXTIME; break; } else cost += get_finit(f); } EFOR; cost; })
//#define COST(a) ({ TimeVal cost = 0; FOR(f, a->prec) { maximize(cost, get_finit(f)); } EFOR; cost; })
//#define INCCOST(cost, action) (cost += ceil(duration(action) * (double) pddl_domain->time_gcd / pddl_domain->time_lcm) + (pddl_domain->action_costs ? 1 : 0))
#define INCCOST(cost, action) (cost += duration(action) + 1)
#define NODE_GVALUE(node) node->length
#define NODE_HVALUE(node) get_ainit(end_action)
//#define NODE_HVALUE(node) relaxed_plan_nb
#define NODE_FVALUE(node) (NODE_GVALUE(node) + NODE_HVALUE(node))
//#define NODE_FVALUE(node) (NODE_HVALUE(node))

#else

#define COST(a) ({ TimeVal cost = 0; FOR(f, a->prec) { if (get_finit(f) == MAXTIME) { cost = MAXTIME; break; } else cost += get_finit(f); } EFOR; cost; })
//#define COST(a) ({ TimeVal cost = 0; FOR(f, a->prec) { maximize(cost, get_finit(f)); } EFOR; cost; })
//#define INCCOST(cost, action) (cost += duration(action) * pddl_domain->time_gcd / pddl_domain->time_lcm + 1)
#define INCCOST(cost, action) (cost++)
#define NODE_GVALUE(node) node->length
#define NODE_HVALUE(node) get_ainit(end_action)
//#define NODE_HVALUE(node) relaxed_plan_nb
#define NODE_FVALUE(node) (NODE_GVALUE(node) + NODE_HVALUE(node) * 3)

#endif


extern void yahsp_init();
extern int yahsp_main();
extern void yahsp_reset();
extern int yahsp_search(Fluent **init, long init_nb, Fluent **goals, long goals_nb);
extern bool action_must_precede(Action *a1, Action *a2);
extern int yahsp_compress_plans();

/* Ajouté par Pierre Savéant et Johann Dréo le 2010-03-26 */
extern BitArray * get_current_state();
/* PS & JD */


#endif /* YAHSP_H */
