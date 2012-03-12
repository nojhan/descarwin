/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : trace_planner.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#include "cpt.h"
#include "options.h"
#include "structs.h"
#include "problem.h"
#include "branching.h"
#include "globs.h"
#include "trace_planner.h"
#include "yahsp.h"
#include "yahsp-mpi.h"


/*****************************************************************************/


void trace_world(const char *message, int sep) {
  char world[100];
  size_t cols;
  size_t i;

  if (tgetent(NULL, getenv("TERM")) < 0) cols = 80;
  else cols = tgetnum("co");
  sprintf(world, " %s : %lu ", message, current_world());
  fprintf(cptout, "\n");
  for(i = 0; i < (cols - strlen(world)) / 2; i++) fputc(sep, cptout);
  fprintf(cptout, "%s", world);
  for(i += strlen(world); i < cols; i++) fputc(sep, cptout);
  fprintf(cptout, "\n\n");
}

void trace_new_world()
{
  trace_world("New World", '=');
}
void trace_backtrack(void) 
{
  trace_world("Backtrack to World", '-');
}

void trace_begin_bound(TimeVal bound)
{
  fprintf(cptout, "Bound : ");
  print_time(cptout, bound - (opt.pddl21 ? pddl_domain->precision.t : 0));
  fprintf(cptout, "  ---  ");
}

void trace_end_bound(ulong nodes, ulong backtracks, double time)
{
  fprintf(cptout, "Nodes : %lu  ---  Backtracks : %lu  ---  Iteration time : %.3f\n", nodes, backtracks, time);
}

void trace_restart(ulong nodes, ulong backtracks, double time, TimeVal bound)
{
  fprintf(cptout, "%lu  ---  %lu  ---  %.3f\nBound : ", nodes, backtracks, time);
  print_time(cptout, bound - (opt.pddl21 ? pddl_domain->precision.t : 0));
  fprintf(cptout, "  ---  ");
}

void trace_problem_stats(size_t actions_nb, size_t fluents_nb, size_t causals_nb)
{
  fprintf(cptout, "\nProblem : %zu actions, %zu fluents, %zu causals\n          %zu init facts, %zu goals\n\n",
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
    fprintf(cptout, "\nLength : %lu\n", plan->length);
  }
}

void trace_search_stats()
{
  if (opt.verbosity > 0) {
    if (opt.yahsp) {
      fprintf(cptout, "Computed nodes : %lu\n", stats.computed_nodes);
      fprintf(cptout, "Evaluated nodes : %lu\n", stats.evaluated_nodes);
      fprintf(cptout, "Expanded nodes : %lu\n", stats.expanded_nodes);
      fprintf(cptout, "Evaluated nodes/sec : %.3f\n", stats.nodes_by_sec);
#ifdef YAHSP_MPI
      fprintf(cptout, "Evaluated nodes/sec/proc : %.3f\n", stats.nodes_by_sec_proc);
      fprintf(cptout, "Messages sent: %lu\n", stats.sends);
      fprintf(cptout, "Messages sent/ev. node : %.3f\n", stats.sends / (double) stats.evaluated_nodes);
      fprintf(cptout, "Bytes sent: %lu\n", stats.data_sent);
      fprintf(cptout, "Min ev. nodes/proc : %lu\n", stats.min_eval_proc);
      fprintf(cptout, "Max ev. nodes/proc : %lu\n", stats.max_eval_proc);
#endif
#ifdef YAHSP_MT
      fprintf(cptout, "Min ev. nodes/thread : %lu\n", stats.min_eval_thread);
      fprintf(cptout, "Max ev. nodes/thread : %lu\n", stats.max_eval_thread);
#endif
    } else {
      fprintf(cptout, "Nodes : %lu\n", nb_nodes());
      fprintf(cptout, "Backtracks : %lu\n", nb_backtracks());
      fprintf(cptout, "Support choices : %lu\n", stats.support_choices);
      fprintf(cptout, "Conflict choices : %lu\n", stats.conflict_choices);
      fprintf(cptout, "Mutex choices : %lu\n", stats.mutex_choices);
      fprintf(cptout, "Start time choices : %lu\n", stats.start_time_choices);
      fprintf(cptout, "World size : %luK\n", world_size() / 1000);
      fprintf(cptout, "Nodes/sec : %.3f\n", nb_nodes() / stats.usearch);
    }
#ifdef YAHSP_MT
#ifdef YAHSP_MPI
    fprintf(cptout, "Core utility : %.3f\n", mini(100.0, stats.usearch * 100 / (stats.wsearch * mpi_get_numtasks() * opt.yahsp_threads * opt.yahsp_teams)));
#else
    fprintf(cptout, "Core utility : %.3f\n", mini(100.0, stats.usearch * 100 / (stats.wsearch * mini(opt.yahsp_threads * opt.yahsp_teams, omp_get_num_procs()))));
#endif
#endif
    fprintf(cptout, "Search us time : %.3f\n", stats.usearch);
    fprintf(cptout, "Search wc time : %.3f\n", stats.wsearch);
    fprintf(cptout, "Total us time : %.3f\n", stats.utotal);
    fprintf(cptout, "Total wc time : %.3f\n", stats.wtotal);
    fprintf(cptout, "\n");
  }
}

void trace_yahsp_anytime_search_stats(TimeVal makespan, double search, double total)
{
  fprintf(cptout, "plan found : ");
  if (pddl_domain->action_costs) fprintf(cptout, "cost ");
  else if (opt.sequential) fprintf(cptout, "length ");
  else fprintf(cptout, "makespan ");
  print_time(cptout, makespan);
  fprintf(cptout, " -- search time %.3f -- total time %.3f\n", search, total);
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
