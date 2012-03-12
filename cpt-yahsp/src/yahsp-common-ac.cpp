/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : yahsp-common.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */

#include "cpt.h"
#include "options.h"
#include "trace.h"
#include "structs.h"
#include "problem.h"
#include "max_atom.h"
#include "plan.h"
#include "trace_planner.h"
#include "globs.h"
#include "propagations.h"
// VERY DIRTY
#define end_action (&yend_action)
#include "yahsp.h"
#include "yahsp-common-ac.h"
#include "yahsp-mpi.h"
#ifdef DAE
#include "yahsp-dae.h"
#endif

static long yahsp_state_size;

TimeVal *ainit;
TimeVal *aused;
TimeVal *finit;

VECTOR(Action *, relaxed_plan);
VECTOR(Action *, applicable);

#ifdef YAHSP_MT
#pragma omp threadprivate(ainit, aused, finit, relaxed_plan, relaxed_plan_nb, applicable, applicable_nb)
#endif

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
//#define NODE_FVALUE(node) (NODE_HVALUE(node))

#endif


void alloc_eval_structures()
{
  yahsp_state_size = ((fluents_nb - 1) / WORDSIZE + 1) * sizeof(long);
  cpt_malloc(aused, actions_nb);
  cpt_malloc(ainit, actions_nb);
  cpt_malloc(finit, fluents_nb);
  cpt_malloc(relaxed_plan, actions_nb);
  cpt_malloc(applicable, actions_nb);
}

void free_eval_structures() 
{
  cpt_free(aused);
  cpt_free(ainit);
  cpt_free(finit);
  cpt_free(relaxed_plan);
  cpt_free(applicable);
}

Node *create_node(State state)
{
  Node *node = cpt_calloc(node, 1);
  state_clone(node->state, state);
  OMP_ATOMIC stats.computed_nodes++;
  return node;
}

void node_free(Node *node) 
{ 
  cpt_free(node->state);
#ifdef YAHSP_MPI
  if (node->rank == mpi_get_rank())
#endif
    cpt_free(node->steps); 
  cpt_free(node->applicable);
  cpt_free(node);
}

Node *node_derive(Node *node)
{
  Node *son = create_node(node->state);
  son->parent = node;
  son->length = node->length;
  son->makespan = node->makespan;
#ifdef YAHSP_MPI
  son->rank = mpi_get_rank();
  //son->fvalue = node->fvalue - node->steps_nb;
  son->fvalue = node->fvalue;
#endif
  return son;
}

void node_compute_key(Node *node)
{
  // D. J. Bernstein hash function
  long i;
  for (i = 0; i < yahsp_state_size; i++) 
    node->key = node->key * 33 + ((uint8_t *) node->state)[i];
}

void copy_applicable_actions(Node *node)
{
  vector_copy(node->applicable, applicable);
}

TimeVal get_action_cost(Action *a)
{ return get_ainit(a); }

TimeVal get_node_fvalue(Node *node)
{ return NODE_FVALUE(node); }


static inline Comparison is_best_action_rp(Action *prod, Action *best)
{
  PREFER(prod->id > 1, best->id <= 1);
  LESS(get_ainit(prod), get_ainit(best));
#ifdef DAE
  LESS(duration(prod), duration(best));
#endif
  return Equal;
}

static inline void update_cost_h1(Fluent *f, TimeVal cost)
{
  if (cost < get_finit(f)) {
    set_finit(f, cost);
    FOR(a, f->consumers) { set_aused(a, true);} EFOR; } 
}

static inline void compute_h1_cost_yahsp()
{
  bool loop = true;
  TimeVal cost;

  while (loop) {
    loop = false;
    FORMIN(a, actions, 2) {
      if (get_aused(a)) {
        set_aused(a, false);
        cost = COST(a);
        if (cost < get_ainit(a)) {
          loop = true;
          set_ainit(a, cost);
          if (cost == 0) applicable[applicable_nb++] = a;
          INCCOST(cost, a);
          FOR(f, a->add) { update_cost_h1(f, cost); } EFOR;
        }
      }
    } EFOR; 
  }
}

void compute_h1(Node *node)
{
  applicable_nb = 0;
#ifdef DAE
  if (heuristic_search(node, finit)) {
    set_ainit(end_action, COST(end_action));
    if (get_ainit(end_action) != MAXTIME) {
      FORMIN(a, actions, 2) {
	set_ainit(a, COST(a));
	if (get_ainit(a) == 0) applicable[applicable_nb++] = a;
      } EFOR;
    }
    return;
  }
#endif
  FOR(a, actions) {
    set_ainit(a, MAXTIME);
    set_aused(a, a->prec_nb == 0);
  } EFOR;
  FOR(f, fluents) {
    set_finit(f, MAXTIME);
    if (state_contains(node->state, f)) update_cost_h1(f, 0);
  } EFOR;
  compute_h1_cost_yahsp();
  set_ainit(end_action, COST(end_action));
#ifdef DAE
  heuristic_insert(node, finit);
#endif
}

