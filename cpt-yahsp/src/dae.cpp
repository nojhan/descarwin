#include "cpt.h"
#include "structs.h"
#include "instantiation.h"
#include "problem.h"
#include "plan.h"
#include "dae.h"
#include "globs.h"
#include "solve.h"
#include "yahsp.h"
#include "trace_planner.h"

TIMER(prepro_time);
TIMER(search_time);

static double prepro_time;

extern PDDLDomain *pddl_domain;

typedef struct AttributeSpace AttributeSpace;

struct AttributeSpace {
  PDDLPredicate *predicate;
  long position;
  VECTOR(PDDLTerm *, constants);
};

static char ** fluent_names;
SVECTOR(PDDLPredicate *, predicates);
SVECTOR(AttributeSpace *, att_spaces);
SVECTOR(Fluent *, state);

static bool last_station_failed;
static int return_code;

static CPTFluentArray all_fluents;

static void create_attribute_spaces()
{
  int i;
  
  cpt_malloc(predicates, pddl_domain->predicates_nb);

  FOR(p, pddl_domain->predicates) {
    p->num = -1;
  } EFOR;

  FOR(f, fluents) {
    if (f->atom->predicate->num == -1) {
      predicates[predicates_nb++] = f->atom->predicate;
      f->atom->predicate->num = att_spaces_nb;
      att_spaces_nb += f->atom->predicate->terms_nb;
    }
  } EFOR;
  
  cpt_malloc(att_spaces, att_spaces_nb);

  for (i = 0; i < att_spaces_nb; i++) {
    cpt_calloc(att_spaces[i], 1);
    cpt_malloc(att_spaces[i]->constants, pddl_domain->constants_domain_nb + pddl_domain->constants_problem_nb);
  }
  
  FOR(f, fluents) {
    PDDLPredicate *p = f->atom->predicate;
    FORi(t, i, f->atom->terms) {
      AttributeSpace *as = att_spaces[p->num + i];
      as->predicate = p;
      as->position = i;
      FOR(c, as->constants) {
	if (c == t) goto suite;
      } EFOR;
      as->constants[as->constants_nb++] = t;
    suite:;
    } EFOR;
  } EFOR;
  
  FOR(as, att_spaces) {
    cpt_realloc(as->constants, as->constants_nb);
  } EFOR;
}


static int cmp_timeval(const void *v1, const void *v2)
{
  TimeVal a1 = *((TimeVal *) v1);
  TimeVal a2 = *((TimeVal *) v2);
  return a1 - a2;
}

static CPTCostArray costs;
CPTFluentArray *fluents_costs; 

static void compute_costs()
{
  long i = 0;
  TimeVal maxim = 0;
  cpt_malloc(costs.elems, fluents_nb * 2);
  FOR(f, fluents) {
    if (strchr(f->atom->predicate->name, '@') != NULL) continue;
    costs.elems[i++] = f->init; 
/*     costs.elems[i++] = f->end; */
    costs.elems[i++] = f->init;
    maximize(maxim, maxi(f->init, f->end));
  } EFOR;
/*   for (i = 1; i < fluents_nb * 2; i+=2) { */
/*     costs.elems[i] = maxim - costs.elems[i]; */
/*     fluents[i / 2]->end = costs.elems[i]; */
/*   } */
  qsort(costs.elems, fluents_nb * 2, sizeof(TimeVal), cmp_timeval);
  costs.nb = 1;
  for (i = 1; i < fluents_nb * 2; i++)
    if (costs.elems[i] != costs.elems[costs.nb - 1])
      costs.elems[costs.nb++] = costs.elems[i];
  cpt_realloc(costs.elems, costs.nb);
  cpt_malloc(fluents_costs, costs.nb);

  //for (i = 0; i < costs.nb; i++) printf("%ld ", costs.elems[i]);
  //printf("\n");

  //FORi(f, i, fluents) { printf("%s --> %d %d\n", fluent_name(f), f->init, f->end); } EFOR;
  for (i = 0; i < costs.nb; i++) {
    cpt_malloc(fluents_costs[i].elems, fluents_nb);
    fluents_costs[i].nb = 0;
    FOR(f, fluents) { 
      if (strchr(f->atom->predicate->name, '@') != NULL) continue;
/*       if (mini(f->init, f->end) <= costs.elems[i] && maxi(f->init, f->end) >= costs.elems[i])  */
      if (f->init == costs.elems[i]) 
/*       if (f->init == costs.elems[i] || f->end == costs.elems[i])  */
	fluents_costs[i].elems[fluents_costs[i].nb++] = f; 
    } EFOR;
    cpt_realloc(fluents_costs[i].elems, fluents_costs[i].nb);
  }
  //size_t j;
  for (i = 0; i < costs.nb; i++) {
    printf("cost : %ld %d -- ", costs.elems[i], fluents_costs[i].nb);
    //for (j = 0; j < fluents_costs[i].nb; j++) printf("%s ", fluent_name(fluents_costs[i].elems[j]));
    printf("\n");
  }
    

  printf("\n");

}

