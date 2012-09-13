/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : plan.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#include "cpt.h"
#include "options.h"
#include "structs.h"
#include "problem.h"
#include "propagations.h"
#include "plan.h"
#include "trace_planner.h"
#ifdef RESOURCES
#include "resources.h"
#endif
#include "globs.h"

/*---------------------------------------------------------------------------*/
/* Static Functions                                                          */
/*---------------------------------------------------------------------------*/


/*****************************************************************************/


void print_plan(FILE *out, SolutionPlan *plan, bool print_synchro)
{
  FORi(step, i, plan->steps) {
    if (opt.pddl21 && !step->action->ope->real_duration) continue;
#ifdef RESOURCES
    if (!print_synchro && step->action->synchro) continue;
#endif
    if (pddl_domain->action_costs && !pddl_domain->durative_actions) fprintf(out, "%zu", i);
    else if (opt.sequential) print_time(out, step->init);
    else print_time_incr(out, step->init, i);
    fprintf(out, ": %s", action_name(step->action));
    if (1||!pddl_domain->action_costs) {
      fprintf(out, " [");
      print_time(out, opt.pddl21 ? step->action->rdur.t : duration(step->action));
      fprintf(out, "]");
    }
    if (pddl_domain->durative_actions && pddl_domain->action_costs)
      fprintf(out, " [%" TIMEP "]", step->action->cost);
#ifdef RESOURCES
    if (step->action->synchro) fprintf(out, " [%" TIMEP ",%" TIMEP "]", step->min_level, step->max_level);
#endif
    fprintf(out, "\n");
#ifdef RATP
//#if 1
    FOR(ac, step->action->ac_constraints) {
      TimeVal acmin, acmax;
      evaluate_ac_forward(ac, step->init+step->action->dur.t, step->init+step->action->dur.t, &acmin, &acmax);
      //fprintf(out, "\t%s %" TIMEP " %" TIMEP "\n", fluent_name(ac->fluent), acmin, acmax);
      fprintf(out, "\t%s produced at ", fluent_name(ac->fluent));
      print_time(out, step->init+step->action->dur.t); fprintf(out, " available in [");
      print_time(out, acmin); fprintf(out, ","); print_time(out, acmax); fprintf(out, "]\n");
    } EFOR;
#endif
  } EFOR;
}

void print_plan_ipc(FILE *out, SolutionPlan *plan, double total_time)
{
  fprintf(out, "; Time %.3f\n", total_time);
  fprintf(out, "; Length %lu\n", plan->length);
  if (pddl_domain->action_costs) {
    fprintf(out, "; TotalCost ");
#ifdef DAE
    if (pddl_domain->action_costs && pddl_domain->durative_actions) {
      fprintf(out, "%" TIMEP, plan->cost_add);
      fprintf(out, "\n; MaxCost ");
      fprintf(out, "%" TIMEP, plan->cost_max);
    }
    else
      print_time(out, plan->makespan);
#else
    print_time(out, plan->makespan);
#endif
  } else {
  fprintf(out, "; Makespan ");
  print_time(out, plan->makespan);
  }
  fprintf(out, "\n");
  print_plan(out, plan, false);
  fprintf(out, "; END OF PLAN");
}

void print_plan_ipc_anytime(SolutionPlan *plan)
{
  char filename[strlen(opt.output_file) + 10];
  static int number = 1;
  sprintf(filename, "%s.%d", opt.output_file, number++);
  FILE *file = fopen(filename, "w");
  print_plan_ipc(file, plan, get_timer(stats.total));
  fclose(file);
}

static Comparison precedes_in_plan(Step *a, Step *b)
{
  PREFER(precedes(a->action, b->action), !precedes(b->action, a->action));
  LESS(a->init, b->init);
  // actions MUST BE LEFT UNORDERED : avoid 2 lines after
  //LESS(duration(a->action), duration(b->action));
  //LESS(a->action->id, b->action->id);
  return Equal;
}

SolutionPlan *plan_save(Action **actions, size_t actions_nb, double search_time)
{
  SolutionPlan *plan = cpt_calloc(plan, 1);
  plan->search_time = search_time;
  cpt_malloc(plan->steps, actions_nb - 2);

  FOR(a, actions) {
    if (a->id > 1) {
      Step *step = cpt_malloc(plan->steps[plan->steps_nb++], 1);
      if (!opt.pddl21 || a->ope->real_duration) plan->length++;
      step->action = a;
#ifdef RESOURCES
      if (a->synchro) {
	step->min_level = min_level(a);
	step->max_level = max_level(a);
      }
#endif
      step->init = first_start(a);
      step->end = last_start(a);
      if (pddl_domain->action_costs) {
	plan->cost_add += duration(a);
	maximize(plan->cost_max, duration(a));
      }
    }
  } EFOR;
  if (!pddl_domain->durative_actions && pddl_domain->action_costs) plan->makespan = plan->cost_add;
  else plan->makespan = first_start(end_action) - (opt.pddl21 ? pddl_domain->precision.t : 0);
  vector_sort(plan->steps, precedes_in_plan);
 
  FOR(s, plan->steps) { 
    s->causals_nb = s->action->causals_nb;
    cpt_calloc(s->causals, s->causals_nb);
    FORi(c, i, s->action->causals) {
      s->causals[i] = -1;
      FORi(s2, j, plan->steps) {
	if (get_producer(c) == s2->action) {
	  s->causals[i] = j;
	  break;
	}
      } EFOR;
    } EFOR;
  } EFOR;

  FOR(s, plan->steps) {
    s->before_nb = plan->steps_nb;
    cpt_calloc(s->before, s->before_nb);
    FORi(s2, i, plan->steps) { 
      if (precedes(s2->action, s->action)) {
	//print_action(s2->action); printf(" < "); print_action(s->action); printf("\n");
	s->before[i] = true; 
      }
    } EFOR;
  } EFOR;

  FOR(s, plan->steps) { s->action = s->action->origin; } EFOR;
  return plan;
}

