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


#define TIMER(x) struct timespec x##_start, x##_end, x##_wstart, x##_wend
#define start_timer(x) NEST( clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &x##_start); clock_gettime(CLOCK_REALTIME, &x##_wstart); )
#define get_timer(x) ({ clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &x##_end); clock_gettime(CLOCK_REALTIME, &x##_wend); ((x##_end.tv_sec - x##_start.tv_sec  + (x##_end.tv_nsec - x##_start.tv_nsec) / (double) 1e9)); })
#define get_wtimer(x) ({ clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &x##_end); clock_gettime(CLOCK_REALTIME, &x##_wend); ((x##_wend.tv_sec - x##_wstart.tv_sec  + (x##_wend.tv_nsec - x##_wstart.tv_nsec) / (double) 1e9)); })


typedef struct SolutionPlan SolutionPlan;
typedef struct Step Step;
typedef struct Statistics Statistics;

struct SolutionPlan {
  VECTOR(Step *, steps);
  ulong length;
  TimeVal makespan;
  TimeVal cost_add;
  TimeVal cost_max;
  double search_time;
  double total_time;
  ulong backtracks;
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

typedef struct AdamState AdamState;
typedef struct Adam Adam;

struct AdamState {
  VECTOR(Fluent *,fluents);
};

struct Adam {
  VECTOR(AdamState, states);
};

struct Statistics {
  /* CPT statistics */
  ulong support_choices;
  ulong conflict_choices;
  ulong mutex_choices;
  ulong start_time_choices;
  /* YAHSP statistics */
  ulong computed_nodes;
  ulong evaluated_nodes;
  ulong expanded_nodes;
  double nodes_by_sec;
  double nodes_by_sec_proc;
  TIMER(monitor);
  TIMER(search);
  TIMER(total);
  TIMER(iteration);
  double usearch;
  double wsearch;
  double utotal;
  double wtotal;
#ifdef YAHSP_MPI
  ulong sends;
  ulong sends_per_proc;
  ulong data_sent;
  ulong min_eval_proc;
  ulong max_eval_proc;
#endif  
#ifdef YAHSP_MT
  ulong min_eval_thread;
  ulong max_eval_thread;
#endif  
};


extern void print_plan(FILE *file, SolutionPlan *plan, bool print_synchro);
extern void print_plan_ipc(FILE *file, SolutionPlan *plan, double total_time);
extern void print_plan_ipc_anytime(SolutionPlan *plan);
extern SolutionPlan *plan_save(Action **actions, size_t actions_nb, double search_time);
extern void plan_free(SolutionPlan *plan);
extern void compress_plans(bool causals, bool orderings);
extern SolutionPlan *read_plan_from_file(char *filename);


#endif /* PLAN_H */