static inline Comparison cmp_actions(Action **a1, Action **a2)
{
  LESS(get_ainit(*a1), get_ainit(*a2));
  FOR(f, (*a1)->prec) { if (deletes(*a2, f)) return Better; } EFOR;
  FOR(f, (*a2)->prec) { if (deletes(*a1, f)) return Worse; } EFOR;
  return Equal;
}

void compute_relaxed_plan(Node *node)
{
  relaxed_plan[0] = end_action;
  relaxed_plan_nb = 1;
  FOR(a, actions) { set_aused(a, false); } EFOR;
  FOR(f, fluents) { set_finit(f, state_contains(node->state, f)); } EFOR;
  FOR(a, relaxed_plan) {
    FOR(f, a->prec) {
      if (!get_finit(f)) {
        set_finit(f, true);
        Action *best = NULL;
        long ties = 1;
        FOR(prod, f->producers) { 
	  if (!best || preferred(is_best_action_rp(prod, best), opt.random, ties)) best = prod; 
	} EFOR;
        if (best != NULL && !get_aused(best)) {
          set_aused(best, true);
          relaxed_plan[relaxed_plan_nb++] = best;
        }
      }
    } EFOR;
  } EFOR;
  vector_sort(relaxed_plan, cmp_actions);
}

static inline bool can_be_applied(Node *node, Action *a)
{
  FOR(f, a->prec) { if (!state_contains(node->state, f)) return false; } EFOR;
  return true;
}

bool action_must_precede(Action *a1, Action *a2)
{
  if (opt.sequential || (opt.fluent_mutexes && amutex(a1, a2))) return true;
  FOR(f, a1->del) { if (consumes(a2, f) || produces(a2, f)) return true; } EFOR;
  FOR(f, a2->del) { if (consumes(a1, f) || produces(a1, f)) return true; } EFOR;
  FOR(f, a2->prec) { if (produces(a1, f)) return true; } EFOR;
  return false;
}

bool actions_mutex(Action *a1, Action *a2)
{
  if (opt.sequential || (opt.fluent_mutexes && amutex(a1, a2))) return true;
  FOR(f, a1->del) { if (consumes(a2, f) || produces(a2, f)) return true; } EFOR;
  FOR(f, a2->del) { if (consumes(a1, f) || produces(a1, f)) return true; } EFOR;
  return false;
}


typedef struct Edge Edge;

typedef struct Vertice Vertice;

struct Edge {
  Vertice *src;
  Vertice *dst;
  ActivityConstraint *ac;
};

struct Vertice {
  Action *a;
  TimeVal est;
  TimeVal lst;
  VECTOR(Edge *, in);
  VECTOR(Edge *, out);
  bool prop;
};

void add_edge(Vertice *v1, Vertice *v2, Fluent *f)
{
  Edge *edge = cpt_calloc(edge, 1);
  edge->src = v1;
  edge->dst = v2;
  cpt_realloc(v1->out, v1->out_nb + 1);
  v1->out[v1->out_nb++] = edge;
  cpt_realloc(v2->in, v2->in_nb + 1);
  v2->in[v2->in_nb++] = edge;
  if (f != NULL) edge->ac = find_ac_constraint(edge->src->a, f);
}

static bool order(Edge *e)
{
  Vertice *s = e->src, *d = e->dst;
  if (e->ac != NULL) {
    TimeVal acmin = 0, acmax = 0;
    evaluate_ac_forward(e->ac, s->est + duration(s->a), s->lst + duration(s->a), &acmin, &acmax);
    if (acmin > d->est) {
      d->est = acmin;
      d->prop = true;
      if (d->est > d->lst) return false;
    }
    if (acmax < d->lst) {
      d->lst = acmax;
      d->prop = true;
      if (d->est > d->lst) return false;
    }
    evaluate_ac_backward(e->ac, d->est, d->lst, &acmin, &acmax);
    if (acmin - duration(s->a) > s->est) {
      s->est = acmin - duration(s->a);
      s->prop = true;
      if (s->est > s->lst) return false;
    }
    if (acmax - duration(s->a) < s->lst) {
      s->lst = acmax - duration(s->a);
      s->prop = true;
      if (s->est > s->lst) return false;
    }
  } else {
    if (s->est + duration(s->a) > d->est) {
      d->est = s->est + duration(s->a);
      d->prop = true;
      if (d->est > d->lst) return false;
    }
    if (d->lst - duration(s->a) < s->lst) {
      s->lst = d->lst - duration(s->a);
      s->prop = true;
      if (d->est > d->lst) return false;
    }
  }
  return true;
}

