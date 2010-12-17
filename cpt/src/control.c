/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : control.c
 *
 * Copyright (C) 2005-2009  Vincent Vidal <vidal@cril.univ-artois.fr>
 */


#include "cpt.h"
#include "structs.h"
#include "problem.h"
#include "globs.h"


void control_new_world(void) { }

void control_backtrack(void) { }

void control_begin_bound(TimeVal bound) { }

void control_end_bound(long nodes, long backtracks, double time) { }

void control_restart(long nodes, long backtracks, double time, TimeVal bound) { }

void control_problem_stats(long actions_nb, long fluents_nb, long causals_nb)
{
  char c;
  while ((c = getchar()) != 'q') {
    switch (c) {
    case 'a': FOR(a, actions) { printf("%s\n", action_name(a)); } EFOR; break;
    case 'f': FOR(f, fluents) { printf("%s\n", fluent_name(f)); } EFOR; break;
    }
  }
}

void control_solution_plan(void) { }

void control_search_stats(bool complete, double search_time, double total_time) { }

void control_conflict_choice(Causal *c, Action *a, bool direction, bool first) { }

void control_support_choice(Causal *c, Action *a, bool direction, bool first) { }

void control_mutex_choice(Action *a1, Action *a2, bool direction, bool first) { }

void control_action_used(Action *a) { }

void control_action_excluded(Action *a) { }

void control_order_before_aa(Action *a1, Action *a2) { }

void control_order_before_ca(Causal *c, Action *a) { }

void control_order_before_ac(Action *a, Causal *c) { }

void control_propagate_action(Action *a) { }

void control_propagate_causal(Causal *c) { }

void control_synchronize_causal(Causal *c) { }

void control_protect_causal(Causal *c) { }

void control_protect_against(Action *a) { }

void control_rem_producer(Causal *c, Action *a) { }

void control_producer_used(Causal *c) { }

void control_update_sup_a(Action *a, TimeVal i) { }

void control_update_sup_c(Causal *c, TimeVal i) { }

void control_update_inf_a(Action *a, TimeVal i) { }

void control_update_inf_c(Causal *c, TimeVal i) { }

