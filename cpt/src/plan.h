/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : plan.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#ifndef PLAN_H
#define PLAN_H


#define PLAN_FOUND 0
#define GOALS_MUTEX 1
#define INIT_PROP_FAILED 2
#define BACKTRACK_LIMIT 3
#define BOUND_LIMIT 4
#define NO_PLAN 5


#ifdef WALLCLOCK_TIME

#include <time.h>
#define TIMER(x) static clock_t x##_start
#define start_timer(x) NEST( x##_start = clock(); )
#define get_timer(x) ((clock() - x##_start) / (double) CLOCKS_PER_SEC)

#else

#include <sys/time.h>
#include <sys/times.h>
#define TIMER(x) struct tms x##_start, x##_end
#define start_timer(x) (void) times(&x##_start)
#define get_timer(x) ({ (void) times(&x##_end); ((double) (x##_end.tms_utime - x##_start.tms_utime) / sysconf(_SC_CLK_TCK)); })

#endif


typedef struct SolutionPlan SolutionPlan;
typedef struct Step Step;
typedef struct Statistics Statistics;

struct SolutionPlan {
  VECTOR(Step *, steps);
  TimeVal makespan;
  double search_time;
  double total_time;
  double backtracks;
};

struct Step {
  Action *action;
  VECTOR(long, causals);
  VECTOR(long, before);
  TimeVal init;
  TimeVal end;
#ifdef RESOURCES
  TimeVal min_level;
  TimeVal max_level;
#endif
};


struct Statistics {
  /* CPT statistics */
  long support_choices;
  long conflict_choices;
  long mutex_choices;
  long start_time_choices;
  /* YAHSP statistics */
  long computed_nodes;
  long evaluated_nodes;
  long expanded_nodes;
  TIMER(monitor);
  TIMER(search);
  TIMER(total);
  TIMER(iteration);
  double wctime;
  double wcsearch;
};


extern void print_plan(FILE *file, SolutionPlan *plan, bool print_synchro);
extern void print_plan_ipc(FILE *file, SolutionPlan *plan, double total_time);
extern void print_plan_ipc_anytime(SolutionPlan *plan);
extern SolutionPlan *plan_save(Action **actions, long actions_nb, double search_time);
extern void plan_free(SolutionPlan *plan);
extern void compress_plans(bool causals, bool orderings);
extern int precedes_in_plan(const void *s1, const void *s2);

extern void plan_free_static(SolutionPlan *plan);
extern SolutionPlan plan_copy_static(SolutionPlan plan);

#endif /* PLAN_H */
