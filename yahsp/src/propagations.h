/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : propagations.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#ifndef PROPAGATIONS_H
#define PROPAGATIONS_H


extern void propagate_action(Action *a);
extern void propagate_causal(Causal *c);
extern void use_action(Action *a);
extern void exclude_causal(Causal *c); 
extern void exclude_action(Action *a);
extern void order_before_aa(Action *a1, Action *a2);
extern void order_before_ca(Causal *c, Action *a);
extern void order_before_ac(Action *a, Causal *c);
extern void make_precede(Action *a1, Action *a2);
extern void more_propagations(void);
extern void clone_action(Action *a, Causal *causal);
extern void restrict_problem(Fluent **init, long init_nb, Fluent **goal, long goal_nb);
extern void evaluate_ac_forward(ActivityConstraint *ac, TimeVal fs, TimeVal ls, TimeVal *acmin, TimeVal *acmax);
extern void evaluate_ac_backward(ActivityConstraint *ac, TimeVal fs, TimeVal ls, TimeVal *acmin, TimeVal *acmax);


#endif /* PROPAGATIONS_H */
