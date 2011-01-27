/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : options.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#ifndef OPTIONS_H
#define OPTIONS_H

typedef struct Option Option;

typedef enum { CONFLICTS_FIRST, SUPPORTS_FIRST, MUTEX_FIRST } BranchingStrategy;

extern int global_argc;
extern const char **global_argv;

struct Option {
  bool anytime;
  char *bound;
  bool bad_supporters_pruning;
  BranchingStrategy branching_strategy;
  bool complete_qualprec;
  bool dae;
  long dichotomy;
  bool distance_boosting;
  long distances;
  bool computing_distances;
  char *ops_file;
  char *facts_file;
  bool fluent_mutexes;
  bool give_suboptimal;
  bool global_mutex_sets;
  long initial_heuristic;
  bool landmarks;
  long last_conflicts;
  bool limit_backtracks;
  bool limit_backtracks_all;
  bool limit_initial_propagation;
  long max_backtracks;
  long max_plan_length;
  long max_propagations;
  bool local_mutex_sets;
  char *max_makespan;
  bool optimal;
  char *output_file;
  bool pb_restrict;
  bool pddl21;
  char *precision;
  char *precision2;
  bool print_actions;
  bool propagate_inactive_causals;
  bool random;
  bool rationals;
#ifdef RATP
  bool ratp;
  long ratp_incr;
  long ratp_interval_incr;
  long ratp_interval_incr_optim;
  char *ratp_input;
  char *ratp_output;
#endif
  bool read_actions;
  char* read_actions_input;
  bool relevance;
  bool restarts;
  long restarts_init;
  double restarts_factor;
  long restarts_min_increment;
  long restarts_max_tries;
  bool shaving;
  long seed;
  bool sequential;
  bool task_intervals;
  long timer;
  bool unique_supports;
  bool verbose_preprocessing;
  long verbosity;
  bool wdeg;
  bool yahsp;
  long yahsp_threads;
  long yahsp_teams;
  long yahsp_strategy;
};

extern Option opt;

extern void cmd_line(int argc, const char **argv);


#endif /* #define OPTIONS_H */
