/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : yahsp-dae.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#ifndef YAHSP_DAE_H
#define YAHSP_DAE_H

#ifdef DAE
#undef check_allocation
#define check_allocation(ptr, x, res) ({ if (x > 0) { if (!(ptr = (typeof(ptr)) res) && opt.dae && ({ heuristic_flush(); !(ptr = (typeof(ptr)) res); })) error(allocation, "Memory allocation error"); } else ptr = NULL; ptr; })
#endif

extern void heuristic_create();
extern void heuristic_insert(Node *node, TimeVal *finit);
extern bool heuristic_search(Node *node, TimeVal *finit);
extern void heuristic_flush();
extern int yahsp_compress_plans();


#endif /* YAHSP_DAE_H */
