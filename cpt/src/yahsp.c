/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : yahsp.c
 *
 * Copyright (C) 2005-2009  Vincent Vidal <vidal@cril.univ-artois.fr>
 */

#include "cpt.h"
#include "trace.h"
#include "structs.h"
#include "problem.h"
#include "max_atom.h"
#include "plan.h"
#include "globs.h"
/* #include "dae.h" */


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
  bool state_registered;
  long gvalue;
  long fvalue;
  TimeVal makespan;
  VECTOR(YStep, steps);
  VECTOR(Action *, applicable);
  bool to_destroy;
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
static bool *goal_preferred;

SVECTOR(Action *, relaxed_plan);
SVECTOR(Action *, applicable);

static gdsl_rbtree_t open_list;
static gdsl_rbtree_t closed_list;
static gdsl_rbtree_t heuristics;

static BitArray current_state;
static BitArray initial_bitstate;

static TimeVal best_makespan = MAXTIME;



#define get_ainit(a) ainit[(a)->id]
#define set_ainit(a, t) ainit[(a)->id] = t
#define get_aused(a) aused[(a)->id]
#define set_aused(a, t) aused[(a)->id] = t
#define get_finit(f) finit[(f)->id]
#define set_finit(f, t) finit[(f)->id] = t


#define CMP(a, b) NEST( if ((a) != (b)) return (a) - (b); )


#define COST(a) ({ TimeVal cost = 0; FOR(f, a->prec) { if (get_finit(f) == MAXTIME) { cost = MAXTIME; break; } else cost += get_finit(f); } EFOR; cost; })
//#define COST(a) ({ long cost = 0; FOR(f, a->prec) { maximize(cost, get_finit(f)); } EFOR; cost; })

#define ISBEST(prod, best) (prod->id > 1 && (best == NULL || get_ainit(prod) < get_ainit(best) || (get_ainit(prod) == get_ainit(best) && duration(prod) < duration(best))))
//#define ISBEST(prod, best) (prod->id > 1 && (best == NULL || get_ainit(prod) < get_ainit(best) || (repl && get_ainit(prod) == get_ainit(best) && duration(prod) < duration(best))))
//#define ISBEST(prod, best) (prod->id > 1 && (best == NULL || get_ainit(prod) < get_ainit(best)))

#define INCCOST(cost, action) cost += duration(action) + 1
//#define INCCOST(cost, action) cost ++


static long heuristic_cmp(Heuristic *h1, Heuristic *h2);

void node_free(Node *node) 
{ 
  if (!node->state_registered) free(node->state);
  free(node->steps); 
  free(node->applicable);
  free(node);
}

static long open_list_cmp(Node *node1, Node *node2)
{
  CMP(node1->fvalue, node2->fvalue);
  CMP(node1->makespan, node2->makespan);
  return node1->id - node2->id;
}

static long closed_list_cmp(Node *node1, Node *node2)
{
  CMP(node1->key, node2->key);
  return bitarray_cmp(node1->state, node2->state, fluents_nb);
}

static Node *open_list_insert(Node *node)
{
  int gdsl_return;
  return (Node *) gdsl_rbtree_insert(open_list, node, &gdsl_return);
}

static Node *to_destroy[10000];
static size_t to_destroy_nb;

static Node *closed_list_insert(Node *node)
{
  int gdsl_return;
  ulong i;
  for (i = 0; i < (ulong) (fluents_nb -1) / __WORDSIZE + 1; i++) node->key ^= node->state[i] * (i + 1);
/*   Node *n = */ gdsl_rbtree_insert(closed_list, node, &gdsl_return);
/*   if (0&&gdsl_return != GDSL_INSERTED) { */
/*     if (node->makespan < n->makespan) { */
/*       n->makespan = node->makespan; */
/*       //to_destroy[to_destroy_nb++] = node; */
/*       return node; */
/*     } */
/*   } */
   return gdsl_return == GDSL_INSERTED ? node : ({ node_free(node); (Node *) NULL; });
}


static void update_cost_h1(Fluent *f, TimeVal cost)
{
  if (cost < get_finit(f)) {
    set_finit(f, cost);
    FOR(a, f->consumers) { set_aused(a, true);} EFOR; } 
}


