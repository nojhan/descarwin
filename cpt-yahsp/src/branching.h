/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : branching.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#ifndef BRANCHING_H
#define BRANCHING_H


extern Causal *last_conflict_candidate;
EVECTOR(Causal *, last_conflicts);

#define support_threat(c, a) (a->used && a != c->consumer && !can_precede_ca(c, a) && !can_precede_ac(a, c))
#define mutex_threat(a1, a2) (a1 != a2 && amutex(a1, a2) && !can_precede_aa(a1, a2) && !can_precede_aa(a2, a1))

extern void init_heuristics(void);
extern void reset_last_conflicts(void);
extern void reset_weights(void);
extern void search(void);


#endif /* BRANCHING_H */
