/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : yahsp.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#ifndef YAHSP_H
#define YAHSP_H

extern void yahsp_init();
extern int yahsp_main();
extern void yahsp_reset();
extern int yahsp_search(Fluent **init, long init_nb, Fluent **goals, long goals_nb);
extern int yahsp_compress_plans();

/* Ajouté par Pierre Savéant et Johann Dréo le 2010-03-26 */
BitArray * get_current_state();
/* PS & JD */

#endif /* YAHSP_H */
