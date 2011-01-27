/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : plan.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#include "cpt.h"
#include "structs.h"
#include "problem.h"
#include "globs.h"
#include "propagations.h"
#include "plan.h"
#include "comparison.h"
#include "trace_planner.h"
#ifdef RESOURCES
#include "resources.h"
#endif

/*---------------------------------------------------------------------------*/
/* Static Functions                                                          */
/*---------------------------------------------------------------------------*/


//static int precedes_in_plan(const void *s1, const void *s2);


/*****************************************************************************/


void print_plan(FILE *out, SolutionPlan *plan, bool print_synchro)
{
  FORi(step, i, plan->steps) {
    if (opt.pddl21 && !step->action->ope->real_duration) continue;
#ifdef RESOURCES
    if (!print_synchro && step->action->synchro) continue;
#endif
    if (pddl_domain->action_costs) fprintf(out, "%ld", i);
    else if (opt.sequential) print_time(out, step->init);
    else print_time_incr(out, step->init, i);
    fprintf(out, ": %s", action_name(step->action));
    if (1||!pddl_domain->action_costs) {
      fprintf(out, " [");
      print_time(out, opt.pddl21 ? step->action->rdur.t : duration(step->action));
      fprintf(out, "]");
    }
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
  fprintf(out, "; Time %.2f\n", total_time);
  fprintf(out, "; Length %ld\n", plan->steps_nb);
  if (pddl_domain->action_costs) {
    fprintf(out, "\n; TotalCost ");
    print_time(out, plan->makespan);
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
  if (opt.output_file != NULL) {
    char filename[strlen(opt.output_file) + 10];
    static int number = 1;
    sprintf(filename, "%s.%d", opt.output_file, number++);
    FILE *file = fopen(filename, "w");
    print_plan_ipc(file, solution_plan, get_timer(stats.total));
    fclose(file);
  }
}

int precedes_in_plan(const void *s1, const void *s2)
{
  Step *a = *((Step **) s1);
  Step *b = *((Step **) s2);
  LESS(a->init, b->init);
  LESS(duration(a->action), duration(b->action));
  LESS(a->action->id, b->action->id);
  return Equal;
}

SolutionPlan *plan_save(Action **actions, long actions_nb, double search_time)
{
  SolutionPlan *plan = cpt_malloc(plan, 1);
  plan->steps_nb = 0;
  plan->search_time = search_time;
  plan->makespan = 0;
  cpt_malloc(plan->steps, actions_nb - 2);
  FOR(a, actions) {
    if (a->id > 1) {
      Step *step = cpt_malloc(plan->steps[plan->steps_nb++], 1);
      step->action = a;
#ifdef RESOURCES
      if (a->synchro) {
	step->min_level = min_level(a);
	step->max_level = max_level(a);
      }
#endif
      step->init = first_start(a);
      step->end = last_start(a);
      maximize(plan->makespan,  first_start(a) + (opt.pddl21 ? a->rdur.t : duration(a)));
    }
  } EFOR;
  if (opt.sequential && pddl_domain->action_costs) plan->makespan = total_plan_cost;
  //plan->makespan = first_start(end_action) - (opt.pddl21 ? pddl_domain->precision.t : 0);
  qsort(plan->steps, plan->steps_nb, sizeof(Step *), precedes_in_plan);

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


SolutionPlan plan_copy_static(SolutionPlan plan)
{
  SolutionPlan copy = plan;
  cpt_malloc(copy.steps, copy.steps_nb);
  FORi(s, i, plan.steps) {
    cpt_malloc(copy.steps[i], 1);
    copy.steps[i] = s;
  } EFOR;
  return copy;
}

void plan_free_static(SolutionPlan *plan)
{
  FOR(s, plan->steps) {
    cpt_free(s->causals);
    cpt_free(s->before);
    cpt_free(s);
  } EFOR;
  cpt_free(plan->steps);
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