static void compute_h1_cost_yahsp(bool goal_pref)
{
  bool loop = true;
  TimeVal cost;

  while (loop) {
    loop = false;
    FORMIN(a, actions, 2) {
      if (get_aused(a) && (!goal_pref || goal_preferred[a->id])) {
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
/*   applicable_nb = 0; */
/*   FOR(a, actions) { if (get_ainit(a) == 0) applicable[applicable_nb++] = a; } EFOR; */
}

static long heuristic_cmp(Heuristic *h1, Heuristic *h2)
{
  CMP(h1->key, h2->key);
  return bitarray_cmp(h1->state, h2->state, fluents_nb);
}

static void heuristic_insert(Node *node)
{
  Heuristic *h = cpt_malloc(h, 1);
  int gdsl_return;
  h->state = node->state;
  h->key = node->key;
  node->state_registered = true;
  cpt_malloc(h->inits, fluents_nb);
  memcpy(h->inits, finit, fluents_nb * sizeof(TimeVal));
  gdsl_rbtree_insert(heuristics, h, &gdsl_return);
}

static bool heuristic_search(Node *node)
{
  Heuristic test;
  test.state = node->state;
  test.key = node->key;
  Heuristic *h = (Heuristic *) gdsl_rbtree_search(heuristics, (gdsl_compare_func_t) heuristic_cmp, &test);
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

static void compute_h1(Node *node, bool goal_pref)
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
  compute_h1_cost_yahsp(goal_pref);
  set_ainit(end_action, COST(end_action));
  if (opt.dae) heuristic_insert(node);
}


static int cmp_actions(const void *v1, const void *v2)
{
  Action *a1 = *((Action **) v1);
  Action *a2 = *((Action **) v2);
  CMP(get_ainit(a1), get_ainit(a2));
  FOR(f, a1->prec) { if (deletes(a2, f)) return -1; } EFOR;
  FOR(f, a2->prec) { if (deletes(a1, f)) return 1; } EFOR;
  return 0;
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
	FOR(prod, f->producers) { if (ISBEST(prod, best)) best = prod; } EFOR;
	if (best != NULL && !get_aused(best)) {
 	  set_aused(best, true);
	  relaxed_plan[relaxed_plan_nb++] = best;
	}
      }
    } EFOR;
  } EFOR;
  qsort(relaxed_plan, relaxed_plan_nb, sizeof(Action *), cmp_actions);
}

static bool can_be_applied(Node *node, Action *a)
{
  FOR(f, a->prec) { if (!bitarray_get(node->state, f)) return false; } EFOR;
  return true;
}

static bool action_must_precede(Action *a1, Action *a2)
{
  if (/* pddl_domain->action_costs ||  */amutex(a1, a2)) return true;
  FOR(f, a2->prec) { if (produces(a1, f)) return true; } EFOR;
  return false;
}

static void node_apply_action(Node *node, Action *a) 
{
    FOR(f, a->del) { bitarray_unset(node->state, f); } EFOR;
    FOR(f, a->add) { bitarray_set(node->state, f); } EFOR;
    node->gvalue++;
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
  son->id = stats.computed_nodes++;
  son->ancestor = node;
  son->gvalue = node->gvalue;
  son->state = bitarray_create(fluents_nb);
  son->makespan = node->makespan;
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
    if (a != NULL && a != end_action) {
      FOR(b, relaxed_plan) {
	if (b != NULL && a != b && b != end_action) {
	  FOR(f, a->add) {
	    if (!bitarray_get(node->state, f) && consumes(b, f)) {
	      Action *best = NULL;
	      FOR(prod, f->producers) { if (prod != a && prod != b && can_be_applied(son, prod) && ISBEST(prod, best)) best = prod; } EFOR;
	      if (best != NULL) {
		relaxed_plan[i] = NULL;
		node_apply_action(son, best);
		if (son->makespan > best_makespan) { node_free(son); return NULL; }
		goto start;
	      }
	    }
	  } EFOR;
	}
      } EFOR;
    }
  } EFOR;
  //if (son->steps_nb == 0) error(no_plan, "Erreur plan relaxé inapplicable");
  if (son->steps_nb > 0) cpt_realloc(son->steps, son->steps_nb);
  return son;
}

static Node *compute_node(Node *node)
{
  if (node == NULL || closed_list_insert(node) == NULL || stats.evaluated_nodes >= opt.max_backtracks) return NULL;
  if (node->makespan >= best_makespan) return NULL;
  stats.evaluated_nodes++;
  compute_h1(node, !opt.dae);
  if (!opt.dae && get_ainit(end_action) == MAXTIME) compute_h1(node, false);
  if (get_ainit(end_action) == MAXTIME) return NULL;
  if (get_ainit(end_action) == 0) return node;
/*   if (get_ainit(end_action) == 0) {  */
/*     if (node->makespan < best_makespan) { */
/*       best_makespan = node->makespan; */
/*       printf("MAK : %ld\n", node->makespan);  */
/*     } */
/*     return NULL;  */
/*   }  */
  vector_copy(node->applicable, applicable);
  compute_relaxed_plan(node);
  //node->fvalue = node->gvalue + relaxed_plan_nb * 3;
  node->fvalue = node->gvalue + get_ainit(end_action);
  //node->fvalue = node->makespan + get_ainit(end_action);
  //open_list_insert(node); return NULL;
  return compute_node(apply_relaxed_plan(open_list_insert(node)));
}



