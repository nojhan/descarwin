/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : yahsp-mt.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#include "cpt.h"
#include "options.h"
#include "trace.h"
#include "structs.h"
#include "plan.h"
#include "trace_planner.h"
#include "globs.h"
// DIRTY
#define end_action (&yend_action)
#include "yahsp.h"
#include "yahsp-common.h"
#ifdef DAE
#include "yahsp-dae.h"
#endif
#ifdef YAHSP_MPI
#include "yahsp-mpi.h"
#endif


static struct {
  gdsl_rbtree_t list;
#ifdef YAHSP_MT
  omp_lock_t lock;
#endif
} *open_lists, *open_list, *closed_lists, *closed_list;

static State current_state;
static State initial_bitstate;

static TimeVal best_makespan_common = MAXTIME;
static TimeVal best_makespan_private = MAXTIME;
static TimeVal *best_makespan;

//static 
bool stop_search;
static ulong nodes_limit;
static ulong working_threads;

#ifdef YAHSP_MT
static ulong evaluated_nodes_thread;
#endif

#ifdef YAHSP_MPI 
static Node *solution_node;

// static struct { int base; int nb; } mpi_teams[] = {
//   {0, 1},
//   {1, 2}, {1, 2},
//   {3, 3}, {3, 3}, {3, 3},
//   {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6},
//   {12, 12}, {12, 12}, {12, 12}, {12, 12}, {12, 12}, {12, 12},
//   {12, 12}, {12, 12}, {12, 12}, {12, 12}, {12, 12}, {12, 12},
//   {24, 24}, {24, 24}, {24, 24}, {24, 24}, {24, 24}, {24, 24}, 
//   {24, 24}, {24, 24}, {24, 24}, {24, 24}, {24, 24}, {24, 24}, 
//   {24, 24}, {24, 24}, {24, 24}, {24, 24}, {24, 24}, {24, 24}, 
//   {24, 24}, {24, 24}, {24, 24}, {24, 24}, {24, 24}, {24, 24}
// };
#endif

static bool stop_on_timer = false;

int ties_pop_best;

#ifdef YAHSP_MT
#ifdef DAE
#pragma omp threadprivate(open_list, closed_list, open_lists, closed_lists, current_state, initial_bitstate, best_makespan, stop_search, nodes_limit, working_threads, evaluated_nodes_thread, ties_pop_best)
static struct { int nb_threads; ulong max_nodes; } node_limits[] = { {1, ULONG_MAX} };
static ulong node_limits_nb = 1;
#else
#pragma omp threadprivate(open_list, closed_list, evaluated_nodes_thread, best_makespan, best_makespan_private, ties_pop_best)
// static struct { int nb_threads; ulong max_nodes; } node_limits[] = 
//   { {1, 50}, {4, 400}, {8, 3000}, {16, 20000}, {32, 100000}, {64, ULONG_MAX} };
// static ulong node_limits_nb = 6;
static struct { int nb_threads; ulong max_nodes; } node_limits[] = { {64, ULONG_MAX} };
static ulong node_limits_nb = 1;
#endif
#endif

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
  OMP(omp_set_lock(&open_list->lock));
  gdsl_rbtree_insert(open_list->list, node, &gdsl_return);
  OMP(omp_unset_lock(&open_list->lock));
}

static Node *closed_list_insert(Node *node)
{ 
  int gdsl_return;
  OMP(omp_set_lock(&closed_list->lock));
  node->id = gdsl_rbtree_get_size(closed_list->list);
  gdsl_rbtree_insert(closed_list->list, node, &gdsl_return);
  OMP(omp_unset_lock(&closed_list->lock));
  return gdsl_return == GDSL_INSERTED ? node : ({ node_free(node); (Node *) NULL; });
}

static bool test_stop_search()
{ 
  return stop_search || stats.evaluated_nodes >= nodes_limit; 
}

static Node *compute_node(Node *node);


#ifdef YAHSP_MPI