bool node_action_schedule(Node *node)
{
  size_t vertices_nb = node->length + 2;
  Vertice *vertices[vertices_nb];
  Node *tmp = node;
  size_t i = vertices_nb;
  FORi(v, i, vertices) { 
    v = cpt_calloc(vertices[i], 1);
    // cpt_malloc(v->in, vertices_nb*2);
    // cpt_malloc(v->out, vertices_nb*2);
    v->prop = true;
    v->lst = MAXTIME;
  } EFOR;
  vertices[--i]->a = end_action;
  vertices[0]->a = start_action;
  vertices[0]->lst = 0;
  while (tmp != NULL) {
    RFOR(s, tmp->steps) {
      vertices[--i]->a = s.action;
    } EFOR;
    tmp = tmp->parent;
  }
  // FOR(a, vertices) { cpt_trace(normal, "%s ", action_name(a->a)); } EFOR;
  // cpt_trace(normal, "\n\n");

  FORi(v1, i, vertices) {
    FOR(f, v1->a->prec) {
      RFORMAX(v2, vertices, i) {
	if (produces(v2->a, f)) {
	  add_edge(v2, v1, f);
	  break;
	}
      } EFOR;
    } EFOR;
    FORMAX(v2, vertices, i) {
      if (actions_mutex(v1->a, v2->a)) {
	add_edge(v2, v1, NULL); 
      }
    } EFOR;
  } EFOR;

  
  bool propagate = true;
  bool contradiction = false;

  while (propagate) {
    propagate = false;
    FOR(v, vertices) {
      if (v->prop) {
	propagate = true;
	v->prop = false;
	FOR(e, v->in) { if (!order(e)) goto contradiction; } EFOR;
	FOR(e, v->out) { if (!order(e)) goto contradiction; } EFOR;
      }
    } EFOR;
  }
  goto nocontradiction;

 contradiction:
  contradiction = true;
  
 nocontradiction:

  node->makespan = vertices[vertices_nb - 1]->est;
  if (!contradiction) {
    i = vertices_nb - 1;
    while (node != NULL) {
      RFORi(s, j, node->steps) {
	node->steps[j].init = vertices[--i]->est;
      } EFOR;
      node = node->parent;
    }

    // FOR(v, vertices) { 
    //   print_time(stdout, v->est);
    //   cpt_trace(normal, " ");
    //   print_time(stdout, v->lst);
    //   cpt_trace(normal, " %s\n", action_name(v->a));
    // } EFOR;
    // cpt_trace(normal, "\n\n");
  }

  FOR(v, vertices) { 
    FOR(e, v->out) { cpt_free(e); } EFOR;
    cpt_free(v->out);
    cpt_free(v->in);
    cpt_free(v);
  } EFOR;

  return !contradiction;
}

bool node_apply_action(Node *node, Action *a) 
{
  FOR(f, a->del) { state_del(node->state, f); } EFOR;
  FOR(f, a->add) { state_add(node->state, f); } EFOR;
  TimeVal init = 0;
  if (opt.sequential) {
    init = node->length;
    node->makespan += duration(a);
  } else {
    Node *tmp = node;
    while (init < tmp->makespan) {
      RFOR(s, tmp->steps) {
	if (s.init + duration(s.action) <= init) continue;
	if (action_must_precede(s.action, a)) {
	  init = s.init + duration(s.action);
	  if (init >= tmp->makespan) goto end;
	}
      } EFOR;
      tmp = tmp->parent;
    }
  end:
    maximize(node->makespan, init + duration(a));
  }
#ifdef YAHSP_MPI
  node->steps[node->steps_nb].action = (Action *) a->id;
#else
  node->steps[node->steps_nb].action = a;
#endif
  node->steps[node->steps_nb].init = init;
  node->steps_nb++;
  node->length++;
  return node_action_schedule(node);
}

