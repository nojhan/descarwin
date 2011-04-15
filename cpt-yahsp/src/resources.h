/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : resources.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#ifndef RESOURCES_H
#define RESOURCES_H


extern void propagate_resource(Action *a);
extern void exclude_resource(Action *a);
extern void protect_causal_resource(Causal *c);


#endif /* RESOURCES_H */
