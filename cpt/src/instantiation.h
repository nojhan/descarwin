/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : instantiation.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */

#ifndef INSTANTIATION_H
#define INSTANTIATION_H 


void instantiate_operators(PDDLDomain *domain);
Fluent *search_fluent(PDDLDomain *domain, PDDLPredicate *predicate, PDDLTerm **params);


#endif /* INSTANTIATION_H */
