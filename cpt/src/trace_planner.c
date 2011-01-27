/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : trace_planner.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#include "cpt.h"
#include "structs.h"
#include "problem.h"
#include "branching.h"
#include "globs.h"
#include "plan.h"
#include "trace_planner.h"


/*---------------------------------------------------------------------------*/
/* Local Variables                                                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Static Functions                                                          */
/*---------------------------------------------------------------------------*/


/*****************************************************************************/

void trace_new_world(void) {
  fprintf(cptout, "\n================================================ New World : %ld ================================================\n\n\n", current_world());
}

void trace_backtrack(void) {
  fprintf(cptout, "\n------------------------------------------- Backtrack to World : %ld -------------------------------------------\n\n", current_world());
}

void trace_begin_bound(TimeVal bound)
{
  fprintf(cptout, "Bound : ");
  print_time(cptout, bound - (opt.pddl21 ? pddl_domain->precision.t : 0));
  fprintf(cptout, "  ---  ");

}

void trace_end_bound(long nodes, long backtracks, double time)
{
  fprintf(cptout, "Nodes : %ld  ---  Backtracks : %ld  ---  Iteration time : %.2f\n", nodes, backtracks, time);
}

void trace_restart(long nodes, long backtracks, double time, TimeVal bound)
{
  fprintf(cptout, "%ld  ---  %ld  ---  %.2f\nBound : ", nodes, backtracks, time);
  print_time(cptout, bound - (opt.pddl21 ? pddl_domain->precision.t : 0));
  fprintf(cptout, "  ---  ");
}

void trace_problem_stats(long actions_nb, long fluents_nb, long causals_nb)
{
  fprintf(cptout, "\nProblem : %ld actions, %ld fluents, %ld causals\n          %ld init facts, %ld goals\n\n",
	 actions_nb, fluents_nb, causals_nb, start_action->add_nb, end_action->prec_nb);
}

void trace_solution_plan(SolutionPlan *plan)
{
  fprintf(cptout, "\n");
  print_plan(cptout, plan, true);
}

void trace_plan_stats(SolutionPlan *plan)
{
  if (opt.verbosity > 0) {
    if (pddl_domain->action_costs) {
      fprintf(cptout, "\nTotal cost : ");
      print_time(cptout, plan->makespan);
    } else if (!opt.sequential) {
      fprintf(cptout, "\nMakespan : ");
      print_time(cptout, plan->makespan);
    }
    fprintf(cptout, "\nLength : %ld\n", plan->steps_nb);
  }
}

void trace_search_stats(double search_time, double total_time)
{
  if (opt.verbosity > 0) {
    if (opt.yahsp) {
      fprintf(cptout, "Computed nodes : %ld\n", stats.computed_nodes);
      fprintf(cptout, "Evaluated nodes : %ld\n", stats.evaluated_nodes);
      fprintf(cptout, "Expanded nodes : %ld\n", stats.expanded_nodes);
      fprintf(cptout, "Evaluated nodes/sec : %.2f\n", stats.evaluated_nodes / search_time);
    } else {
      fprintf(cptout, "Nodes : %ld\n", nb_nodes());
      fprintf(cptout, "Backtracks : %ld\n", nb_backtracks());
      fprintf(cptout, "Support choices : %ld\n", stats.support_choices);
      fprintf(cptout, "Conflict choices : %ld\n", stats.conflict_choices);
      fprintf(cptout, "Mutex choices : %ld\n", stats.mutex_choices);
      fprintf(cptout, "Start time choices : %ld\n", stats.start_time_choices);
      fprintf(cptout, "World size : %ldK\n", world_size() / 1000);
      fprintf(cptout, "Nodes/sec : %.2f\n", nb_nodes() / search_time);
    }
    double wctime = omp_get_wtime() - stats.wcsearch;
    fprintf(cptout, "Search us time : %.2f\n", search_time);
    fprintf(cptout, "Search wc time : %.2f\n", wctime);
    fprintf(cptout, "Core utility : %.2f\n", mini(100, search_time * 100 / (wctime * mini(opt.yahsp_threads, omp_get_num_procs()))));
    fprintf(cptout, "Total us time : %.2f\n", total_time);
    fprintf(cptout, "Total wc time : %.2f\n", omp_get_wtime() - stats.wctime);
    fprintf(cptout, "\n");
  }
}

void trace_yahsp_anytime_search_stats(TimeVal makespan, double search_time, double total_time)
{
  fprintf(cptout, "plan found : ");
  if (pddl_domain->action_costs) fprintf(cptout, "cost ");
  else if (opt.sequential) fprintf(cptout, "length ");
  else fprintf(cptout, "makespan ");
  print_time(cptout, makespan);
  fprintf(cptout, " -- search time %.2f -- total time %.2f\n", search_time, total_time);
}


void trace_conflict_choice(Causal *c, Action *a, bool direction, bool first)
{
  if (first) {
    fprintf(cptout, "*** Support threat choice : <"); print_action(a); fprintf(cptout, ", "); print_causal(c); fprintf(cptout, "> among\n");
    FOR(c, active_causals) {
      FOR(a, c->fluent->edeleters) {
	if (support_threat(c, a)) { fprintf(cptout, "\t<"); print_action(a); fprintf(cptout, ", "); print_causal(c); fprintf(cptout, ">\n"); }
      } EFOR;
    } EFOR;
    fprintf(cptout, "\n\n");
  }
  if (first == direction) { fprintf(cptout, "*** Ordering choice : ["); print_action(a); fprintf(cptout, " << "); print_causal(c); fprintf(cptout, "]\n\n"); }
  else { fprintf(cptout, "*** Ordering choice : ["); print_action(c->consumer); fprintf(cptout, " << "); print_action(a); fprintf(cptout, "]\n\n"); }      
}

