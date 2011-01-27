/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : yahsp-mt.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */

#include "cpt.h"
#include "trace.h"
#include "structs.h"
#include "problem.h"
#include "max_atom.h"
#include "plan.h"
#include "comparison.h"
#include "globs.h"
#include "trace_planner.h"


typedef struct Node Node;
typedef struct YStep YStep;

struct YStep {
  Action *action;
  TimeVal init;
};

struct Node {
  long id;
  Node *ancestor;
  unsigned long key;
  BitArray state;
  long fvalue;
  long length;
  TimeVal makespan;
  VECTOR(YStep, steps);
  VECTOR(Action *, applicable);
};

typedef struct Heuristic Heuristic;

struct Heuristic {
  unsigned long key;
  BitArray state;
  TimeVal *inits;
};


static TimeVal *ainit;
static TimeVal *aused;
static TimeVal *finit;

SVECTOR(Action *, relaxed_plan);
SVECTOR(Action *, applicable);

static gdsl_rbtree_t *open_lists;
static omp_lock_t *open_locks;
static gdsl_rbtree_t *closed_lists;
static omp_lock_t *closed_locks;
static gdsl_rbtree_t open_list;
static omp_lock_t *open_lock;
static gdsl_rbtree_t closed_list;
static omp_lock_t *closed_lock;
static gdsl_rbtree_t heuristics;
static omp_lock_t heuristics_lock;

static BitArray current_state;
static BitArray initial_bitstate;

static TimeVal best_makespan = MAXTIME;

static bool plan_found = false;
static long nodes_bound;

static long threads_count[] = { 1, 4, 8, 16, 32, 64 };
static long nodes_limits[] = { 50, 400, 3000, 20000, 100000, LONG_MAX };
static long nodes_limits_nb = 6;

/* static long threads_count[] = { 64 }; */
/* static long nodes_limits[] = { LONG_MAX }; */
/* static long nodes_limits_nb = 1; */

#pragma omp threadprivate(ainit, aused, finit, relaxed_plan, relaxed_plan_nb, applicable, applicable_nb, open_list, open_lock, closed_list, closed_lock)

#define get_ainit(a) ainit[(a)->id]
#define set_ainit(a, t) ainit[(a)->id] = t
#define get_aused(a) aused[(a)->id]
#define set_aused(a, t) aused[(a)->id] = t
#define get_finit(f) finit[(f)->id]
#define set_finit(f, t) finit[(f)->id] = t

#undef check_allocation
#define check_allocation(ptr, x, res) ({ if (x > 0) { if (!(ptr = (typeof(ptr)) res) && opt.dae && ({ gdsl_rbtree_flush(heuristics); !(ptr = (typeof(ptr)) res); })) error(allocation, "Memory allocation error"); } else ptr = NULL; ptr; })


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

static Comparison is_best_action_rp(Action *prod, Action *best)
{
  PREFER(prod->id > 1, best->id <= 1);
  LESS(get_ainit(prod), get_ainit(best));
#ifdef DAE
  LESS(duration(prod), duration(best));
#endif
  return Equal;
}

void node_free(Node *node) 
{ 
  cpt_free(node->state);
  cpt_free(node->steps); 
  cpt_free(node->applicable);
  cpt_free(node);
}

static Comparison open_list_cmp(Node *node1, Node *node2)
{
  LESS(node1->fvalue, node2->fvalue);
#ifdef DAE
  LESS(node1->makespan, node2->makespan);
#else
  LESS(node1->length, node2->length);
#endif
  switch (opt.yahsp_strategy) {
  case 0: 
    LESS(node1->id, node2->id);
    return Equal;
  case 1: 
    return Equal;
  default:
    if (omp_get_thread_num() % 2 == 1) LESS(node1->id, node2->id);
    return Equal;
  }
}

static Comparison closed_list_cmp(Node *node1, Node *node2)
{
  LESS(node1->key, node2->key);
  return (Comparison) bitarray_cmp(node1->state, node2->state, fluents_nb);
}

static Node *open_list_insert(Node *node)
{
  int gdsl_return;
  Node *n;
  omp_set_lock(open_lock);
  n = (Node *) gdsl_rbtree_insert(open_list, node, &gdsl_return);
  omp_unset_lock(open_lock);
  return n;
}

