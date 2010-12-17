/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : solve.h
 *
 * Copyright (C) 2005-2009  Vincent Vidal <vidal@cril.univ-artois.fr>
 */


#ifndef SOLVE_H
#define SOLVE_H 


extern int cpt_main(int argc, const char **argv);
extern int cpt_basic_search(void);
extern int cpt_search(Fluent **init, long init_nb, Fluent **goals, long goals_nb, bool compress, bool compress_causals, bool compress_orderings);


#endif /* SOLVE_H */