void trace_support_choice(Causal *c, Action *a, bool direction, bool first)
{
  if (first) {
    fprintf(cptout, "*** Open precondition choice : "); print_causal(c); fprintf(cptout, " among\n");
    FOR(c, active_causals) {
#ifndef RESOURCES
      if (!is_produced(c)) { 
#else
      if (!is_produced(c) && !c->fluent->no_branching) { 
#endif
	fprintf(cptout, "\t"); print_causal(c);
	fprintf(cptout, "\n"); }
    } EFOR;
    fprintf(cptout, "\n\n");
    fprintf(cptout, "*** Producer choice : "); print_action(a); fprintf(cptout, "\n");
    
    //    FORPROD(a, c) { fprintf(cptout, "\t"); print_action(a); fprintf(cptout, " unsupported precs : %d - deleted subgoals : %d\n", unsupported_precs(a), deleted_subgoals(a)); } EFOR;
    FORPROD(a, c) { fprintf(cptout, "\t"); print_action(a); fprintf(cptout, "\n"); } EFOR;
    fprintf(cptout, "\n\n");
  }
}

void trace_mutex_choice(Action *a1, Action *a2, bool direction, bool first)
{
  if (first) {
    fprintf(cptout, "*** Mutex choice : <"); print_action(a1); fprintf(cptout, ", "); print_action(a2); fprintf(cptout, "> among\n");
    FORPAIR(a1, a2, active_actions) {
	if (mutex_threat(a1, a2)) { fprintf(cptout, "\t<"); print_action(a1); fprintf(cptout, ", "); print_action(a2); fprintf(cptout, ">\n"); }
    } EFORPAIR;
    fprintf(cptout, "\n\n");
    if (first == direction) { fprintf(cptout, "*** Ordering choice : ["); print_action(a1); fprintf(cptout, " << "); print_action(a2); fprintf(cptout, "]\n\n"); }
    else { fprintf(cptout, "*** Ordering choice : ["); print_action(a2); fprintf(cptout, " << "); print_action(a1); fprintf(cptout, "]\n\n"); }      
  }
}

void trace_start_time_choice(Action *a, bool direction, bool first)
{
  if (first) {
    fprintf(cptout, "*** Starting time choice : "); print_action(a); fprintf(cptout, " among\n");
    FOR(a, active_actions) {
      if (first_start(a) < last_start(a)) { fprintf(cptout, "\t"); print_action(a); fprintf(cptout, "\n"); }
    } EFOR;
    fprintf(cptout, "\n\n");
  }
}

void trace_action_used(Action *a)
{
  fprintf(cptout, "use "); print_action(a); fprintf(cptout, "\n");
}

void trace_action_excluded(Action *a)
{
  fprintf(cptout, "exclude "); print_action(a); fprintf(cptout, "\n");
}

void trace_order_before_aa(Action *a1, Action *a2)
{
  fprintf(cptout, "order before "); print_action(a1); fprintf(cptout, " --> "); print_action(a2); fprintf(cptout, "\n");
}

void trace_order_before_ca(Causal *c, Action *a)
{
  fprintf(cptout, "order before "); print_causal(c); fprintf(cptout, " --> "); print_action(a); fprintf(cptout, "\n");
}

void trace_order_before_ac(Action *a, Causal *c)
{
  fprintf(cptout, "order before "); print_action(a); fprintf(cptout, " --> "); print_causal(c); fprintf(cptout, "\n");
}

void trace_propagate_action(Action *a)
{
  fprintf(cptout, "propagate action "); print_action(a); fprintf(cptout, "\n");
}

void trace_propagate_causal(Causal *c)
{
  fprintf(cptout, "propagate causal on "); print_causal(c); fprintf(cptout, "\n");
}

void trace_synchronize_causal(Causal *c)
{
  fprintf(cptout, "synchronize "); print_causal(c); fprintf(cptout, "\n"); 
}

void trace_protect_causal(Causal *c)
{
    fprintf(cptout, "protect causal "); print_causal(c); fprintf(cptout, "\n");
}

void trace_protect_against(Action *a)
{
  fprintf(cptout, "protect against action "); print_action(a); fprintf(cptout, "\n");
}

void trace_rem_producer(Causal *c, Action *a)
{
  fprintf(cptout, "\t"); print_causal(c); fprintf(cptout, "  !=  "); print_action(a); fprintf(cptout, "\n");
}

void trace_producer_used(Causal *c)
{
  Action *prod = get_producer(c);

  if (!prod->used) { fprintf(cptout, "\t"); print_causal(c); fprintf(cptout, "  ==  "); print_action(prod); fprintf(cptout, "\n"); }
}

void trace_update_sup_a(Action *a, TimeVal i)
{
  if (last_start(a) > i) { fprintf(cptout, "update sup "); print_action(a); fprintf(cptout, " : "); print_time(cptout, i); fprintf(cptout, "\n"); }
}

void trace_update_sup_c(Causal *c, TimeVal i)
{
  if (last_start(c) > i) { fprintf(cptout, "update sup "); print_causal(c); fprintf(cptout, " : "); print_time(cptout, i); fprintf(cptout, "\n"); }
}

void trace_update_inf_a(Action *a, TimeVal i)
{
  if (first_start(a) < i) { fprintf(cptout, "update inf "); print_action(a); fprintf(cptout, " : "); print_time(cptout, i); fprintf(cptout, "\n"); }
}

void trace_update_inf_c(Causal *c, TimeVal i)
{
  if (first_start(c) < i) { fprintf(cptout, "update inf "); print_causal(c); fprintf(cptout, " : "); print_time(cptout, i); fprintf(cptout, "\n"); }
}

