/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : globs.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#ifndef GLOBS_H
#define GLOBS_h


#include "structs.h"
#include "plan.h"


EVECTOR(Action *, actions);
EVECTOR(Fluent *, fluents);
EVECTOR(Causal *, causals);
EVECTOR(Action *, active_actions);
EVECTOR(Causal *, active_causals);
#ifdef RESOURCES
EVECTOR(Resource *, resources);
#endif
extern size_t total_actions_nb;
extern TimeVal total_plan_cost;

extern Action *start_action;
extern Action *end_action;
// DIRTY
extern Action yend_action;


EVECTOR(Fluent *, init_state);
EVECTOR(Fluent *, goal_state);

extern SolutionPlan *solution_plan;
EVECTOR(SolutionPlan *, plans);

extern Statistics stats;

extern Causal *last_conflict_candidate;
EVECTOR(Causal *, last_conflicts);

extern FILE *cptout;

extern struct drand48_data random_buffer;

#ifdef YAHSP_MT
#pragma omp threadprivate(random_buffer)
#ifdef DAE
#pragma omp threadprivate(stats, yend_action, solution_plan, plans, plans_nb)
#endif
#endif


#endif /* GLOBS_H */
