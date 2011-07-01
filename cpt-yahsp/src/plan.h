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
#define get_wtimer(x) ((clock() - x##_start) / (double) CLOCKS_PER_SEC)

#else

#include <sys/time.h>
#include <sys/times.h>
#define TIMER(x) struct tms x##_start, x##_end; struct timeval x##_wstart, x##_wend
#define start_timer(x) NEST((void) times(&x##_start); gettimeofday(&x##_wstart, NULL);)
#define get_timer(x) ({ (void) times(&x##_end); ((double) (x##_end.tms_utime - x##_start.tms_utime) / sysconf(_SC_CLK_TCK)); })
#define get_wtimer(x) ({ (void) gettimeofday(&x##_wend, NULL); ((double) (x##_wend.tv_sec - x##_wstart.tv_sec + (x##_wend.tv_usec - x##_wstart.tv_usec) / (double) 1000000)); })

#endif


typedef struct SolutionPlan SolutionPlan;
typedef struct Step Step;
typedef struct Statistics Statistics;

struct SolutionPlan {
  VECTOR(Step *, steps);
  TimeVal makespan;
  double search_time;
  double total_time;
  long backtracks;
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
  long page_faults;
  long vol_context_switch;
  long inv_context_switch;
  long no_work_counter;
  long sends;
  long sends_per_proc;

  double nodes_by_sec;
  double nodes_by_sec_proc;
  TIMER(monitor);
  TIMER(search);
  TIMER(total);
  TIMER(iteration);
};


extern void print_plan(FILE *file, SolutionPlan *plan, bool print_synchro);
extern void print_plan_ipc(FILE *file, SolutionPlan *plan, double total_time);
extern void print_plan_ipc_anytime(SolutionPlan *plan);
extern SolutionPlan *plan_save(Action **actions, long actions_nb, double search_time);
extern void plan_free(SolutionPlan *plan);
extern void compress_plans(bool causals, bool orderings);
extern Comparison precedes_in_plan(Step **s1, Step **s2);

extern void plan_free_static(SolutionPlan *plan);
extern SolutionPlan plan_copy_static(SolutionPlan plan);

#endif /* PLAN_H */