Node *apply_relaxed_plan(Node *node, TimeVal best_makespan)
{ 
  Node *son = node_derive(node);

  son->steps = cpt_malloc(son->steps, relaxed_plan_nb);
  son->steps_nb = 0;

 start:
  FORi(a, i, relaxed_plan) {
    if (a != NULL && a != end_action && can_be_applied(son, a)) {
      if (node_apply_action(son, a)) {
	relaxed_plan[i] = NULL;
	if (son->makespan > best_makespan) { node_free(son); return NULL; }
	goto start;
      }
    }
  } EFOR;
  /* FORi(a, i, relaxed_plan) { */
  /*   if (a != NULL && a != end_action) { */
  /*     Fluent *unsat = NULL; */
  /*     FOR(f, a->prec) { */
  /* 	if (!bitarray_get(son->state, f)) { */
  /* 	  if (unsat != NULL) goto next_action; */
  /* 	  unsat = f; */
  /* 	} */
  /*     } EFOR; */
  /*     if (!unsat) exit(55); */
  /*     Action *best = NULL; */
  /*     long ties = 1; */
  /*     FOR(prod, unsat->producers) { */
  /* 	if (prod != a && can_be_applied(son, prod) && prod->id > 1 */
  /* 	    && (!best || preferred(is_best_action_rp(prod, best), opt.random, ties))) { */
  /* 	  FOR(f, a->prec) { if (deletes(prod, f)) goto next_prod; } EFOR; */
  /* 	  best = prod; */
  /* 	} */
  /*     next_prod:; */
  /*     } EFOR; */
  /*     if (best != NULL) { */
  /* 	if (son->steps_nb > relaxed_plan_nb*2) exit(55); */
  /* 	//trace(normal, "%s\n", action_name(best)); */
  /*     	node_apply_action(son, best); */
  /*     	if (son->makespan > best_makespan) { node_free(son); return NULL; } */
  /*     	goto start; */
  /*     } */
  /*   } */
  /*   next_action:; */
  /* } EFOR; */
  FORi(a, i, relaxed_plan) {
    if (a == NULL || a == end_action) continue;
    FOR(b, relaxed_plan) {
      if (b == NULL || b == a || b == end_action) continue;
      FOR(f, a->add) {
#ifdef DAE
	// voluntary bug!!
	if (!state_contains(node->state, f) && consumes(b, f))
#else
	if (!state_contains(son->state, f) && consumes(b, f)) 
#endif
	  {
	    Action *best = NULL;
	    long ties = 1;
	    FOR(prod, f->producers) {
	      if (prod != a && prod != b && can_be_applied(son, prod) && prod->id > 1
		  && (!best || preferred(is_best_action_rp(prod, best), opt.random, ties))) best = prod; 
	    } EFOR;
	    if (best != NULL) {
	      relaxed_plan[i] = best;
	      goto start;
	    }
	  }
      } EFOR;
    } EFOR;
  } EFOR;
  cpt_realloc(son->steps, son->steps_nb);
  return son;
}

SolutionPlan *create_solution_plan(Node *node)
{
  SolutionPlan *plan = cpt_calloc(plan, 1);
  long length = node->length;
  cpt_malloc(plan->steps, (plan->steps_nb = length));
  plan->makespan = node->makespan;
  plan->length = length;
  plan->backtracks = stats.evaluated_nodes;
  while (node != NULL) {
    RFOR(a, node->steps) {
      Step *s = cpt_calloc(plan->steps[--length], 1);
      s->action = a.action;
      s->init = a.init;
    } EFOR;
    node = node->parent;
  }
  vector_sort(plan->steps, precedes_in_plan);
#ifdef DAE
  FOR(s, plan->steps) { 
    plan->cost_add += s->action->cost; 
    maximize(plan->cost_max, s->action->cost);
  } EFOR;
#endif
  return plan;
}

void yahsp_trace_anytime(Node *node)
{
#if defined YAHSP_MT || defined YAHSP_MPI
  trace_proc(yahsp_anytime_search_stats, node->makespan, get_wtimer(stats.search), get_wtimer(stats.total));
#else
  trace_proc(yahsp_anytime_search_stats, node->makespan, get_timer(stats.search), get_timer(stats.total));
#endif
  if (opt.output_file != NULL) {
    SolutionPlan *plan = create_solution_plan(node);
    print_plan_ipc_anytime(plan);
    plan_free(plan);
  }
}

int yahsp_compress_plans()
{
  SolutionPlan *plan = cpt_calloc(plan, 1);

  FOR(p, plans) { plan->steps_nb += p->steps_nb; } EFOR;
  cpt_malloc(plan->steps, plan->steps_nb);
  plan->steps_nb = 0;
  FOR(p, plans) {
    FOR(s, p->steps) {
      *cpt_calloc(plan->steps[plan->steps_nb], 1) = *s;
      s = plan->steps[plan->steps_nb++];
      s->init = 0;
      RFOR(s2, plan->steps) {
        if (action_must_precede(s2->action, s->action)) {
          maximize(s->init, s2->end);
          if (s->init == plan->makespan) break;
        }
      } EFOR;
      s->end = s->init + duration(s->action);
      maximize(plan->makespan, s->end);
    } EFOR;
  } EFOR;
  vector_sort(plan->steps, precedes_in_plan);
  solution_plan = plan;
#ifdef DAE
  FOR(s, plan->steps) { 
    plan->cost_add += s->action->cost; 
    maximize(plan->cost_max, s->action->cost);
  } EFOR;
#endif
  return PLAN_FOUND;
}
