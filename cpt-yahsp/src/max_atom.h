/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : max_atom.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#ifndef MAX_ATOM_H
#define MAX_ATOM_H


extern void compute_reachable(void);
extern void compute_init_h0_cost(void);
extern void compute_init_h1_cost(void);
extern void compute_init_rh1_cost(void);
extern void compute_h1_distances(void);
extern void compute_h2_distances(void);
extern void compute_init_h2_cost(void);
extern void compute_init_edeletes(void);
extern void compute_relevance(TimeVal bnd);


#endif /* MAX_ATOM_H */