static int open_list_map(Node *node, gdsl_location_t loc, void *data) 
{
  if (*((Node **) data) == NULL) *((Node **) data) = node;
  return GDSL_MAP_STOP;
}

static Node *open_list_pop_best()
{
  Node *node = NULL;
  gdsl_rbtree_map_infix(open_list, (gdsl_map_func_t) open_list_map, &node);
  if (node != NULL) gdsl_rbtree_remove(open_list, node);
  return node;
}

void yahsp_reset()
{
  free(current_state);
  current_state = bitarray_create(fluents_nb);
  bitarray_copy(current_state, initial_bitstate, fluents_nb);
}

static Node *yahsp_plan()
{
  Node *node = cpt_calloc(node, 1), *son;
  node->state = current_state;
  if ((son = compute_node(node))) return son;
  while ((node = open_list_pop_best())) {
    stats.expanded_nodes++;
    FOR(a, node->applicable) {
      son = node_derive(node);
      cpt_malloc(son->steps, 1);
      node_apply_action(son, a);
      if ((son = compute_node(son))) return son;
      if (stats.evaluated_nodes >= opt.max_backtracks) return NULL;
    } EFOR;
    //if (node->to_destroy) node_free(node); else 
    cpt_free(node->applicable);
    FOR(n, to_destroy) { gdsl_rbtree_remove(open_list, n); node_free(n); } EFOR;
    to_destroy_nb = 0;
  }
  return NULL;
}

void yahsp_init()
{
  compute_init_rh1_cost();
  cpt_malloc(ainit, actions_nb);
  cpt_malloc(aused, actions_nb);
  cpt_malloc(finit, fluents_nb);
  cpt_malloc(relaxed_plan, actions_nb);
  cpt_malloc(goal_preferred, actions_nb);
  cpt_malloc(applicable, actions_nb);

  open_list = gdsl_rbtree_alloc(NULL, NULL, NULL, (gdsl_compare_func_t) open_list_cmp);
  closed_list = gdsl_rbtree_alloc(NULL, NULL, (gdsl_free_func_t) node_free, (gdsl_compare_func_t) closed_list_cmp);
  if (opt.dae) heuristics = gdsl_rbtree_alloc(NULL, NULL, NULL, (gdsl_compare_func_t) heuristic_cmp);

  initial_bitstate = bitarray_create(fluents_nb);
  FOR(f, init_state) { bitarray_set(initial_bitstate, f); } EFOR;
  yahsp_reset();
  srand(opt.seed);
}


int yahsp_main() 
{
  if (!opt.dae) {
    FOR(a, actions) { goal_preferred[a->id] = true; } EFOR;
    FOR(f, end_action->prec) {
      if (!produces(start_action, f)) 
	FOR(a, actions) { if (deletes(a, f) || edeletes(a, f)) goal_preferred[a->id] = false; } EFOR;
    } EFOR;
  }

  stats.computed_nodes = 0;
  stats.evaluated_nodes = 0;
  stats.expanded_nodes = 0;

  Node *node = yahsp_plan();

  if (node == NULL) {
    current_state = NULL;
    gdsl_rbtree_flush(open_list);
    gdsl_rbtree_flush(closed_list);
    return NO_PLAN;
  }
  
  current_state = bitarray_create(fluents_nb);
  bitarray_copy(current_state, node->state, fluents_nb);

  SolutionPlan *plan = cpt_calloc(plan, 1);

  cpt_malloc(plan->steps, (plan->steps_nb = node->gvalue));
  plan->makespan = node->makespan;

  while (node != NULL) {
    FORi(a, i, node->steps) {
      //printf("%s %d %d\n", action_name(a->action), a->init, a->end);
      Step *s = cpt_calloc(plan->steps[node->gvalue - node->steps_nb + i], 1);
      s->action = a.action;
      s->init = a.init;
    } EFOR;
    node = node->ancestor;
  }
  
  qsort(plan->steps, plan->steps_nb, sizeof(Step *), precedes_in_plan);
  plan->backtracks = stats.evaluated_nodes;

  solution_plan = plan;
  
  gdsl_rbtree_flush(open_list);
  gdsl_rbtree_flush(closed_list);

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
      s->init = 0;
      RFOR(s2, plan->steps) {
	if (action_must_precede(s2->action, s->action)) {
	  maximize(s->init, s2->end);
	  if (s->init == plan->makespan) break;
	}
      } EFOR;
      s->end = s->init + duration(s->action);
      maximize(plan->makespan, s->end);
      *cpt_calloc(plan->steps[plan->steps_nb++], 1) = *s; 
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