static char **pair_cost;


static void compute_difficult_pairs()
{
  cpt_malloc(pair_cost, fluents_nb);
  FOR(f, fluents) { cpt_malloc(pair_cost[f->id], f->id + 1); } EFOR;
  FORPAIR(f1, f2, fluents) {
    if (f1 == f2) pair_cost[f2->id][f1->id] = 0;
    else if (fmutex(f1, f2)) pair_cost[f2->id][f1->id] = 100;
    else {
      size_t nb = 0, nbm = 0;
      char diff;
      FOR(a1, f1->producers) {
	if (a1 != start_action)
	  FOR(a2, f2->producers) {
	    if (a2 != start_action && a1 != a2) {
	      nb++;
	      if (amutex(a1, a2)) nbm++;
	    }
	  } EFOR;
      } EFOR;
      diff = nb == 0 ? 0 : (100 * nbm) / nb;
      pair_cost[f2->id][f1->id] = diff == 0 ? 1 : diff == 100 ? 99 : diff;
    }
    //printf("%s %s %d\n", fluent_names[f1->id], fluent_names[f2->id], pair_cost[f2->id][f1->id]);
  } EFORPAIR;
}

char cptGetFluentDifficulty(CPTFluent f1, CPTFluent f2) 
{
  if (f1->id < f2->id) return pair_cost[f2->id][f1->id];
  else return pair_cost[f1->id][f2->id];
}

void cptMakeProblem(char *cmdline)
{
  char cmd[strlen(cmdline) + 1];
  const char * params[100];
  int nb = 0;
  
  start_timer(prepro_time);
  strcpy(cmd, cmdline);
  params[nb++] = "cpt";
  params[nb++] = "-dae";
  params[nb++] = strtok(cmd," ");
  while ((params[nb++] = strtok(NULL," ")));
  cpt_main(nb - 1, params);
  cptSetPropagationLimit(10000);
  create_attribute_spaces();
  cpt_malloc(fluent_names, fluents_nb);
  FORi(f, i, fluents) {
    fluent_names[i] = cpt_malloc(fluent_names[i], strlen(fluent_name(f)) + 1);
    strcpy(fluent_names[i], fluent_name(f));
  } EFOR;
  if (!opt.yahsp) {
    cpt_malloc(state, fluents_nb);
    state_nb = 0;
  }
  cpt_malloc(plans, 1000);
  plans_nb = 0;
  prepro_time = get_timer(prepro_time);
  compute_costs();
  all_fluents.elems = fluents;
  all_fluents.nb = fluents_nb;
  compute_difficult_pairs();
}

CPTCostArray cptGetCosts()
{
  return costs;
}

CPTFluentArray cptGetFluentsByCost(long cost)
{
  long i;
  //return all_fluents;
  for (i = 0; i < costs.nb; i++) 
    if (costs.elems[i] == cost)
      return fluents_costs[i];
  return fluents_costs[0];
}

CPTFluentArray cptGetInit()
{
  CPTFluentArray fluents;
  fluents.elems = init_state;
  fluents.nb = init_state_nb;
  return fluents;
}

CPTFluentArray cptGetGoal()
{
  CPTFluentArray fluents;
  fluents.elems = goal_state;
  fluents.nb = goal_state_nb;
  return fluents;
}

CPTTermArray cptGetPossibleTerms(CPTPredicate p, int pos)
{
  CPTTermArray terms;
  terms.elems = att_spaces[p->num + pos]->constants;
  terms.nb = att_spaces[p->num + pos]->constants_nb;
  return terms;
}

CPTPredicateArray cptGetPredicates()
{
  CPTPredicateArray preds;
  preds.elems = predicates;
  preds.nb = predicates_nb;
  return preds;
}

const char * cptGetPredicateName(CPTPredicate p)
{
  return p->name;
}

int cptGetIndexFromFluent(CPTFluent f)
{
  return (int) f->id;
}

CPTFluent cptGetFluentFromIndex(int n)
{
  return fluents[n];
}

CPTTermArray cptGetTerms(CPTFluent f)
{
  CPTTermArray terms;
  terms.elems = f->atom->terms;
  terms.nb = f->atom->terms_nb;
  return terms;
}

CPTPredicate cptGetPredicate(CPTFluent f)
{
  return f->atom->predicate;
}

int cptGetArity(CPTPredicate p)
{
  return p->terms_nb;
}

const char * cptGetTermName(CPTTerm t)
{
  return t->name;
}

char *cptGetFluentName(CPTFluent f)
{
  return fluent_names[f->id];
}

