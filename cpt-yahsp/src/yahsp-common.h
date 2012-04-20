/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : yahsp-common.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


extern void alloc_eval_structures();
extern void free_eval_structures();
extern Node *create_node(State state);
extern void node_free(Node *node);
extern Node *node_derive(Node *node);
extern void node_compute_key(Node *node);
extern void copy_applicable_actions(Node *node);
extern TimeVal get_action_cost(Action *a);
extern TimeVal get_node_fvalue(Node *node);
extern bool node_apply_action(Node *node, Action *a);
extern void compute_h1(Node *node);
extern void compute_relaxed_plan(Node *node);
extern Node *apply_relaxed_plan(Node *node, TimeVal best_makespan);
extern bool action_must_precede(Action *a1, Action *a2);
extern SolutionPlan *create_solution_plan(Node *node);
extern void yahsp_trace_anytime(Node *node);