void plan_free(SolutionPlan *plan)
{
  FOR(s, plan->steps) {   
    cpt_free(s->causals);
    cpt_free(s->before);
    cpt_free(s); 
  } EFOR;
  cpt_free(plan->steps);
  cpt_free(plan);
}

void compress_plans(bool causals, bool orderings)
{
  Action *actions_prec[actions_nb+opt.max_plan_length];
  int actions_prec_nb = 0;

  FORMIN(a, actions, 2) { a->nb_instances = -1; } EFOR;
  FOR(p, plans) {
    Action *actions_plan[p->steps_nb];
    int actions_plan_nb = p->steps_nb;
    FORi(s, i, p->steps) {
      clone_action(s->action, NULL);
      //s->action->nb_instances++;
      actions_plan[i] = actions[actions_nb - 1];
      //use_action(actions_plan[i]);
    } EFOR;
    if (causals) {
      FORi(s, i, p->steps) {
	FORi(c, j, s->causals) {
	  if (c != -1) {
	    Causal * causal = actions_plan[i]->causals[j];
/* 	    print_causal(actions_plan[i]->causals[j]); */
/* 	    printf("    "); */
/* 	    print_action(actions_plan[c]); */
/* 	    printf("\n"); */
	    if (!can_produce(causal, actions_plan[c])) exit(55);
	    set_producer(causal, actions_plan[c]);
	  }
	} EFOR;
      } EFOR;
    }
    
    if (orderings) {
      FORi(s, i, p->steps) {
	FORi(prec, j, s->before) {
	  if (prec && actions_plan[j] != actions_plan[i]) {
	    //print_action(actions_plan[j]); printf(" < "); print_action(actions_plan[i]); printf("\n");
	    order_before_aa(actions_plan[j], actions_plan[i]);
	  }
	} EFOR;
      } EFOR;
      FOR(a, actions_prec) {
	FOR(b, actions_plan) {
	    if (amutex(a,b)) order_before_aa(a, b);
	} EFOR;
      } EFOR;
    }

/*     actions_prec_nb = p->steps_nb; */
/*     memcpy(actions_prec, actions_plan, actions_prec_nb * sizeof(Action *)); */

    memcpy(actions_prec+actions_prec_nb, actions_plan, p->steps_nb * sizeof(Action *));
    actions_prec_nb += p->steps_nb;

  } EFOR;
  FORMIN(a, actions, 2) { if (a->nb_instances < 0) exclude_action(a); } EFOR;
  //FORMIN(a, actions, 2) { if (!a->used) exclude_action(a); } EFOR;
}

SolutionPlan *read_plan_from_file(char *filename)
{
  FILE *file = fopen(filename, "r");
  if (file == NULL) error(no_file, "Plan file not found");
  char buffer[STRING_MAX];
  SolutionPlan *plan = cpt_calloc(plan, 1);
  cpt_malloc(plan->steps, 100000);
  while (fscanf(file, "%[^\n]\n", buffer) != EOF) {
    char *s = buffer;
    char *s2 = s;
    bool action = false;
    TimeVal init = 0;
    while (*s2 && *s2 != ':' && *s2 != ';') s2++;
    if (*s2 == ':') {
      *s2++ = '\0';
      init = atoi(s);
    }
    while (*s2 && *s2 != ';')
      if (*s2 == '(') { action = true; s = s2++; }
      else if (*s2 == ')') { *++s2 = '\0'; break; }
      else { *s2 = tolower(*s2); s2++; }
    if (action) {
      FOR(a, actions) {
	if (strcmp(s, action_name(a)) == 0) {
	  cpt_malloc(plan->steps[plan->steps_nb], 1);
	  plan->steps[plan->steps_nb]->action = a;
	  plan->steps[plan->steps_nb++]->init = init;
	  maximize(plan->makespan, init + duration(a)); 
	}
      } EFOR;
    }
  }
  fclose(file);
  cpt_realloc(plan->steps, plan->steps_nb);
  plan->makespan = plan->steps_nb;
  return plan;
}