static Node *closed_list_insert(Node *node)
{ 
  int gdsl_return;
  ulong i;
  for (i = 0; i < (ulong) (fluents_nb -1) / __WORDSIZE + 1; i++) 
    node->key ^= node->state[i] * (i + 1);
  omp_set_lock(closed_lock);
  gdsl_rbtree_insert(closed_list, node, &gdsl_return);
  omp_unset_lock(closed_lock);
  return gdsl_return == GDSL_INSERTED ? node : ({ node_free(node); (Node *) NULL; });
}

static void update_cost_h1(Fluent *f, TimeVal cost)
{
  if (cost < get_finit(f)) {
    set_finit(f, cost);
    FOR(a, f->consumers) { set_aused(a, true);} EFOR; } 
}

static void compute_h1_cost_yahsp()
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

static void heuristic_insert(Node *node)
{
  Heuristic *h = cpt_malloc(h, 1);
  int gdsl_return;
  h->key = node->key;
  h->state = bitarray_create(fluents_nb);
  bitarray_copy(h->state, node->state, fluents_nb);
  cpt_malloc(h->inits, fluents_nb);
  memcpy(h->inits, finit, fluents_nb * sizeof(TimeVal));
  omp_set_lock(&heuristics_lock);
  gdsl_rbtree_insert(heuristics, h, &gdsl_return);
  omp_unset_lock(&heuristics_lock);
}

static bool heuristic_search(Node *node)
{
  Heuristic test;
  test.key = node->key;
  test.state = node->state;
  omp_set_lock(&heuristics_lock);
  Heuristic *h = (Heuristic *) gdsl_rbtree_search(heuristics, (gdsl_compare_func_t) heuristic_cmp, &test);
  omp_unset_lock(&heuristics_lock);
  if (h == NULL) return false;
  memcpy(finit, h->inits, fluents_nb * sizeof(TimeVal));
  set_ainit(end_action, COST(end_action));
  if (get_ainit(end_action) != MAXTIME) {
    FORMIN(a, actions, 2) {
      set_ainit(a, COST(a));
      if (get_ainit(a) == 0) applicable[applicable_nb++] = a;
    } EFOR;
  }
  return true;
}

static void compute_h1(Node *node)
{
  applicable_nb = 0;
  if (opt.dae && heuristic_search(node)) return;
  FOR(a, actions) {
    set_ainit(a, MAXTIME);
    set_aused(a, a->prec_nb == 0);
  } EFOR;
  FOR(f, fluents) {
    set_finit(f, MAXTIME);
    if (bitarray_get(node->state, f)) update_cost_h1(f, 0);
  } EFOR;
  compute_h1_cost_yahsp();
  set_ainit(end_action, COST(end_action));
  if (opt.dae) heuristic_insert(node);
}


static Comparison cmp_actions(const void *s1, const void *s2)
{
  Action *a1 = *((Action **) s1);
  Action *a2 = *((Action **) s2);
  LESS(get_ainit(a1), get_ainit(a2));
  FOR(f, a1->prec) { if (deletes(a2, f)) return Better; } EFOR;
  FOR(f, a2->prec) { if (deletes(a1, f)) return Worse; } EFOR;
  return Equal;
}

static void compute_relaxed_plan(Node *node)
{
  relaxed_plan[0] = end_action;
  relaxed_plan_nb = 1;
  FOR(a, actions) { set_aused(a, false); } EFOR;
  FOR(f, fluents) { set_finit(f, bitarray_get(node->state, f)); } EFOR;
  FOR(a, relaxed_plan) {
    FOR(f, a->prec) {
      if (!get_finit(f)) {
        set_finit(f, true);
        Action *best = NULL;
        long ties = 1;
        FOR(prod, f->producers) { 
	  if (!best || preferred(is_best_action_rp(prod, best), ties)) best = prod; 
	} EFOR;
        if (best != NULL && !get_aused(best)) {
          set_aused(best, true);
          relaxed_plan[relaxed_plan_nb++] = best;
        }
      }
    } EFOR;
  } EFOR;
  qsort(relaxed_plan, relaxed_plan_nb, sizeof(Action *), (int (*)(const void*, const void*)) cmp_actions);
}

