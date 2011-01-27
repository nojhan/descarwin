/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : options.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#include <getopt.h>
#include "cpt.h"
#include "trace.h"
#include "options.h"
#include "globs.h"


/*---------------------------------------------------------------------------*/
/* Local Macros                                                              */
/*---------------------------------------------------------------------------*/


#define TEXT_WIDTH 80

#ifdef __WIN32__ 
#define GETOPT getopt_long
#else
#define GETOPT getopt_long_only
#endif


/*---------------------------------------------------------------------------*/
/* Local Variables                                                           */
/*---------------------------------------------------------------------------*/

Option opt;

typedef struct {
  const char *long_name;
  const char *short_name;
  const char *argument;
  char letter;
  const char *detail;
} CPTOption;

int global_argc;
const char **global_argv;

static long nb_options;

static CPTOption long_options[] = {
  {NULL, "Input"},
  {"ops", NULL, "<file>", 'o', "Domain file (typed/untyped PDDL)"},
  {"facts", NULL, "<file>", 'f', "Problem file (typed/untyped PDDL)"},

  {NULL, "General"},
  {"anytime", "any", NULL, 'N', "Anytime behavior."},
  {"plan-length", "pl", "<x>", 'l', "Maximum plan length (default 500). This value does not have to be set for canonical planning (see -c below)."},
  {"timer", NULL, "<x>", 't', "Maximum amount of user time in seconds for giving a solution."},
  {"output-file", "out", "<file>", 'H', "Output plan in file in IPC format."},
  {"pddl21", "p21", "<precision>", 'a', "PDDL2.1 semantics. The precision is the duration timepoints (e.g. 0.001), used for the 'epsilon' separating fluent modifiction."},
  {"temporal-precision", "tp", "<precision>", 'K', "Precision for non-PDDL2.1 temporal planning."},

  {NULL, "Bounds"},
  {"initial-heuristic", "ih", "<x>", 'i', "Heuristic for computing a lower bound of the makespan. Possible values are 0 (no heuristic, default), 1 (h1 heuristic), 2 (h2 heuristic, similar to temporal planning graph)."},
  {"bound", "binf", "<x>", 'b', "Sets initial bound on the makespan to at least x (default 0)."},
  {"max-makespan", "bsup", "<x>", 'm', "Maximum makespan."},

  {NULL, "Distances"},
  {"distances", NULL, "<x>", 'd', "Heuristic for computing distances between actions. Possible values are 0 (no distances), 1 (h1 heuristic, default), 2 (h2 heuristic)." },
  {"no-distance-boosting", "ndb", NULL, 'D', "Disables distance boosting at preprocessing."},

  {NULL, "Pruning"},
  {"propagate-inactive-causals", "pic", NULL, 'n', "Enables propagation to support variables not yet in the plan." },
  {"global-mutex-sets", "gms", NULL, 'G', "Enables global mutex sets."},
  {"local-mutex-sets", "lms", NULL, 'L', "Enables local mutex sets."},
  {"mutex-sets", "ms", NULL, 'M', "Enables all mutex sets."},
  {"no-relevance", "nr", NULL, 'R', "Disables relevance computation."},
  {"no-bad-supporters-pruning", "nbsp", NULL, 'B', "Disables bad supporters pruning at preprocessing."},
  {"no-unique-supports", "nus", NULL, 'U', "Disables unique supports pruning rule during constraint propagation."},
  {"complete-qualitative-precedences", "cqp", NULL, 'e', "Enables qualitative precedences over actions not yet in the plan."},
  {"enables-landmarks", "ul", NULL, 'A', "Enables landmarks."},
  {"task-intervals", "ti", NULL, 'T', "Enables task intervals pruning."},
  {"shaving", NULL, NULL, 'S', "Shaving."},
  {"fluent-mutexes", "fm", NULL, 'F', "Enables h2-based fluent mutexes in yahsp."},

  {NULL, "Search"},
  {"limit-initial-propagation", "lip", "<x>", 'J', "Limits the number of atomic propagations."},
  {"canonic", NULL, NULL, 'c', "Searches a canonical plan, where an action can belong only once to the plan."},
  {"sequential", "seq", NULL, 'P', "Searches a sequential plan."},
  {"no-optimal", "sub", NULL, 'O', "Tunes propagations and heuristics for suboptimal planning. This option can be used with a high value of the initial bound (see -b above)." },
  {"give-suboptimal", "gs", NULL, 'u', "Gives suboptimal solution when dichotomic search is used when limiting the number of backtracks." },
  {"dichotomy", NULL, "<x>", 'h', "Dichotomic search for temporal planning. Possible values are 0 (no dichotomy), 1 (increment set to the minimal duration), 2 (increment set to the mean duration, default), 3 (increment set to the maximal duration minus minimum duration), 4 (increment set to maximum duration)."},
  {"restarts", NULL, "<f,m,i>", 'r', "Enables restarts. Argument are <f>: first maximum number of backtracks, <m>: multiplier, <i>: minimum increment."},
  {"seed", NULL, "<x>", 's', "Randomized search with seed <x>."},
  {"max-backtracks-bound", "maxbb", "<x>", 'x', "Maximum number of backtracks <x> allowed at each bound."},
  {"max-backtracks", "maxb", "<x>", 'X', "Maximum number of backtracks <x> allowed for the whole problem."},

  {NULL, "Branching"},
  {"conflicts-first", "cf", NULL, 'g', "Conflicts first search strategy"},
  {"supports-first", "sf", NULL, 'j', "Supports first search strategy"},
  {"mutex-first", "mf", NULL, 'k', "Mutex first search strategy"},
  {"last-conflicts-nb", "lc", "<x>", 'C', "Number of last conflicts for last conflicts reasoning (default: 1)"},
  {"wdeg", "wd", NULL, 'W', "Enables wdeg heuristic"},

  {NULL, "Verbosity"},
  {"verbosity", NULL, "<x>", 'v', "Verbosity level."},
  {"verbose-preprocessing", "vp", NULL, 'p', "Prints results of preprocessing rules."},
  {"print-actions", "pa", NULL, 'q', "Prints the details on all the actions of the domain."},
  {"rationals", "rat", NULL, 'Q', "Prints times as rationals."},

  {NULL, "Special"},
#ifdef RATP
  {"ratp", NULL, "<f,g>", 'w', "Options for the RATP bus scheduling problem."},
  {"ratp-incr", NULL, "<x>", 'I', "Increment wait_max for the RATP bus scheduling problem."},
  {"ratp-limit", NULL, "<x>", 'N', "Goal type for RATP bus scheduling."},
#endif
  {"dae", NULL, NULL, 'E', "Divide-and-Evolve approach."},
  {"read-actions", NULL, "<f>", 'z', "Limit to actions in file."},
  {"yahsp", "yahsp", "<x>", 'y', "Suboptimal Yahsp search, with <x> threads."}
};


