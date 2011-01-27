/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : solve.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#include "cpt.h"
#include "structs.h"
#include "problem.h"
#include "plan.h"
#include "propagations.h"
#include "branching.h"
#include "max_atom.h"
#include "preprocess.h"
#include "scheduling.h"
#include "yahsp.h"
#ifdef RATP
#include "ratp.h"
#endif
#include "globs.h"
#include "solve.h"
#include "trace_planner.h"
#ifdef RESOURCES
#include "resources.h"
#endif


/*---------------------------------------------------------------------------*/
/* Global Variables                                                          */
/*---------------------------------------------------------------------------*/


PDDLDomain *pddl_domain;

VECTOR(Action *, actions);
VECTOR(Fluent *, fluents);
VECTOR(Causal *, causals);
VECTOR(Action *, active_actions);
VECTOR(Causal *, active_causals);
#ifdef RESOURCES
VECTOR(Resource *, resources);
#endif
long total_actions_nb;
TimeVal total_plan_cost;

Action *start_action;
Action *end_action;

VECTOR(Fluent *, init_state);
VECTOR(Fluent *, goal_state);

SolutionPlan *solution_plan;
VECTOR(SolutionPlan *, plans);

Statistics stats;
 
Causal *last_conflict_candidate;
VECTOR(Causal *, last_conflicts);

#ifdef DAE
FILE *cptout = stderr;
#else
FILE *cptout = stdout;
#endif


/*---------------------------------------------------------------------------*/
/* Local Macros                                                              */
/*---------------------------------------------------------------------------*/


#define print_trace(first_run) NEST( if (!first_run) trace_proc(restart, nb_nodes_run(), nb_backtracks_run(), get_timer(iteration), last_start(end_action)); )


/*---------------------------------------------------------------------------*/
/* Static Functions                                                          */
/*---------------------------------------------------------------------------*/


static void solve2(void);
#ifndef WALLCLOCK_TIME
static void partial_statistics_request(int n);
static void user_interruption(int n);
static void timer_interruption(int n);
#endif

/*****************************************************************************/


void _begin_monitor(const char *s) 
{
  long i; 
  start_timer(stats.monitor);
  trace(monitor, "%s", s);
  for (i = 0; i < 34 - (long) strlen(s); i++) trace(monitor, ".");
}

void _end_monitor(void) 
{
  trace(monitor, " done : %.2f          \n", get_timer(stats.monitor)); 
}

#ifndef WALLCLOCK_TIME

static void init_system(long time_limit)
{
  struct itimerval timer;
  signal(SIGTSTP, partial_statistics_request);
  signal(SIGINT, user_interruption);
  signal(SIGVTALRM, timer_interruption);
  if (opt.timer > 0) {
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    timer.it_value.tv_sec = time_limit;
    timer.it_value.tv_usec = 0;
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
  }
}

static void partial_statistics_request(int n)
{
  int v = opt.verbosity;
  opt.verbosity = 1;
  trace(normal, "\n----- Partial statistics at bound ");
  print_time(cptout, last_start(end_action));
  trace(normal, " -----\n");
  trace_proc(search_stats, get_timer(stats.search), get_timer(stats.total));
  opt.verbosity = v;
}

static void user_interruption(int n)
{
  error(user_interruption, "User interruption");
}

static void timer_interruption(int n)
{
  error(timer_interruption, "Timer interruption");
}

#endif


int cpt_main(int argc, const char **argv)
{
  cmd_line(argc, argv);
  
#ifdef RATP
  read_ratp_problem();
#endif

  init_heuristics();

  start_timer(stats.total);

  stats.wctime = omp_get_wtime();

#ifndef WALLCLOCK_TIME
  init_system(opt.timer);
#endif
  
  create_problem();

  fesetround(FE_UPWARD);

  stats.wcsearch = omp_get_wtime();
  
  if (opt.yahsp) {
    yahsp_init();
    start_timer(stats.search);
  }

  if (opt.dae) return 0;

#ifdef RATP
  return 0;
#endif

  switch (cpt_basic_search()) {
  case GOALS_MUTEX : error(no_plan, "The goals are mutually exclusive");
  case INIT_PROP_FAILED : error(no_plan, "Initial propagation failed");
  case BOUND_LIMIT : error(no_plan,"No plan of makespan lower than upper limit of the bound");
  case BACKTRACK_LIMIT : 
    if (!solution_plan || !opt.give_suboptimal) error(no_plan, "The maximum number of backtracks is reached"); 
    trace(normal, "\nWARNING : The maximum number of backtracks is reached before proving optimality !\n");
  case PLAN_FOUND :
    trace_proc(solution_plan, solution_plan);
    trace_proc(plan_stats, solution_plan);
    if (opt.output_file != NULL) {
      FILE *file = fopen(opt.output_file, "w");
      print_plan_ipc(file, solution_plan, get_timer(stats.total));
      fclose(file);
    }
    break;
  }
  trace_proc(search_stats, get_timer(stats.search), get_timer(stats.total));

  return PLAN_FOUND;
}


