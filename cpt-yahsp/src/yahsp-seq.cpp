/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : yahsp-seq.c
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
// VERY DIRTY
#define end_action (&yend_action)
#include "yahsp.h"
#include "yahsp-common.h"
#ifdef DAE
#include "yahsp-dae.h"
#endif


static gdsl_heap_t open_list;
static gdsl_rbtree_t closed_list;

static State current_state;
static State initial_bitstate;

static TimeVal best_makespan;
static ulong nodes_limit;

#ifdef DAE
#pragma omp threadprivate(open_list, closed_list, current_state, initial_bitstate, best_makespan, nodes_limit)
#endif


static Comparison open_list_cmp(Node *node1, Node *node2)
{
  GREATER(node1->fvalue, node2->fvalue);
#ifdef DAE
  LESS(node1->makespan, node2->makespan);
#else
  GREATER(node1->length, node2->length);
#endif
  GREATER(node1->id, node2->id);
  return Equal;
}

static Comparison closed_list_cmp(Node *node1, Node *node2)
{
  LESS(node1->key, node2->key);
  return (Comparison) state_cmp(node1->state, node2->state);
}

static Node *open_list_insert(Node *node)
{
  return (Node *) gdsl_heap_insert(open_list, node);
}

static Node *closed_list_insert(Node *node)
{
  int gdsl_return;
  node_compute_key(node);
  node->id = gdsl_rbtree_get_size(closed_list);
  gdsl_rbtree_insert(closed_list, node, &gdsl_return);
  return gdsl_return == GDSL_INSERTED ? node : ({ node_free(node); (Node *) NULL; });
}

static Node *compute_node(Node *node)
{
  if (node == NULL 
      || closed_list_insert(node) == NULL 
      || stats.evaluated_nodes >= nodes_limit
      || node->makespan >= best_makespan) return NULL;
  stats.evaluated_nodes++;
  compute_h1(node);
  if (get_action_cost(end_action) == MAXTIME) return NULL;
  if (get_action_cost(end_action) == 0) {
    if (!opt.anytime) return node;
    if (node->makespan < best_makespan) {
      best_makespan = node->makespan;
      yahsp_trace_anytime(node);
    }
    return NULL;
  }
  copy_applicable_actions(node);
  compute_relaxed_plan(node);
  node->fvalue = get_node_fvalue(node);
  return compute_node(apply_relaxed_plan(open_list_insert(node), best_makespan));
}

static Node *open_list_pop_best()
{
  return (Node *) gdsl_heap_remove_top(open_list);
}

static Node *yahsp_plan()
{
  Node *node = cpt_calloc(node, 1), *son;
  state_clone(node->state, current_state);
  if ((son = compute_node(node))) return son;
  while ((node = open_list_pop_best())) {
    stats.expanded_nodes++;
    FOR(a, node->applicable) {
      son = node_derive(node);
      cpt_malloc(son->steps, 1);
      node_apply_action(son, a);
      if ((son = compute_node(son))) return son;
      if (stats.evaluated_nodes >= nodes_limit) return NULL;
    } EFOR;
    cpt_free(node->applicable);
  }
  return NULL;
}

void yahsp_reset()
{
  state_copy(current_state, initial_bitstate);
}

void yahsp_init()
{
  alloc_eval_structures();
  open_list = gdsl_heap_alloc(NULL, NULL, NULL, (gdsl_compare_func_t) open_list_cmp);
  closed_list = gdsl_rbtree_alloc(NULL, NULL, (gdsl_free_func_t) node_free, (gdsl_compare_func_t) closed_list_cmp);
#ifdef DAE
  heuristic_create();
#endif
  initial_bitstate = state_create();
  current_state = state_create();
  FOR(f, init_state) { state_add(initial_bitstate, f); } EFOR;
  yahsp_reset();
  cpt_srand(opt.seed);
}


int yahsp_main(ulong max_evaluated_nodes)
{
  nodes_limit = max_evaluated_nodes;

  stats.computed_nodes = 0;
  stats.evaluated_nodes = 0;
  stats.expanded_nodes = 0;
  best_makespan = MAXTIME;
  // solution_plan = read_plan_from_file("plan");
  // print_plan_ipc(stdout, solution_plan, 0);
  // plan_free(solution_plan);
  Node *node = yahsp_plan();

  if (node != NULL) {
    state_copy(current_state, node->state);
    solution_plan = create_solution_plan(node);
  }
  gdsl_heap_flush(open_list);
  gdsl_rbtree_flush(closed_list);
  stats.nodes_by_sec = stats.evaluated_nodes / get_timer(stats.search);
  return (node == NULL ? NO_PLAN : PLAN_FOUND);
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
