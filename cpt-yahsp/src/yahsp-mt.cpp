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
#include "trace_planner.h"
#include "globs.h"
// DIRTY
#define end_action (&yend_action)
#include "yahsp.h"
#ifdef DAE
#include "yahsp-dae.h"
#endif
#ifdef YAHSP_MPI
#include "yahsp-mpi.h"
#endif


long yahsp_max_evaluated_nodes;

static long yahsp_state_size;
static long node_id_counter;


static TimeVal *ainit;
static TimeVal *aused;
static TimeVal *finit;

SVECTOR(Action *, relaxed_plan);
SVECTOR(Action *, applicable);

static gdsl_rbtree_t *open_lists;
static gdsl_rbtree_t *closed_lists;
static gdsl_rbtree_t open_list;
static gdsl_rbtree_t closed_list;
#ifdef YAHSP_MT
static omp_lock_t *open_locks;
static omp_lock_t *closed_locks;
static omp_lock_t *open_lock;
static omp_lock_t *closed_lock;
#endif

static State current_state;
static State initial_bitstate;

static TimeVal best_makespan = MAXTIME;

static bool stop_search;
static long nodes_limit;
static long working_threads;


typedef struct NodeLimit Nodelimit;

struct NodeLimit {
  long nb_threads;
  long max_nodes;
};

#ifdef YAHSP_MT
static NodeLimit node_limits[] = { {1, 50}, {4, 400}, {8, 3000}, {16, 20000}, {32, 100000}, {64, LONG_MAX} };
static long node_limits_nb = 6;
#else
static NodeLimit node_limits[] = { {1, LONG_MAX} };
static long node_limits_nb = 1;
#endif

#ifdef YAHSP_MT
#pragma omp threadprivate(ainit, aused, finit, relaxed_plan, relaxed_plan_nb, applicable, applicable_nb, open_list, closed_list)
#pragma omp threadprivate(open_lock, closed_lock)
#else
#ifdef DAE
#pragma omp threadprivate(ainit, aused, finit, relaxed_plan, relaxed_plan_nb, applicable, applicable_nb, open_list, closed_list, open_lists, closed_lists, current_state, initial_bitstate, best_makespan, stop_search, nodes_limit, working_threads, node_id_counter)
#endif
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

static void node_free(Node *node) 
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
#ifdef YAHSP_MT
    if (omp_get_thread_num() % 2 == 1) 
#endif
    LESS(node1->id, node2->id);
    return Equal;
  }
}

static Comparison closed_list_cmp(Node *node1, Node *node2)
{
  LESS(node1->key, node2->key);
  return (Comparison) state_cmp(node1->state, node2->state);
}

static void open_list_insert(Node *node)
{
  int gdsl_return;
#ifdef YAHSP_MT
  omp_set_lock(open_lock);
#endif
  gdsl_rbtree_insert(open_list, node, &gdsl_return);
#ifdef YAHSP_MT
  omp_unset_lock(open_lock);
#endif
}