#ifdef YAHSP_MT
static void lists_insert_foreign(Node *node, int dest)
{
  int gdsl_return;
  omp_set_lock(&closed_lists[dest].lock);
  node->id = gdsl_rbtree_get_size(closed_lists[dest].list);
  gdsl_rbtree_insert(closed_lists[dest].list, node, &gdsl_return);
  omp_unset_lock(&closed_lists[dest].lock);
  if (gdsl_return == GDSL_INSERTED) {
    omp_set_lock(&open_lists[dest].lock);
    gdsl_rbtree_insert(open_lists[dest].list, node, &gdsl_return);
    omp_unset_lock(&open_lists[dest].lock);
  } else node_free(node);
}
#endif

void solution_path_free(Node *node)
{
  while (node != NULL) {
    Node *n = node->parent;
    cpt_free(node->steps);
    node_free(node);
    node = n;
  }
}

int node_target_team(Node *node)
{ return (node->key / mpi_get_numtasks()) % opt.yahsp_teams; }

static Node *compute_or_send_node(Node *node)
{
  if (node == NULL) return NULL;
  node_compute_key(node);
  if (opt.yahsp_duplicate_search || !mpi_send_node(node, node->key % mpi_get_numtasks())) {
#ifdef YAHSP_MT
    int dest = node_target_team(node);
    if (!opt.yahsp_duplicate_search && omp_get_thread_num() % opt.yahsp_teams != dest) {
      node->fvalue -= 10000000;
      lists_insert_foreign(node, dest);
    }
    else
#endif
      return compute_node(node);
  }
  return NULL;
}

bool stop_called;

static Node *check_received_nodes()
{
  Node *node;
  bool synchro_required; 
  synchro_required = false;

  OMP_MASTER 
    if (stop_on_timer && !stop_called) {
      stop_called = true;
      mpi_call_synchronize();
    }
  while ((node = mpi_recv_node(&synchro_required))) {
#ifdef YAHSP_MT
    lists_insert_foreign(node, node_target_team(node));
#else
    if (closed_list_insert(node) != NULL) open_list_insert(node);
#endif
  }
  if (synchro_required) {
    bool stop = stop_on_timer;
    node = mpi_synchronize_solution(solution_node, best_makespan, &stop);
    if (stop) { stop_called = false; stop_search = true; return NULL; }
    if (!opt.anytime) { stop_search = true; return node; }
    OMP_MASTER if (mpi_is_master() && node != NULL) yahsp_trace_anytime(node);
    solution_path_free(node);
  }
  return NULL;
}

#else

static Node *compute_or_send_node(Node *node)
{
  if (node == NULL) return NULL;
  node_compute_key(node);
  return compute_node(node);
}

#endif

static Node *compute_node_no_closed_check(Node *node)
{ 
  if (test_stop_search() || node->makespan >= *best_makespan) return NULL; 
  OMP_ATOMIC stats.evaluated_nodes++;
#ifdef YAHSP_MT
  evaluated_nodes_thread++;
#endif
  compute_h1(node);
  if (get_action_cost(end_action) == MAXTIME) return NULL;
  if (get_action_cost(end_action) == 0) {
#ifndef YAHSP_MPI    
    if (!opt.anytime) return node;
#endif
    if (node->makespan < *best_makespan) {
      OMP_CRITICAL {
	if (node->makespan < *best_makespan) {
	  *best_makespan = node->makespan;
#ifdef YAHSP_MPI
	  solution_node = node;
	  mpi_call_synchronize();
#else
	  yahsp_trace_anytime(node);
#endif
	}
      }
    }
    return NULL;
  }
  copy_applicable_actions(node);
  compute_relaxed_plan(node);
  node->fvalue = get_node_fvalue(node);
  open_list_insert(node);

  Node *son = apply_relaxed_plan(node, *best_makespan);
  if (son == NULL) return NULL;
  node_compute_key(son);
  Node *res = compute_node(son);
  
  //Node *res = compute_or_send_node(apply_relaxed_plan(node, *best_makespan));

  stats.wsearch = get_wtimer(stats.search);
  return res;
}

static Node *compute_node(Node *node)
{
  if (closed_list_insert(node) == NULL) return NULL;
  return compute_node_no_closed_check(node);
}

static Comparison open_list_ties_cmp(Node *node1, Node *node2)
{
  LESS(node1->fvalue, node2->fvalue);
  LESS(node1->length, node2->length);
  return Equal;
}