static bool can_be_applied(Node *node, Action *a)
{
  FOR(f, a->prec) { if (!bitarray_get(node->state, f)) return false; } EFOR;
  return true;
}

static bool action_must_precede(Action *a1, Action *a2)
{
  if (opt.sequential || (opt.fluent_mutexes && amutex(a1, a2))) return true;
  FOR(f, a1->del) { if (consumes(a2, f) || produces(a2, f)) return true; } EFOR;
  FOR(f, a2->del) { if (consumes(a1, f) || produces(a1, f)) return true; } EFOR;
  FOR(f, a2->prec) { if (produces(a1, f)) return true; } EFOR;
  return false;
}

static void node_apply_action(Node *node, Action *a) 
{
  FOR(f, a->del) { bitarray_unset(node->state, f); } EFOR;
  FOR(f, a->add) { bitarray_set(node->state, f); } EFOR;
  node->length++;
  TimeVal init = 0;
  Node *tmp = node;
  while (init < tmp->makespan) {
    RFOR(s, tmp->steps) {
      if (s.init + duration(s.action) <= init) continue;
      if (action_must_precede(s.action, a)) {
        init = s.init + duration(s.action);
        if (init >= tmp->makespan) goto end;
      }
    } EFOR;
    tmp = tmp->ancestor;
  }
 end:
  node->steps[node->steps_nb].action = a;
  node->steps[node->steps_nb].init = init;
  node->steps_nb++;
  maximize(node->makespan, init + duration(a));
}

static Node *node_derive(Node *node)
{
  Node *son = cpt_calloc(son, 1);
#pragma omp critical
  son->id = stats.computed_nodes++;
  son->ancestor = node;
  son->length = node->length;
  son->makespan = node->makespan;
  son->state = bitarray_create(fluents_nb);
  bitarray_copy(son->state, node->state, fluents_nb);
  return son;
}


static Node *apply_relaxed_plan(Node *node)
{
  Node *son = node_derive(node);

  son->steps = cpt_malloc(son->steps, relaxed_plan_nb);
  son->steps_nb = 0;

 start:
  FORi(a, i, relaxed_plan) {
    if (a != NULL && a != end_action && can_be_applied(son, a)) {
      relaxed_plan[i] = NULL;
      node_apply_action(son, a);
      if (son->makespan > best_makespan) { node_free(son); return NULL; }
      goto start;
    }
  } EFOR;
  FORi(a, i, relaxed_plan) {
    if (a == NULL || a == end_action) continue;
    FOR(b, relaxed_plan) {
      if (b == NULL || b == a || b == end_action) continue;
      FOR(f, a->add) {
	if (!bitarray_get(son->state, f) && consumes(b, f)) {
	  Action *best = NULL;
	  long ties = 1;
	  FOR(prod, f->producers) {
	    if (prod != a && prod != b && can_be_applied(son, prod) && prod->id > 1
		&& (!best || preferred(is_best_action_rp(prod, best), ties))) best = prod; 
	  } EFOR;
	  if (best != NULL) {
	    relaxed_plan[i] = best;
	    goto start;
	  }
	}
      } EFOR;
    } EFOR;
  } EFOR;
  if (son->steps_nb > 0) cpt_realloc(son->steps, son->steps_nb);
  return son;
}

static void create_solution_plan(Node *node)
{
  SolutionPlan *plan = cpt_calloc(plan, 1);

  cpt_malloc(plan->steps, (plan->steps_nb = node->length));
  plan->makespan = node->makespan;

  while (node != NULL) {
    FORi(a, i, node->steps) {
      Step *s = cpt_calloc(plan->steps[node->length - node->steps_nb + i], 1);
      s->action = a.action;
      s->init = a.init;
    } EFOR;
    node = node->ancestor;
  }

  qsort(plan->steps, plan->steps_nb, sizeof(Step *), precedes_in_plan);
  plan->backtracks = stats.evaluated_nodes;

  solution_plan = plan;
}

bool test_finished()
{ return plan_found || stats.evaluated_nodes >= nodes_bound; }