CPTFluent cptGetFluent(CPTPredicate p, CPTTermArray terms)
{
  return (CPTFluent) search_fluent(pddl_domain, p, terms.elems);
}

int cptMutex(CPTFluent f1, CPTFluent f2)
{
  return fmutex(f1, f2);
}

/*******************************************************************************/

void apply_plan(void)
{
  FOR(s, solution_plan->steps) {
    FORi(f, i, state) {
      FOR(d, s->action->del) { if (f == d) { state[i--] = state[--state_nb]; break; } } EFOR;
    } EFOR;
    FOR(a, s->action->add) { 
      FOR(f, state) { if (a == f) goto next; } EFOR;
      state[state_nb++] = a; 
    next:;
    } EFOR;
  } EFOR;
}

void cptResetSearch()
{
  FOR(p, plans) { plan_free(p); } EFOR;
  plans_nb = 0;
  last_station_failed = false;
  if (!opt.yahsp) {
    state_nb = 0;
    FOR(f, init_state) { state[state_nb++] = f; } EFOR;
    FORMIN(a, actions, 2) {
      a->nb_instances = (opt.max_plan_length == 0 ? 0 : opt.max_plan_length - 1);
    } EFOR;
  }
  else {
    yahsp_reset();
  }
}

CPTSolutionPlan cptSolveNext(CPTFluentArray goal)
{
  if (last_station_failed) error(station, "Last station failed : please reset search");
  start_timer(search_time);
  return_code = cpt_search(state, state_nb, goal.elems, goal.nb, false, false, false, opt.max_backtracks);
  if (solution_plan) {
    solution_plan->search_time = get_timer(search_time);
    solution_plan->total_time = solution_plan->search_time;
    plans[plans_nb++] = solution_plan;
    if (!opt.yahsp) apply_plan();
    return plans[plans_nb - 1];
  } else {
    last_station_failed = true;
    return NULL;
  }
}

CPTSolutionPlan cptCompressSolution(bool compress_causals, bool compress_orderings)
{
  if (last_station_failed) error(station, "Last station failed : please reset search");
  start_timer(search_time);
  bool cqp = opt.complete_qualprec;
  opt.complete_qualprec = true;
  return_code = cpt_search(init_state, init_state_nb, goal_state, goal_state_nb, true, compress_causals, compress_orderings, opt.max_backtracks);
  opt.complete_qualprec = cqp;
  if (solution_plan) {
    solution_plan->search_time = get_timer(search_time);
    solution_plan->total_time = solution_plan->search_time;
    FOR(p, plans) {
      solution_plan->total_time += p->total_time;
    } EFOR;
  }
  return solution_plan;
}


void cptPrintSolutionPlan(FILE *out, CPTSolutionPlan solution_plan)
{
  print_plan(out, solution_plan, false);
}

void cptFreeSolutionPlan(CPTSolutionPlan solution_plan)
{
  plan_free(solution_plan);
}

CPTFluent cptGetFluentFromName(char *name)
{
  FORi(f, j, fluents) {
    if (strcmp(name, fluent_names[j]) == 0) return f;
  } EFOR;
  error(station, "Fluent not found : %s", name);
}

CPTFluentArray cptGetFluentArrayFromNames(char **names, int names_nb)
{
  CPTFluentArray fl;
  int i;
  fl.nb = 0;
  cpt_malloc(fl.elems, names_nb);
  for(i = 0; i < names_nb; i++) {
    if (names[i]) fl.elems[fl.nb++] = cptGetFluentFromName(names[i]);
  }
  return fl;
}
      
CPTFluentArray cptGetAllFluents(void)
{
  return all_fluents;
}

long cptGetMakespan(CPTSolutionPlan plan)
{
  return plan->makespan;
}

long cptGetPGCD()
{
  return pddl_domain->time_gcd;
}

long cptGetPPCM()
{
  return pddl_domain->time_lcm;
}

int cptGetLength(CPTSolutionPlan plan)
{
  return plan->steps_nb;
}

long cptGetBacktracks(CPTSolutionPlan plan)
{
  return plan->backtracks;
}

double cptGetSearchTime(CPTSolutionPlan plan)
{
  return plan->search_time;
}

double cptGetTotalTime(CPTSolutionPlan plan)
{
  return plan->total_time;
}

double cptGetPreprocessingTime(CPTSolutionPlan plan)
{
  return prepro_time;
}

void cptSetMaxBacktracks(long backtracks)
{
  opt.max_backtracks = backtracks;
}

int cptGetReturnCode(void)
{
  return return_code;
}

void cptSetPDDL21(char *precision)
{
  opt.pddl21 = true;
  opt.precision = precision;
}

void cptSetPropagationLimit(long max_propagations)
{
  opt.limit_initial_propagation = true; 
  opt.max_propagations = max_propagations;
}