/*---------------------------------------------------------------------------*/
/* Static Functions                                                          */
/*---------------------------------------------------------------------------*/


static void usage(void);


/*****************************************************************************/


void cmd_line(int argc, const char **argv)
{
  global_argc = argc;
  global_argv = argv;
  nb_options = sizeof(long_options) / sizeof(CPTOption);
  struct option options[nb_options * 2 + 1];
  char arg_string[nb_options + 1];
  int c, i, j = 0, k = 0;

  for(i = 0; i < nb_options; i++) {
    if (long_options[i].long_name) {
      options[j].name = long_options[i].long_name;
      options[j].has_arg = (long_options[i].argument != NULL);
      options[j].flag = NULL;
      options[j].val = long_options[i].letter;
      arg_string[k++] = options[j].val;
      if (options[j++].has_arg) arg_string[k++] = ':';
      if (long_options[i].short_name) {
	options[j].name = long_options[i].short_name;
	options[j].has_arg = (long_options[i].argument != NULL);
	options[j].flag = NULL;
	options[j++].val = long_options[i].letter;
      }
    }
  }
  memset(&options[j], 0, sizeof(struct option));
  arg_string[k] = 0;

  opt.anytime = false;
  opt.bound = (char *) "0";
  opt.bad_supporters_pruning = true;
  opt.dae = false;
  opt.dichotomy = 2;
  opt.distance_boosting = true;
  opt.distances = 1;
  opt.computing_distances = false;
  opt.fluent_mutexes = true;
  opt.global_mutex_sets = false;
  opt.initial_heuristic = 0;
  opt.limit_initial_propagation = false;
  opt.landmarks = false;
  opt.last_conflicts = 1;
  opt.limit_backtracks = false;
  opt.limit_backtracks_all = false;
  opt.local_mutex_sets = false;
  opt.max_backtracks = LONG_MAX;
  opt.max_makespan = NULL;
  opt.max_plan_length = 500;
  opt.optimal = true;
  opt.output_file = NULL;
  opt.pb_restrict = true;
  opt.pddl21 = false;
  opt.precision = NULL;
  opt.precision2 = NULL;
  opt.print_actions = false;
  opt.propagate_inactive_causals = false;
  opt.random = false;
  opt.rationals = false;
#ifdef RATP
  opt.ratp = false;
  opt.ratp_incr = 0;
  opt.ratp_interval_incr = 0;
  opt.ratp_interval_incr_optim = 0;
#endif
  opt.read_actions = false;
  opt.relevance = true;
  opt.restarts = false;
  opt.restarts_init = LONG_MAX;
  opt.restarts_max_tries = LONG_MAX;
  opt.sequential = false;
  opt.task_intervals = false;
  opt.timer = 0;
  opt.unique_supports = true;
  opt.verbose_preprocessing = false;
  opt.verbosity = 1;
  opt.wdeg = false;
  opt.yahsp = false;

  while ((c = GETOPT(argc, (char * const *) argv, arg_string, options, NULL)) != EOF) {
    switch ((char) c) {
    case 'a': opt.pddl21 = true; opt.precision = strtok(optarg,","); opt.precision2 = strtok(NULL,","); 
      trace(options, "PDDL2.1 semantics with temporal precision : %s,%s\n", opt.precision, opt.precision2); break;
    case 'b': opt.bound = optarg; trace(options, "initial bound : %s\n", opt.bound); break;
    case 'c': opt.max_plan_length = 0; trace(options, "enable canonicity\n"); break;
    case 'd': opt.distances = atol(optarg); trace(options, "distances : h%ld\n", opt.distances); break;
    case 'e': opt.complete_qualprec = true; trace(options, "complete qualitative precedences\n"); break;
    case 'f': opt.facts_file = (char *) optarg; trace(options, "problem file : %s\n", opt.facts_file); break;
    case 'g': opt.branching_strategy = CONFLICTS_FIRST; trace(options, "conflicts first strategy\n"); break;
    case 'h': opt.dichotomy = atol(optarg); trace(options, "dichotomy : %ld\n", opt.dichotomy); break;
    case 'i': opt.initial_heuristic = atol(optarg); trace(options, "initial heuristic : h%ld\n", opt.initial_heuristic); break;
    case 'j': opt.branching_strategy = SUPPORTS_FIRST; trace(options, "supports first strategy\n"); break;
    case 'k': opt.branching_strategy = MUTEX_FIRST; trace(options, "mutex first strategy\n"); break;
    case 'l': opt.max_plan_length = atol(optarg); trace(options, "max plan length : %ld\n", opt.max_plan_length); break;
    case 'm': opt.max_makespan = optarg; trace(options, "max makespan : %s\n", opt.max_makespan); break;
    case 'n': opt.propagate_inactive_causals = true; trace(options, "propagate inactive causals\n"); break;
    case 'o': opt.ops_file = optarg; trace(options, "domain file : %s\n", opt.ops_file); break;
    case 'p' : opt.verbose_preprocessing = true; trace(options, "enable verbose preprocessing\n"); break;
    case 'q' : opt.print_actions = true; trace(options, "print actions\n"); break;
    case 'r': opt.restarts = true; opt.random = true; opt.restarts_init = atol(strtok(optarg,",")); opt.restarts_factor = atof(strtok(NULL,",")); 
      opt.restarts_min_increment = atol(strtok(NULL,",")); { char *tries = strtok(NULL,","); if (tries) opt.restarts_max_tries = atol(tries); }
      trace(options, "restarts : init : %ld, factor : %.2f, min increment : %ld, max tries : %ld\n", 
	    opt.restarts_init, opt.restarts_factor, opt.restarts_min_increment, opt.restarts_max_tries); 
	break;
    case 's': opt.random = true; opt.seed = atol(optarg); trace(options, "randomized, seed : %ld\n", opt.seed); break;
    case 't': opt.timer = atol(optarg); trace(options, "timer : %ld\n", opt.timer); break;
    case 'u': opt.give_suboptimal = true; trace(options, "give suboptimal solution for dichotomic search and limiting backtracks\n"); break;
    case 'v': opt.verbosity = atol(optarg); trace(options, "verbosity level : %ld\n", opt.verbosity); break;
#ifdef RATP
    case 'w': opt.ratp = true; opt.ratp_input = strtok(optarg,","); opt.ratp_output = strtok(NULL,","), opt.branching_strategy = SUPPORTS_FIRST; 
      opt.task_intervals = false; 
      opt.max_plan_length = 0;
      trace(options, "ratp files : %s, %s\n", opt.ratp_input, opt.ratp_output); break;
#endif
    case 'x': opt.limit_backtracks = true; opt.max_backtracks = atol(optarg); trace(options, "maximum backtracks/bound : %ld\n", opt.max_backtracks); break;
    case 'X': opt.limit_backtracks_all = true; opt.max_backtracks = atol(optarg); trace(options, "maximum backtracks/all : %ld\n", opt.max_backtracks); break;
    case 'y': { opt.yahsp = true; opt.pb_restrict = true; opt.yahsp_threads = atol(strtok(optarg, ",")); 
      char *teams = strtok(NULL, ","); opt.yahsp_teams = teams ? atol(teams) : 1;
      if (teams) { char *strategy = strtok(NULL, ","); opt.yahsp_strategy = strategy ? atol(strategy) : 0; }
      trace(options, "suboptimal yahsp search, threads : %ld, teams : %ld, strategy : %ld\n", 
	    opt.yahsp_threads, opt.yahsp_teams, opt.yahsp_strategy); break; }
    case 'z': opt.read_actions = true; opt.read_actions_input = (char *) optarg; trace(options, "action file : %s\n", opt.read_actions_input); break;
    case 'A': opt.landmarks = true; trace(options, "enable landmarks\n"); break;
    case 'B': opt.bad_supporters_pruning = false; trace(options, "disable bad supporters pruning\n"); break;
    case 'C': opt.last_conflicts = atol(optarg); trace(options, "number of last conflicts : %ld\n", opt.last_conflicts); break;
    case 'D': opt.distance_boosting = false; trace(options, "disable distance increase\n"); break;
    case 'E': opt.dae = true; opt.limit_initial_propagation = true; opt.max_propagations = 10000; opt.pb_restrict = opt.yahsp; 
      trace(options, "divide-and-evolve approach\n"); break;
    case 'F': opt.fluent_mutexes = true; trace(options, "enables h2 based fluent mutexes in yahsp\n"); break;
    case 'G': opt.global_mutex_sets = true; trace(options, "enable global mutex sets\n"); break;
    case'H': opt.output_file = (char *) optarg; trace(options, "output file : %s\n", opt.output_file); break;
#ifdef RATP
    case 'I': opt.ratp_interval_incr = atol(strtok(optarg,",")); 
      opt.ratp_interval_incr_optim = atol(strtok(NULL,",")); 
      opt.ratp_incr = atol(strtok(NULL,","));
      opt.limit_backtracks_all = true; 
      opt.max_backtracks = atol(strtok(NULL,",")); 
      opt.give_suboptimal = true;
      trace(options, "interval increments : %ld (relaxation), %ld (optimization)\nwait max increment : %ld\nmaximum backtracks : %ld\n", 
	    opt.ratp_interval_incr, opt.ratp_interval_incr_optim, opt.ratp_incr, opt.max_backtracks); break;
#endif
    case 'J': opt.limit_initial_propagation = true; opt.max_propagations = atol(optarg); 
      trace(options, "limited propagation : %ld\n", opt.max_propagations); break;
    case 'K': { opt.precision = strdup(strtok(optarg,",")); 
      char *p = strtok(NULL,","); if (p != NULL) opt.precision2 = strdup(p);
      trace(options, "temporal precision : %s,%s\n", opt.precision, opt.precision2); break; }
    case 'L': opt.local_mutex_sets = true; trace(options, "enable local mutex sets\n"); break;
    case 'M': opt.global_mutex_sets = true; opt.local_mutex_sets = true; trace(options, "enable global and local mutex sets\n"); break;
    case 'N': opt.anytime = true; trace(options, "anytime behavior\n"); break;
    case 'O': opt.optimal = false; opt.complete_qualprec = true;
      opt.relevance = false; opt.bound = (char *) "10000"; opt.propagate_inactive_causals = true;
      opt.landmarks = true; opt.branching_strategy = SUPPORTS_FIRST; trace(options, "suboptimal search\n"); break;
    case 'P': opt.sequential = true; trace(options, "sequential plan search\n"); break;
    case 'Q': opt.rationals = true; trace(options, "rationals printing\n"); break;
    case 'R': opt.relevance = false; trace(options, "disable relevance\n"); break;
    case 'S': opt.shaving = true; trace(options, "enable shaving\n"); break;
    case 'T': opt.task_intervals = true; trace(options, "enable task intervals\n"); break;
    case 'U': opt.unique_supports = false; trace(options, "disable unique supports\n"); break;
    case 'W': opt.wdeg = true; trace(options, "enable wdeg\n"); break;
    }
  }
  
  trace(options, "\n");
  if (optind < argc) {
    trace(options, "arguments error : ");
    while (optind < argc)
      trace(options, "%s ", argv[optind++]);
    trace(options, "\n\n");
    exit(ERR_options);
  }
  if (!opt.ops_file) { usage(); error(options, "Ops file missing"); }
  if (!opt.facts_file) {usage(); error(options, "Facts file missing"); }
}

static void usage(void)
{
  CPTOption *o = long_options;
  long i, max = TEXT_WIDTH - 10;
  const char *s; 
  char *end, buf[max + 1];
  
  buf[max] = 0;
  printf("Usage:    cpt -o <domain file> -f <problem file> [Options]\n\n");
  for (i = 0; i < nb_options; i++) {
    if (!o[i].long_name) printf("\n%s:\n", o[i].short_name);
    else {
      printf("  -%c", o[i].letter);
      if (o[i].short_name) printf(", -%s", o[i].short_name);
      printf(", -%s", o[i].long_name);
      if (o[i].argument) printf(" %s", o[i].argument);      
      if ((s = o[i].detail)) {
	long length = 0;
	while(*(s += length) != 0) {
	  strncpy(buf, s, max);
	  length = strlen(buf);
 	  if (*(s + length)) {
	    if ((end = strrchr(buf, ' '))) *end = 0;
	    length = strlen(buf) + 1;
	  }
	  printf("\n          %s", buf);
	}
      }
      printf("\n");
    }
  }
}