int cpt_basic_search(void)
{
  /* char *goals[]={ */
  /*   "(fuel-level plane2 fl4)",  */
  /*   "(fuel-level plane1 fl1)",  */
  /*   "(at plane3 city4)",  */
  /*   "(at plane1 city3)",  */
  /*   "(fuel-level plane3 fl0)"}; */

  /* goal_state_nb = 0; */

  /* int i; */
  /* for (i=0; i < 5; i++) { */
  /*   FOR(f, fluents) { */
  /*     char *n = fluent_name(f); */
  /*     if (strcmp(n, goals[i]) == 0)  */
  /* 	  goal_state[goal_state_nb++] = f; */
  /*   } EFOR; */
  /* } */

  /* FOR(f, goal_state) { */
  /*   printf("%s %d\n", fluent_name(f), f->id); */
  /* } EFOR; */

  /* return cpt_search(init_state, init_state_nb, goal_state, goal_state_nb, false, false, false); */

  return cpt_search(NULL, -1, NULL, -1, false, false, false);
}

int cpt_search(Fluent **init, long init_nb, Fluent **goals, long goals_nb, 
	       bool compress, bool compress_causals, bool compress_orderings)
{
  FORPAIR(f1, f2, goals) { if (fmutex(f1, f2)) return GOALS_MUTEX; } EFORPAIR;

  if (init_nb != -1) {
    start_action->add = init;
    start_action->add_nb = init_nb;
    end_action->prec = goals;
    end_action->prec_nb = goals_nb;
  }

  solution_plan = NULL;

  if (opt.yahsp) {
    if (!compress) return yahsp_main();
    else return yahsp_compress_plans();
  }

  TimeVal bound, dic_increment = 2, dic_lower = 0, dic_upper = 0, maxmsp = pddl_domain->max_makespan.t;
  long actions_nb_orig = actions_nb, fluents_nb_orig = fluents_nb, causals_nb_orig = causals_nb;
  long old_nodes, old_backtracks;
#ifndef RATP
  bool initial_prop_backtrackable = opt.dae;
#else
  bool initial_prop_backtrackable = opt.dae || opt.ratp;
#endif

  begin_monitor("Initial propagations");
  if ((initial_prop_backtrackable && new_world(false)) || (!initial_prop_backtrackable && protected())) {
    if (compress) compress_plans(compress_causals, compress_orderings);
    set_limit_initprop(opt.limit_initial_propagation, opt.max_propagations);
    restrict_problem(init, init_nb, goals, goals_nb);
  } else return INIT_PROP_FAILED;
  end_monitor();

  if (opt.limit_initial_propagation && propagations_limit_reached()) {
/*     restore_initial_world(0); */
/*     return INIT_PROP_FAILED;  */
  }
  set_limit_initprop(false, 0);

  /* landmarks : avant ou après la première propagation ??? */
  if (opt.landmarks) {
    begin_monitor("Computing landmarks");
    compute_landmarks();
    end_monitor();
  }
  
  trace_proc(problem_stats, actions_nb_orig, fluents_nb_orig, causals_nb_orig);

  bound = dic_lower = dic_upper = maxi(pddl_domain->bound.t + (opt.pddl21 ? pddl_domain->precision.t : 0), first_start(end_action));
  if (bound > maxmsp) {
    restore_initial_world(0);
    return BOUND_LIMIT;
  }
  
  if (opt.dichotomy != 0) {
    TimeVal mind = MAXTIME, maxd = 0;
    long nbac = 0;
    double mean = 0;
    FORMIN(a, actions, 2) {
      if (!opt.pddl21 || a->ope->real_duration) {
	TimeVal dur = opt.pddl21 ? a->rdur.t : duration(a);
	minimize(mind, dur);
	maximize(maxd, dur);
	mean += dur; 
	nbac++;
      }
    } EFOR;
    mean = ceil(mean / maxi(nbac, 1));
    if (mind == MAXTIME) mind = maxd = 1;
    switch (opt.dichotomy) {
    case 1: dic_increment = mind; break;
    case 2: dic_increment = mean; break;
    case 3: dic_increment = (maxd - mind); break;
    case 4: dic_increment = maxd; break;
    }
    maximize(dic_increment, 1);
    dic_increment *= 2;
  }

  FOR(c, causals) { c->weight = 0; } EFOR;
  FOR(a, actions) { a->weight = 0; } EFOR;
  last_conflict_candidate = NULL;
  last_conflicts_nb = 0;

  start_timer(stats.search);

  set_backtrack_limit(opt.max_backtracks);
  reset_backtracks();

  // not useful due to global mutex sets ?
  if (opt.sequential) {
    propagate();
    //maximize(bound, active_actions_nb - 2);
    maximize(bound, total_plan_cost);
    dic_lower = dic_upper = bound;
  }

  while (true) {
    start_timer(stats.iteration);
    old_nodes = nb_nodes();
    old_backtracks = nb_backtracks();
    trace_proc(begin_bound, bound);
    if (new_world(false)) {
      //update_inf_a(end_action, bound);
      update_sup_a(end_action, bound);
      compute_relevance(bound);
      if (opt.shaving) { propagate(); shaving(); }
      last_conflict_candidate = NULL;
      last_conflicts_nb = 0;
/*       FOR(c, causals) { c->weight = 0; } EFOR; */
/*       FOR(a, actions) { a->weight = 0; } EFOR; */
      solve2();
      /* print_time(cptout, first_start(end_action)); printf("\n"); */
      if (solution_plan) plan_free(solution_plan);
      solution_plan = plan_save(active_actions, active_actions_nb, get_timer(stats.search));
      solution_plan->backtracks = nb_backtracks();
      dic_upper = maxi(first_start(end_action), dic_lower);
      trace_proc(end_bound, nb_nodes() - old_nodes, nb_backtracks() - old_backtracks, get_timer(stats.iteration));
      restore_initial_world(opt.pb_restrict ? 0 : 1); 
    } else {
      trace_proc(end_bound, nb_nodes() - old_nodes, nb_backtracks() - old_backtracks, get_timer(stats.iteration));
      if (!solution_plan) dic_upper = mini(bound + dic_increment, (maxmsp == MAXTIME ? MAXTIME : (maxmsp - bound) + maxmsp));
      dic_lower = bound;
    }
    if ((opt.limit_backtracks_all && backtrack_limit_reached()) || (opt.limit_initial_propagation && propagations_limit_reached())) {
      restore_initial_world(0); 
      if (solution_plan) {
	if (!opt.give_suboptimal) plan_free(solution_plan); 
	else solution_plan->backtracks = nb_backtracks();
      }
      return BACKTRACK_LIMIT; 
    }
    if (dic_upper - dic_lower <= 1) {
      restore_initial_world(0);
      return solution_plan ? PLAN_FOUND : BOUND_LIMIT;
    }
    bound = (dic_lower + dic_upper + 1) / 2;
  }
}

static void solve2(void) 
{  
  bool first_run = true;
  long tries = opt.restarts_max_tries;
  TIMER(iteration);

  if (opt.limit_backtracks) set_backtrack_limit(opt.max_backtracks);
  else if (opt.restarts) set_backtrack_limit(opt.restarts_init);
  while (true) {
    start_timer(iteration);
    if (new_world(false)) {
      search();
      print_trace(first_run);
      break;
    } else { 
      if (!backtrack_limit_reached() || (opt.limit_backtracks_all && backtrack_limit_reached()) || opt.limit_backtracks) {
	print_trace(first_run);
	contradiction();
      }
      first_run = false;
      print_trace(first_run);
      if (opt.restarts) {
	if (--tries == 0) set_backtrack_limit(LONG_MAX);
	else set_backtrack_limit(maxi((long) ((double) backtrack_limit() * opt.restarts_factor), backtrack_limit() + opt.restarts_min_increment));
      }
    }
  }
}