static Node *closed_list_insert(Node *node)
{ 
  int gdsl_return;
#ifdef YAHSP_MT
  omp_set_lock(closed_lock);
#endif
  gdsl_rbtree_insert(closed_list, node, &gdsl_return);
#ifdef YAHSP_MT
  omp_unset_lock(closed_lock);
#endif
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

static void compute_h1(Node *node)
{
  applicable_nb = 0;
#ifdef DAE
  if (heuristic_search(node, ainit, finit, applicable, &applicable_nb)) return;
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

static Comparison cmp_actions(Action **a1, Action **a2)
{
  LESS(get_ainit(*a1), get_ainit(*a2));
  FOR(f, (*a1)->prec) { if (deletes(*a2, f)) return Better; } EFOR;
  FOR(f, (*a2)->prec) { if (deletes(*a1, f)) return Worse; } EFOR;
  return Equal;
}

static void compute_relaxed_plan(Node *node)
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
	  if (!best || preferred(is_best_action_rp(prod, best), ties)) best = prod; 
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

static bool can_be_applied(Node *node, Action *a)
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

static void node_apply_action(Node *node, Action *a) 
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
  end:;
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
}


static Node *create_node(State state)
{
  Node *node = cpt_calloc(node, 1);
  state_clone(node->state, state);
#ifdef YAHSP_MT
#pragma omp critical
#endif
  node->id = node_id_counter++;
#ifdef YAHSP_MT
#pragma omp atomic
#endif
  stats.computed_nodes++;
  return node;
}

static Node *node_derive(Node *node)
{
  Node *son = create_node(node->state);
  son->parent = node;
  son->length = node->length;
  son->makespan = node->makespan;
#ifdef YAHSP_MPI
  son->fvalue = node->fvalue;
#endif
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
  /* 	    && (!best || preferred(is_best_action_rp(prod, best), ties))) { */
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
	if (!state_contains(node->state, f) && consumes(b, f)) {
#else
	if (!state_contains(son->state, f) && consumes(b, f)) {
#endif
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
  cpt_realloc(son->steps, son->steps_nb);
  return son;
}

static SolutionPlan *create_solution_plan(Node *node)
{
  SolutionPlan *plan = cpt_calloc(plan, 1);
  long length = node->length;
  cpt_malloc(plan->steps, (plan->steps_nb = length));
  plan->makespan = node->makespan;
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
  return plan;
}

static void yahsp_trace_anytime(Node *node)
{
  trace_proc(yahsp_anytime_search_stats,
	     node->makespan, get_wtimer(stats.search), get_wtimer(stats.total));
  if (opt.output_file != NULL) {
    SolutionPlan *plan = create_solution_plan(node);
    print_plan_ipc_anytime(plan);
    plan_free(plan);
  }
}

static bool test_stop_search()
{ return stop_search || stats.evaluated_nodes >= nodes_limit; }

static void compute_key(Node *node)
{
  long i;
  for (i = 0; i < yahsp_state_size; i++) 
    node->key ^= node->state[i] * (i + 1);
}

static Node *compute_node(Node *node);

#ifdef YAHSP_MPI
static Node *compute_or_send_node(Node *node)
{
  mpi_send_requested_node();
  if ((stop_search = mpi_recv_stop_search())) return NULL;
  if (node == NULL) return NULL;
  compute_key(node);
  if (!mpi_send_node(node))
    return compute_node(node);
  node_free(node);
  return NULL;
}
#else
static Node *compute_or_send_node(Node *node)
{
  if (node == NULL) return NULL;
  compute_key(node);
  return compute_node(node);
}
#endif

static Node *compute_node_no_closed_check(Node *node)
{
  if (test_stop_search()) return NULL;
#ifdef YAHSP_MPI
  minimize(best_makespan, mpi_recv_best_makespan());
#endif
  if (node->makespan >= best_makespan) return NULL;
#ifdef YAHSP_MT
#pragma omp atomic
#endif
  stats.evaluated_nodes++;
  compute_h1(node);
  if (get_ainit(end_action) == MAXTIME) return NULL;
  if (get_ainit(end_action) == 0) {
#ifndef YAHSP_MPI    
    if (!opt.anytime) return node;
#endif
    if (node->makespan < best_makespan) {
#ifdef YAHSP_MT
#pragma omp critical
#endif
      {
	if (node->makespan < best_makespan) {
	  best_makespan = node->makespan;
#ifdef YAHSP_MPI
	  mpi_send_goal_node(node);
#else
	  yahsp_trace_anytime(node);
#endif
	}
      }
      return NULL;
    }
    return NULL;
  }
  vector_copy(node->applicable, applicable);
  compute_relaxed_plan(node);
  node->fvalue = NODE_FVALUE(node);
  open_list_insert(node);
  return compute_or_send_node(apply_relaxed_plan(node));
}

static Node *compute_node(Node *node)
{
  if (closed_list_insert(node) == NULL) return NULL;
  else return compute_node_no_closed_check(node);
}

static int open_list_map(Node *node, gdsl_location_t loc, Node **data) 
{
  if (*data == NULL) *data = node;
  return GDSL_MAP_STOP;
}

static Node *open_list_pop_best()
{
  Node *node = NULL;
#ifdef YAHSP_MT
  omp_set_lock(open_lock);
#endif
  gdsl_rbtree_map_infix(open_list, (gdsl_map_func_t) open_list_map, &node);
  if (node != NULL) gdsl_rbtree_remove(open_list, node);
#ifdef YAHSP_MT
  omp_unset_lock(open_lock);
#endif
  return node;
}


static Node *compute_initial_node()
{
  return compute_or_send_node(create_node(current_state));
}

#ifdef YAHSP_MPI
static Node *yahsp_plan_master()
{
  Node *node = compute_initial_node();
  do {
    node = mpi_recv_goal_node();
    if (node->makespan >= best_makespan) continue;
    if (!opt.anytime || opt.output_file != NULL) mpi_gather_path(node);
    if (opt.anytime) {
      best_makespan = node->makespan;
      mpi_send_best_makespan(best_makespan);
      yahsp_trace_anytime(node);
    }
  } while (opt.anytime);
  mpi_send_stop_search();
  return node;
}
#endif

static Node *yahsp_plan()
{ 
  Node *node, *son = NULL;
  bool open_empty = false;
  bool started = false;

  stop_search = false;

#ifndef YAHSP_MPI
#ifdef YAHSP_MT
  if (omp_get_thread_num() < opt.yahsp_teams)
#endif
  son = compute_initial_node();
#endif
  son = compute_initial_node();

#ifdef YAHSP_MT
#pragma omp barrier
#endif
  if (son != NULL) return son;

#ifdef YAHSP_MPI
  while (!test_stop_search()) {
    stop_search = mpi_recv_stop_search();
    if (stop_search) return NULL;
    mpi_send_requested_node();
    while ((node = mpi_recv_node())) {
      started = true;
      node->id = node_id_counter++;
      if (closed_list_insert(node) != NULL) open_list_insert(node);
    }
#else
  while (!open_empty && !test_stop_search()) {
#endif
    if ((node = open_list_pop_best())) {
      started = true;
#ifdef YAHSP_MT
#pragma omp atomic
#endif
      working_threads++;
#ifdef YAHSP_MPI
      if (node->applicable == NULL) {
	son = compute_node_no_closed_check(node);
	if (son != NULL) {
#ifdef YAHSP_MT
#pragma omp atomic
#endif
	  working_threads--; 
	  return son;
	}
	continue;
      }
#endif
#ifdef YAHSP_MT
#pragma omp atomic
#endif
      stats.expanded_nodes++;
      FOR(a, node->applicable) {
        son = node_derive(node);
        cpt_malloc(son->steps, 1);
        node_apply_action(son, a);
        if ((son = compute_or_send_node(son))) {
#ifdef YAHSP_MT
#pragma omp atomic
#endif
	  working_threads--; 
	  return son; 
	}
        if (test_stop_search()) { 
#ifdef YAHSP_MT
#pragma omp atomic
#endif
	  working_threads--; 
	  return NULL; 
	}
      } EFOR;
      cpt_free(node->applicable);
#ifdef YAHSP_MT
#pragma omp atomic
#endif
      working_threads--;
    } else if (started) stats.no_work_counter++;
#ifdef YAHSP_MT
#pragma omp critical
#endif
    if (working_threads == 0 && gdsl_rbtree_is_empty(open_list)) open_empty = true;
  }
  return NULL;
}

void yahsp_reset()
{
  state_copy(current_state, initial_bitstate);
}

void yahsp_init()
{
  long i;
  cpt_malloc(aused, actions_nb);
  cpt_malloc(ainit, actions_nb);
  cpt_malloc(finit, fluents_nb);
  cpt_malloc(relaxed_plan, actions_nb);
  cpt_malloc(applicable, actions_nb);
  cpt_malloc(open_lists, opt.yahsp_teams);
  cpt_malloc(closed_lists, opt.yahsp_teams);
#ifdef YAHSP_MT
  cpt_malloc(open_locks, opt.yahsp_teams);
  cpt_malloc(closed_locks, opt.yahsp_teams);
#endif
  for(i = 0; i < opt.yahsp_teams; i++) {
    open_lists[i] = gdsl_rbtree_alloc(NULL, NULL, NULL, (gdsl_compare_func_t) open_list_cmp);
    closed_lists[i] = gdsl_rbtree_alloc(NULL, NULL, (gdsl_free_func_t) node_free, (gdsl_compare_func_t) closed_list_cmp);
#ifdef YAHSP_MT
    omp_init_lock(&open_locks[i]);
    omp_init_lock(&closed_locks[i]);
#endif
  }
#ifdef DAE
  heuristic_create();
#endif
  yahsp_state_size = (fluents_nb - 1) / __WORDSIZE + 1;
  initial_bitstate = state_create();
  current_state = state_create();
  FOR(f, init_state) { state_add(initial_bitstate, f); } EFOR;
  yahsp_reset();
  srand(opt.seed);
#ifdef YAHSP_MPI
  mpi_yahsp_init(yahsp_state_size);
#endif
}

int yahsp_main() 
{
  Node *node = NULL;

  stop_search = false;
  node_id_counter = 0;
  stats.computed_nodes = 0;
  stats.evaluated_nodes = 0;
  stats.expanded_nodes = 0;
  best_makespan = MAXTIME;

  FOR(node_limit, node_limits) {
    long num_threads = mini(opt.yahsp_threads, node_limit.nb_threads);
    nodes_limit = mini(yahsp_max_evaluated_nodes, (num_threads == opt.yahsp_threads ? LONG_MAX : node_limit.max_nodes));    
    //trace(normal, "#threads : %ld -- already evaluated nodes : %ld\n", num_threads, stats.evaluated_nodes);
    if (test_stop_search()) break;
#ifdef YAHSP_MT
#pragma omp parallel num_threads(num_threads * opt.yahsp_teams)
#endif
    {
#ifdef YAHSP_MT
      if (omp_get_thread_num() > 0) {
	cpt_malloc(aused, actions_nb);
	cpt_malloc(ainit, actions_nb);
	cpt_malloc(finit, fluents_nb);
	cpt_malloc(relaxed_plan, actions_nb);
	cpt_malloc(applicable, actions_nb);
      }
      open_list = open_lists[omp_get_thread_num() % opt.yahsp_teams];
      closed_list = closed_lists[omp_get_thread_num() % opt.yahsp_teams];
      open_lock = &open_locks[omp_get_thread_num() % opt.yahsp_teams];
      closed_lock = &closed_locks[omp_get_thread_num() % opt.yahsp_teams];
#else
      open_list = open_lists[0];
      closed_list = closed_lists[0];
#endif
      working_threads = 0;
#ifdef YAHSP_MPI
      mpi_barrier();
      Node *n = (mpi_is_master() ? yahsp_plan_master() : yahsp_plan());
#else
      Node *n = yahsp_plan();
#endif
#ifdef YAHSP_MT
      if (omp_get_thread_num() > 0) {
	cpt_free(aused);
	cpt_free(ainit);
	cpt_free(finit);
	cpt_free(relaxed_plan);
	cpt_free(applicable);
      }
#endif
      if (n != NULL) {
#ifdef YAHSP_MT
#pragma omp critical
#endif
	if (node == NULL || n->makespan < node->makespan) {
	  node = n;
	  stop_search = true;
	}
      }
    }
  } EFOR;

  int return_code;
  if (node == NULL) {
    return_code = NO_PLAN;
  } else {
    state_copy(current_state, node->state);
    solution_plan = create_solution_plan(node);
    return_code = PLAN_FOUND;
  }
  long i;
  for(i = 0; i < opt.yahsp_teams; i++) {
    gdsl_rbtree_flush(open_lists[i]);
    gdsl_rbtree_flush(closed_lists[i]);
  }

#ifdef YAHSP_MPI
  mpi_compute_statistics();
#else
  stats.nodes_by_sec = stats.evaluated_nodes / get_timer(stats.search);
#endif

  return return_code;
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