static Node *compute_node(Node *node)
{
  if (node == NULL || closed_list_insert(node) == NULL || test_finished() || stats.evaluated_nodes >= opt.max_backtracks) return NULL;
  if (node->makespan >= best_makespan) return NULL;
#pragma omp atomic
  stats.evaluated_nodes++;
  compute_h1(node);
  if (get_ainit(end_action) == MAXTIME) return NULL;
  if (get_ainit(end_action) == 0) {
    if (!opt.anytime) return node;
    if (node->makespan < best_makespan) {
#pragma omp critical
      {
	if (node->makespan < best_makespan) {
	  best_makespan = node->makespan;
	  trace_proc(yahsp_anytime_search_stats, 
		     node->makespan, omp_get_wtime() - stats.wcsearch, omp_get_wtime() - stats.wctime);
	  create_solution_plan(node);
	  print_plan_ipc_anytime(solution_plan);
	  plan_free(solution_plan);
	}
      }
      return NULL;
    }
  }
  vector_copy(node->applicable, applicable);
  compute_relaxed_plan(node);
  node->fvalue = NODE_FVALUE(node);
  return compute_node(apply_relaxed_plan(open_list_insert(node)));
}

static int open_list_map(Node *node, gdsl_location_t loc, void *data) 
{
  if (*((Node **) data) == NULL) *((Node **) data) = node;
  return GDSL_MAP_STOP;
}

long working_threads;

static Node *open_list_pop_best()
{
  Node *node = NULL;
  omp_set_lock(open_lock);
  gdsl_rbtree_map_infix(open_list, (gdsl_map_func_t) open_list_map, &node);
  if (node != NULL) { 
    gdsl_rbtree_remove(open_list, node);
#pragma omp atomic
    working_threads++;
  }
  omp_unset_lock(open_lock);
  return node;
}

static Node *yahsp_plan()
{ 
  Node *node, *son = NULL;
  bool open_empty = false;
  plan_found = false;
  working_threads = 0;
  if (omp_get_thread_num() < opt.yahsp_teams) {
    cpt_calloc(node, 1);
    node->state = bitarray_create(fluents_nb);
    bitarray_copy(node->state, current_state, fluents_nb);
    son = compute_node(node);
  }
#pragma omp barrier
  if (son != NULL) return son;
  while (!open_empty && !test_finished()) {
    if ((node = open_list_pop_best())) {
#pragma omp atomic
      stats.expanded_nodes++;
      FOR(a, node->applicable) {
        son = node_derive(node);
        cpt_malloc(son->steps, 1);
        node_apply_action(son, a);
        if ((son = compute_node(son))) { 
#pragma omp atomic
	  working_threads--; 
	  return son; 
	}
        if (test_finished() || stats.evaluated_nodes >= opt.max_backtracks) { 
#pragma omp atomic
	  working_threads--; 
	  return NULL; 
	}
      } EFOR;
      cpt_free(node->applicable);
#pragma omp atomic
      working_threads--;
    }
#pragma omp critical
    if (working_threads == 0 && gdsl_rbtree_is_empty(open_list)) open_empty = true;
  }
  return NULL;
}

void yahsp_reset()
{
  cpt_free(current_state);
  current_state = bitarray_create(fluents_nb);
  bitarray_copy(current_state, initial_bitstate, fluents_nb);
}

void yahsp_init()
{
  long i;
  compute_init_rh1_cost();
  cpt_malloc(open_lists, opt.yahsp_teams);
  cpt_malloc(open_locks, opt.yahsp_teams);
  cpt_malloc(closed_lists, opt.yahsp_teams);
  cpt_malloc(closed_locks, opt.yahsp_teams);
  for(i = 0; i < opt.yahsp_teams; i++) {
    open_lists[i] = gdsl_rbtree_alloc(NULL, NULL, NULL, (gdsl_compare_func_t) open_list_cmp);
    omp_init_lock(&open_locks[i]);
    closed_lists[i] = gdsl_rbtree_alloc(NULL, NULL, (gdsl_free_func_t) node_free, (gdsl_compare_func_t) closed_list_cmp);
    omp_init_lock(&closed_locks[i]);
  }
  omp_init_lock(&heuristics_lock);
  if (opt.dae) heuristics = gdsl_rbtree_alloc(NULL, NULL, (gdsl_free_func_t) heuristic_free, (gdsl_compare_func_t) heuristic_cmp);
  initial_bitstate = bitarray_create(fluents_nb);
  FOR(f, init_state) { bitarray_set(initial_bitstate, f); } EFOR;
  yahsp_reset();
  srand(opt.seed);
}