static int open_list_map(Node *node, gdsl_location_t loc, Node **data) 
{
  if (*data == NULL) *data = node;
  if (opt.random && open_list_ties_cmp(node, *data) == Equal) { 
    if (cpt_rand() % ++ties_pop_best == 0) *data = node;
    return GDSL_MAP_CONT;
  }
  return GDSL_MAP_STOP;
}

static Node *open_list_pop_best()
{
  Node *node = NULL;
  OMP(omp_set_lock(&open_list->lock));
  ties_pop_best = 0;
  gdsl_rbtree_map_infix(open_list->list, (gdsl_map_func_t) open_list_map, &node);
  if (node != NULL) gdsl_rbtree_remove(open_list->list, node);
  OMP(omp_unset_lock(&open_list->lock));
  return node;
}

static Node *yahsp_plan()
{ 
  Node *node, *son = NULL;

#ifdef YAHSP_MPI
  mpi_barrier();
  if ((mpi_is_master() OMP( && omp_get_thread_num() == 0)) || opt.yahsp_duplicate_search)
#else
#ifdef YAHSP_MT
  bool open_empty = false;
  if (omp_get_thread_num() < opt.yahsp_teams)
#endif
#endif
    son = compute_or_send_node(create_node(current_state));

  OMP_BARRIER;
  if (son != NULL) return son;

#ifdef YAHSP_MPI
  while (!test_stop_search()) {
    if ((node = check_received_nodes()) || test_stop_search()) return node;
#else
  while (!open_empty && !test_stop_search()) { 
#endif
    if ((node = open_list_pop_best())) {
      OMP_ATOMIC working_threads++;
#ifdef YAHSP_MPI
      if (node->applicable == NULL) {
	//printf("proc %d thread %d lookahead\n", mpi_get_rank(), omp_get_thread_num());
	if ((son = compute_node_no_closed_check(node))) {
	  OMP_ATOMIC working_threads--;
	  return son;
	}
	continue;
      }
#endif
      OMP_ATOMIC stats.expanded_nodes++;
      //printf("proc %d thread %d expands\n", mpi_get_rank(), omp_get_thread_num());
      FOR(a, node->applicable) {
        son = node_derive(node);
        cpt_malloc(son->steps, 1);
        node_apply_action(son, a);
        if ((son = compute_or_send_node(son)) || test_stop_search()) {
      	  OMP_ATOMIC working_threads--;
      	  return son; 
      	}
#ifdef YAHSP_MPI
	if ((son = check_received_nodes()) || test_stop_search()) return son;
#endif
      } EFOR;
      cpt_free(node->applicable);
      OMP_ATOMIC working_threads--;
    } else {
#ifdef YAHSP_MPI
//       Node *node = mpi_get_cached_node();
//       if (node == NULL) continue;
// #ifdef YAHSP_MT
//       int dest = node_target_team(node);
//       lists_insert_foreign(node, dest);
// #else
//       if (closed_list_insert(node) != NULL) open_list_insert(node);
// #endif
      mpi_flush_cached_nodes();
#endif
    }
#ifndef YAHSP_MPI
    OMP_CRITICAL if (working_threads == 0 && gdsl_rbtree_is_empty(open_list->list)) open_empty = true;
#endif
  }
  return NULL;
}

void yahsp_reset()
{
  state_copy(current_state, initial_bitstate);
}


void yahsp_init()
{
  int i;
  alloc_eval_structures();
  cpt_malloc(open_lists, opt.yahsp_teams);
  cpt_malloc(closed_lists, opt.yahsp_teams);
  for(i = 0; i < opt.yahsp_teams; i++) {
    open_lists[i].list = gdsl_rbtree_alloc(NULL, NULL, NULL, (gdsl_compare_func_t) open_list_cmp);
    closed_lists[i].list = gdsl_rbtree_alloc(NULL, NULL, (gdsl_free_func_t) node_free, (gdsl_compare_func_t) closed_list_cmp);
#ifdef YAHSP_MT
    omp_init_lock(&open_lists[i].lock);
    omp_init_lock(&closed_lists[i].lock);
#endif
  }
#ifdef DAE
  heuristic_create();
#endif
  initial_bitstate = state_create();
  current_state = state_create();
  FOR(f, init_state) { state_add(initial_bitstate, f); } EFOR;
  yahsp_reset();
#ifdef YAHSP_MPI
  OMP_PARALLEL(num_threads(opt.yahsp_threads * opt.yahsp_teams)) {
    cpt_rand(opt.seed + mpi_get_rank() OMP(* (opt.yahsp_teams) + omp_get_thread_num()));
  }
#else
  cpt_srand(opt.seed);
#endif
}

static void yahsp_timer_init(long time_limit, sighandler_t handler)
{
  signal(SIGALRM, handler);
  alarm(time_limit);
}

static void yahsp_exit_on_timer(int n)
{
  exit(0);
}

static void yahsp_stop_on_timer(int n)
{
  yahsp_timer_init(100, yahsp_exit_on_timer); 
#ifdef YAHSP_MPI
  if (mpi_is_master())
#endif
    stop_on_timer = true;
}

int yahsp_main(ulong nodes_limit_global)
{
  Node *best_node = NULL;
  int num_threads;

  stop_search = false;
  stats.computed_nodes = 0;
  stats.evaluated_nodes = 0;
  stats.expanded_nodes = 0;
  best_makespan_common = MAXTIME;
  best_makespan_private = MAXTIME;
#ifdef YAHSP_MT
  stats.min_eval_thread = ULONG_MAX;
  stats.max_eval_thread = 0;
  evaluated_nodes_thread = 0;
#endif

#ifdef YAHSP_MPI
  mpi_barrier();
#endif
  start_timer(stats.search);
  OMP_MASTER yahsp_timer_init(-opt.timer, yahsp_stop_on_timer);

  FOR(node_limit, node_limits) {
    num_threads = mini(opt.yahsp_threads, node_limit.nb_threads);
    nodes_limit = mini(nodes_limit_global, 
		       (num_threads == opt.yahsp_threads ? ULONG_MAX : node_limit.max_nodes));    
    //trace(normal, "#threads : %d -- already evaluated nodes : %lu\n", num_threads, stats.evaluated_nodes);
#ifdef YAHSP_MT
    // long-term forgotten...
    int i;
    for(i = 0; i < opt.yahsp_teams; i++) {
      gdsl_rbtree_flush(open_lists[i].list);
      gdsl_rbtree_flush(closed_lists[i].list);
    }
#endif
    OMP_PARALLEL(num_threads(num_threads * opt.yahsp_teams)) {
      if (opt.yahsp_duplicate_search) best_makespan = &best_makespan_private;
      else best_makespan = &best_makespan_common;
#ifdef YAHSP_MT
      if (omp_get_thread_num() > 0) alloc_eval_structures();
      open_list = &open_lists[omp_get_thread_num() % opt.yahsp_teams];
      closed_list = &closed_lists[omp_get_thread_num() % opt.yahsp_teams];
#else
      open_list = &open_lists[0];
      closed_list = &closed_lists[0];
#endif
      working_threads = 0;
#ifdef YAHSP_MPI
      mpi_yahsp_init();
#endif
      Node *node = yahsp_plan();
#ifdef YAHSP_MPI
      mpi_yahsp_cleanup();
#endif
#ifdef YAHSP_MT
      OMP_CRITICAL {
	minimize(stats.min_eval_thread, evaluated_nodes_thread);
	maximize(stats.max_eval_thread, evaluated_nodes_thread);
      }
      if (omp_get_thread_num() > 0) free_eval_structures();
#endif
      OMP_CRITICAL 
	if (node != NULL && (best_node == NULL || node->makespan < best_node->makespan)) {
	  best_node = node;
	  stop_search = true;
	}
    }
    if (stop_search || stats.evaluated_nodes >= nodes_limit_global) break;
  } EFOR;
  if (best_node != NULL) {
    state_copy(current_state, best_node->state);
    solution_plan = create_solution_plan(best_node);
#ifdef YAHSP_MPI
    solution_path_free(best_node);
#endif
  }
  OMP_PARALLEL(num_threads(num_threads * opt.yahsp_teams)) {
    gdsl_rbtree_flush(open_list->list);
    gdsl_rbtree_flush(closed_list->list);
  }
#ifdef YAHSP_MPI
  mpi_compute_statistics();
#else
  stats.nodes_by_sec = stats.evaluated_nodes / get_wtimer(stats.search);
#endif
  return (best_node == NULL ? NO_PLAN : PLAN_FOUND);
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