int yahsp_main() 
{
  Node *node = NULL;
  long num_threads;

  stats.computed_nodes = 0;
  stats.evaluated_nodes = 0;
  stats.expanded_nodes = 0;

  FORi(nodes_limit, i, nodes_limits) {
    num_threads = mini(opt.yahsp_threads, threads_count[i]);
    //num_threads = threads_count[i];
    nodes_bound = num_threads == opt.yahsp_threads ? LONG_MAX : nodes_limit;
    //trace(normal, "#threads : %ld -- already evaluated nodes : %ld\n", num_threads, stats.evaluated_nodes);
#pragma omp parallel num_threads(num_threads * opt.yahsp_teams)
    {
      cpt_malloc(aused, actions_nb);
      cpt_malloc(ainit, actions_nb);
      cpt_malloc(finit, fluents_nb);
      cpt_malloc(relaxed_plan, actions_nb);
      cpt_malloc(applicable, actions_nb);
      open_list = open_lists[omp_get_thread_num() % opt.yahsp_teams];
      open_lock = &open_locks[omp_get_thread_num() % opt.yahsp_teams];
      closed_list = closed_lists[omp_get_thread_num() % opt.yahsp_teams];
      closed_lock = &closed_locks[omp_get_thread_num() % opt.yahsp_teams];
      Node *n = yahsp_plan();
      cpt_free(aused);
      cpt_free(ainit);
      cpt_free(finit);
      cpt_free(relaxed_plan);
      cpt_free(applicable);

      if (n != NULL) {
#pragma omp critical
	if (node == NULL || n->makespan < node->makespan) {
	  node = n;
	  plan_found = true;
	}
      }
    }
    if (plan_found || stats.evaluated_nodes >= opt.max_backtracks) break;
    /* long i; */
    /* for(i = 0; i < opt.yahsp_teams; i++) { */
    /*   gdsl_rbtree_flush(open_lists[i]); */
    /*   gdsl_rbtree_flush(closed_lists[i]); */
    /* } */
  } EFOR;

  if (node == NULL) {
    current_state = NULL;
    long i;
    for(i = 0; i < opt.yahsp_teams; i++) {
      gdsl_rbtree_flush(open_lists[i]);
      gdsl_rbtree_flush(closed_lists[i]);
    }
    return NO_PLAN;
  }

  bitarray_copy(current_state, node->state, fluents_nb);

  create_solution_plan(node);

  long i;
  for(i = 0; i < opt.yahsp_teams; i++) {
    gdsl_rbtree_flush(open_lists[i]);
    gdsl_rbtree_flush(closed_lists[i]);
  }

  return PLAN_FOUND;
}

int yahsp_compress_plans()
{
  SolutionPlan *plan = cpt_calloc(plan, 1);

  FOR(p, plans) { plan->steps_nb += p->steps_nb; } EFOR;
  cpt_malloc(plan->steps, plan->steps_nb);
  plan->steps_nb = 0;
  FOR(p, plans) {
    FOR(s, p->steps) {
      *cpt_calloc(plan->steps[plan->steps_nb++], 1) = *s;
      s = plan->steps[plan->steps_nb - 1];
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
  qsort(plan->steps, plan->steps_nb, sizeof(Step *), precedes_in_plan);
  solution_plan = plan;
  return PLAN_FOUND;
}

/* Ajouté par Pierre Savéant et Johann Dréo le 2010-03-26 */
BitArray * get_current_state()
{
    /*
    unsigned int i = 0;

    printf( "current_state= " );
    for( i = 0; i < fluents_nb; i++ ) {
        printf( "%i ", ( bitarray_get( current_state, fluents[i] ) ? 1 : 0 ) );
    }
    printf("\n");
    */

    return & current_state;
}
/* PS & JD */
