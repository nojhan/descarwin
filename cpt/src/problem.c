/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : problem.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#include "cpt.h"
#include "trace.h"
#include "structs.h"
#include "instantiation.h"
#include "problem.h"
#include "plan.h"
#include "max_atom.h"
#include "propagations.h"
#include "preprocess.h"
#include "scheduling.h"
#include "yahsp.h"
#include "globs.h"
#ifdef RESOURCES
#include "resources.h"
#endif


/*---------------------------------------------------------------------------*/
/* Local Variables                                                           */
/*---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*/
/* Local Macros                                                              */
/*---------------------------------------------------------------------------*/


/* #define keep_reachable_tab(tab) \ */
/*   NEST( long i = 0; if (tab) FOR(x, tab) { if (x->init != MAXCOST) tab[i++] = x; } EFOR; \ */
/* 	if (i == 0) { if (tab) cpt_free(tab); tab = NULL; } else if (tab##_nb != i) { cpt_realloc(tab, i); tab##_nb = i; } ) */

#define keep_reachable_tab(tab) \
  NEST( long i = 0; if (tab) FOR(x, tab) { if (reachable[x->id]) tab[i++] = x; } EFOR; \
	if (i == 0) { if (tab) cpt_free(tab); } else if (tab##_nb != i) cpt_realloc(tab, i); tab##_nb = i; )


/*---------------------------------------------------------------------------*/
/* Static Functions                                                          */
/*---------------------------------------------------------------------------*/


static void create_structures(void);
static void create_edeletes(void);
static void finalize_structures(void);
static void keep_reachable(void);


/*****************************************************************************/


void print_time(FILE *file, TimeVal x) 
{ print_time_incr(file, x, -1); }

void print_time_incr(FILE *file, TimeVal x, int step)
{
  char buffer[21];
  mpz_t num;
  mpq_t time;

  sprintf(buffer, "%" TIMEP, x);
  mpz_init_set_str(num, buffer, 10);
  mpz_mul(num, num, pddl_domain->time_gcd_rat);
  mpq_init(time);
  mpq_set_num(time, num);
  mpq_set_den(time, pddl_domain->time_lcm_rat);
  mpq_canonicalize(time);
  
  if (opt.rationals) {
    mpq_out_str(file, 10, time);
  } else {
    mpf_t time_float, incr;
    mpf_init(time_float);
    mpf_set_q(time_float, time);
    if (step != -1 && !opt.sequential && opt.precision2) {
      mpf_init_set_str(incr, opt.precision2, 10);
      mpf_mul_ui(incr, incr, step);
      mpf_add(time_float, time_float, incr);
      gmp_fprintf(file, "%.*Ff", strlen(opt.precision2) - 2, time_float);
      mpf_clear(incr);
    } else {
      gmp_fprintf(file, "%.Ff", time_float);
    }
    mpf_clear(time_float);
  }
  mpz_clear(num);
  mpq_clear(time);
}

char *fluent_name(Fluent *f)
{
  return make_name(f->atom->predicate->name, f->atom->terms, f->atom->terms_nb);
}

void print_fluent(Fluent *f)
{
  trace(normal, "FLUENT %ld : %s\n  INIT -> ", f->id, fluent_name(f));
  print_time(cptout, f->init);
  trace(normal, "\n  PRODUCERS -> ");
  FOR(a, f->producers) { trace(normal, " %s", action_name(a)); } EFOR;
  trace(normal, "\n  CONSUMERS -> ");
  FOR(a, f->consumers) { trace(normal, " %s", action_name(a)); } EFOR;
  trace(normal, "\n  DELETERS -> ");
  FOR(a, f->deleters) { trace(normal, " %s", action_name(a)); } EFOR;
  trace(normal, "\n  EDELETERS -> ");
  FOR(a, f->edeleters) { trace(normal, " %s", action_name(a)); } EFOR;
  trace(normal, "\n\n");
}

#ifdef RESOURCES
void print_resources_local(ResourceLocal **resources, long resources_nb, bool print_action)
{
  FOR(rl, resources) { 
    if (print_action) trace(normal, "  ACTION %s ->\n", action_name(rl->action));
    char *name = resource_name(rl->resource);
    if (val_known(rl->min_level)) trace(normal, "    check %s >= %" TIMEP "\n", name, rl->min_level);
    if (val_known(rl->max_level)) trace(normal, "    check %s <= %" TIMEP "\n", name, rl->max_level);
    if (val_known(rl->increased)) trace(normal, "    increase %s by %" TIMEP "\n", name, rl->increased);
    if (val_known(rl->decreased)) trace(normal, "    decrease %s by %" TIMEP "\n", name, rl->decreased);
    if (val_known(rl->assigned)) trace(normal, "    assign %s to %" TIMEP "\n", name, rl->assigned); 
  } EFOR;
}
#endif

void print_complete_action(Action *a)
{
  trace(normal, "ACTION %ld : %s\n", a->id, action_name(a));
  trace(normal, "  INIT -> ");
  print_time(cptout, a->init);
  trace(normal, "\n  DURATION -> ");
  print_time(cptout, duration(a));
  if (opt.pddl21 && a->ope->real_duration) {
    trace(normal, "\n  RDURATION -> ");
    print_time(cptout, a->rdur.t);
  }
  trace(normal, "\n  PREC ->");
  FOR(f, a->prec) { trace(normal, " %s", fluent_name(f)); } EFOR;
  trace(normal, "\n  ADD ->");
  FOR(f, a->add) { trace(normal, " %s", fluent_name(f)); } EFOR;
  trace(normal, "\n  DEL ->");
  FOR(f, a->del) { trace(normal, " %s", fluent_name(f)); } EFOR;
  trace(normal, "\n  EDEL ->");
  FOR(f, a->edel) { trace(normal, " %s", fluent_name(f)); } EFOR;
#ifdef RESOURCES
  if (a->resources) {
    trace(normal, "\n  RESOURCES ->\n");
    print_resources_local(a->resources, a->resources_nb, false);
  }
#endif
  if (a->ac_constraints) {
    trace(normal, "\n  ACTIVITY ->\n");
    FOR(ac, a->ac_constraints) {
      trace(normal, "    %s -- min : ", fluent_name(ac->fluent));
      print_time(cptout, ac->min.t);
      trace(normal, " -- max : ");
      print_time(cptout, ac->max.t);
      if (ac->time) {
	trace(normal, "\n\t\t");
	FOR2(t, ac->time, d, ac->dur) {
	  trace(normal, "(");
	  print_time(cptout, t.t);
	  trace(normal, ",");
	  print_time(cptout, d.t);
	  trace(normal, ") ");
	} EFOR;
      }
    } EFOR;
  }
  trace(normal, "\n\n");
}

char *action_name(Action *a)
{
  return make_name(a->ope->name, a->parameters, a->parameters_nb);
}

void print_action(Action *a)
{
  trace(normal, "%s%s%ld[", (a->used ? "*" : (a->excluded ? "!" : "")), action_name(a), a->id);
  print_time(cptout, first_start(a));
  trace(normal, ",");
  print_time(cptout, last_start(a));
  trace(normal, "](");
  print_time(cptout, duration(a));
  trace(normal, ")");
#ifdef RESOURCES
  if (a->synchro) fprintf(cptout, "[%" TIMEP ",%" TIMEP "]", min_level(a), max_level(a));
#endif
}

void free_ac_constraint(ActivityConstraint *ac) 
{
  mpq_clear(ac->min.q);
  mpq_clear(ac->max.q);
  cpt_free(ac);
}

void free_action(Action *a)
{
  cpt_free(a->parameters);
  mpq_clear(duration_rat(a));
  if (opt.pddl21 && a->ope->real_duration) mpq_clear(a->rdur.q);
  cpt_free(a->precedences);
  cpt_free(a->prec);
  cpt_free(a->add);
  cpt_free(a->del);
  cpt_free(a->edel);
  cpt_free(a->distances); 
  cpt_free(a->consumes);
  cpt_free(a->produces);
  cpt_free(a->deletes);
  cpt_free(a->edeletes);
  cpt_free(a->mutex);
  if (!opt.optimal) cpt_free(a->deletes_included);
#ifdef RESOURCES
  if (a->resources) {
    FOR(rl, a->resources) {
      cpt_free(rl);
    } EFOR;
    cpt_free(a->resources);
  }
#endif
  FOR(ac, a->ac_constraints) {
    free_ac_constraint(ac);
  } EFOR;
  cpt_free(a->ac_constraints);
  cpt_free(a);
}

#ifdef RESOURCES
char *resource_name(Resource *r)
{
  return make_name(r->atom->predicate->name, r->atom->terms, r->atom->terms_nb);
}

void print_resource(Resource *r)
{
  trace(normal, "RESOURCE %s :\n", resource_name(r));
  FOR(a, r->fluent_modified->producers) {
    ResourceLocal *resource =  resource_local(a, r);
    print_resources_local(&resource, 1, true);
  } EFOR;
}
#endif

void print_causal(Causal *c)
{
  trace(normal, "S(%s, [", fluent_name(c->fluent));
  print_time(cptout, first_start(c));
  trace(normal, " ");
  print_time(cptout, last_start(c));
  trace(normal, "],");
  print_action(c->consumer);
  trace(normal, ")");
}

static void create_structures(void) 
{
  static int pass = 0;
  long actions_more = (pass < 2 ? 0 : opt.max_plan_length);

  total_actions_nb = actions_nb + actions_more;

  pass++;

  start_action = actions[0];
  end_action = actions[1];

  FORMAXi(a, i, actions, total_actions_nb) {
    a->id = i;
    bitarray_set_index(a);
    if (i < actions_nb) {
      if (pass > 1) {
	cpt_free(a->consumes);
	cpt_free(a->produces);
	cpt_free(a->deletes);
	cpt_free(a->edeletes);
	cpt_free(a->mutex);
	if (!opt.optimal) cpt_free(a->deletes_included);
      }
      a->consumes = bitarray_create(fluents_nb);
      a->produces = bitarray_create(fluents_nb);
      a->deletes = bitarray_create(fluents_nb);
      a->edeletes = bitarray_create(fluents_nb);
      if (opt.fluent_mutexes) a->mutex = bitarray_create(a->id + 1);
      if (!opt.optimal) a->deletes_included = bitarray_create(actions_nb);
    }
  } EFOR;

  FORi(f, i, fluents) {
    f->id = i;
    bitarray_set_index(f);
    if (f->pair_cost) cpt_free(f->pair_cost);
    if (pass == 2 && opt.fluent_mutexes) f->mutex = bitarray_create(f->id + 1);
    if (pass == 2 && opt.initial_heuristic == 2) cpt_calloc(f->pair_cost, fluents_nb);
    if (pass == 3 && opt.distances == 2) cpt_calloc(f->pair_cost, fluents_nb);
  } EFOR;

  init_state = start_action->add;
  init_state_nb = start_action->add_nb;
  goal_state = end_action->prec;
  goal_state_nb = end_action->prec_nb;

  if (!opt.pb_restrict && pass == 3) {
    start_action->add = fluents;
    start_action->add_nb = fluents_nb;
    end_action->prec = fluents;
    end_action->prec_nb = fluents_nb;
  }
  
  FOR(a, actions) {
    if (pass == 3 && !opt.yahsp) cpt_calloc(a->distances, actions_nb);
    FOR(f, a->prec) { f->consumers_nb++; } EFOR;
    FOR(f, a->add) { f->producers_nb++;  } EFOR;
    FOR(f, a->del) { f->deleters_nb++; } EFOR;
    FOR(f, a->edel) { f->edeleters_nb++; } EFOR;
  } EFOR;

  FOR(f, fluents) {
    cpt_malloc(f->consumers, f->consumers_nb + actions_more);
    cpt_malloc(f->producers, f->producers_nb + actions_more);
    cpt_malloc(f->deleters, f->deleters_nb + actions_more);
    cpt_malloc(f->edeleters, f->edeleters_nb + actions_more);
    f->consumers_nb = 0;
    f->producers_nb = 0;
    f->deleters_nb = 0;
    f->edeleters_nb = 0;
  } EFOR;

  FOR(a, actions) {
    FOR(f, a->prec) { f->consumers[f->consumers_nb++] = a; set_consumes(a, f); } EFOR;
    FOR(f, a->add) { f->producers[f->producers_nb++] = a; set_produces(a, f); } EFOR;
    FOR(f, a->del) { f->deleters[f->deleters_nb++] = a; set_deletes(a, f); } EFOR;
    FOR(f, a->edel) { f->edeleters[f->edeleters_nb++] = a; set_edeletes(a, f); } EFOR;
  } EFOR;

  if (pass > 1 && opt.fluent_mutexes) {
    FOR(f, fluents) {
      FOR(a, f->deleters) {
	FOR(a2, f->consumers) { set_amutex(a, a2); } EFOR;
	FOR(a2, f->producers) { set_amutex(a, a2); } EFOR;
      } EFOR;
      FOR(a, f->edeleters) {
	FOR(a2, f->consumers) { set_amutex(a, a2); } EFOR;
	FOR(a2, f->producers) { set_amutex(a, a2); } EFOR;
      } EFOR;
      if (opt.pddl21) {
	FORPAIR(a1, a2, f->producers) { if (a1 != a2) set_amutex(a1, a2); } EFORPAIR;
	FORPAIR(a1, a2, f->deleters) { if (a1 != a2) set_amutex(a1, a2); } EFORPAIR;
      }
    } EFOR;
  }
}

static void create_edeletes(void) 
{
  Fluent *tmp[fluents_nb];
  long tmp_nb;

  FOR(a, actions) {
    tmp_nb = 0;
    if (a == end_action || (a == start_action && !opt.pb_restrict)) continue;
    FOR(f, fluents) {
      if (f->init != MAXCOST) {
	if (deletes(a, f)) goto suite;
	FOR(f2, a->prec) { if (fmutex(f, f2)) goto suite; } EFOR;
	FOR(f2, a->add) { if (fmutex(f, f2)) goto suite; } EFOR;
	continue;
      suite:
	tmp[tmp_nb++] = f;
      }
    } EFOR;
    vector_copy(a->edel, tmp);
  } EFOR;
}


static void finalize_structures(void)
{
  long i;

  if (!opt.yahsp) {
    FOR(f, fluents) {
      cpt_malloc(f->indac, total_actions_nb);
      if (opt.distances == 2) cpt_free(f->pair_cost);
      for (i = 0; i < total_actions_nb; i++)
	f->indac[i] = VAL_UNKNOWN;
      FORi(a, i, f->producers) { f->indac[a->id] = i; } EFOR;
    } EFOR;
  }

  if (!opt.pb_restrict) {
    FOR(f, fluents) { unset_produces(start_action, f); } EFOR;
    FOR(f, init_state) { set_produces(start_action, f); } EFOR;
  }

  // problématique avec h2 !!!!
  if (opt.fluent_mutexes)  {
    FORPAIR(f1, f2, fluents) {
      unset_fmutex(f1, f2);
      FOR(a1, f1->producers) {
	if (!produces(a1, f1)) continue;
	FOR(a2, f2->producers) {
	  if (!produces(a2, f2)) continue;
	  if (produces(a1, f2) || produces(a2, f1) || (a1 != start_action && !edeletes(a1, f2)) || (a2 != start_action && !edeletes(a2, f1))) goto suite2;
	} EFOR;
      } EFOR;
      set_fmutex(f1, f2);
    suite2:;
    } EFORPAIR;
  }

#ifdef RESOURCES
  FOR(r, resources) {
    cpt_calloc(r->reslocals, (r->reslocals_nb = actions_nb));
  } EFOR;

  FOR(a, actions) {
    FOR(r, a->resources) {
      r->resource->reslocals[a->id] = r;
    } EFOR;
  } EFOR;
#endif
  
  if (!opt.pb_restrict)
    FOR(f, fluents) { set_produces(start_action, f); } EFOR;
}

static void keep_reachable(void)
{
  long i, actions_orig = actions_nb;
  bool cont = true;
  bool reachable[fluents_nb];

  if (opt.pddl21) {
    if (opt.pddl21) FOR(a, actions) { FOR(f, a->add) { if (deletes(a, f)) a->init = MAXCOST; } EFOR; } EFOR;
    bool loop = true;
    while (loop) {
      loop = false;
      FOR(a, actions) { 
	if (a->init != MAXCOST && a->ac_constraints && a->ac_constraints[0]->fluent->consumers) {
	  Action *cons =  a->ac_constraints[0]->fluent->consumers[0];
	  if (cons->init == MAXCOST && a->init != MAXCOST) {
	    a->init = MAXCOST;
	    loop = true;
	  }
	}
      } EFOR;
    }
  }

  FORMIN(a, actions, 2) {
#ifdef RESOURCES
    if (a->synchro) goto useful;
    FOR(f, a->add) { if (!f->resource && !consumes(a, f)) goto useful; } EFOR;
    FOR(f, a->del) { if (!f->resource && !produces(a, f)) goto useful; } EFOR;
    FOR (r, a->resources) { 
      if ((val_known(r->increased) && r->increased > 0) || 
	  (val_known(r->decreased) && r->decreased > 0) || 
	  (val_known(r->assigned) && r->assigned > 0))
	goto useful; 
    } EFOR;
#else
    FOR(f, a->add) { if (!consumes(a, f)) goto useful; } EFOR;
    FOR(f, a->del) { if (!produces(a, f)) goto useful; } EFOR;
#endif
    a->init = MAXCOST;
  useful:;
  } EFOR;

  while (cont) {
    cont = false;

    FOR(f, fluents) { reachable[f->id] = false; } EFOR;
    FOR(a, actions) {
      if (a->init != MAXCOST) {
	FOR(f, a->prec) { if (f->end != MAXCOST) reachable[f->id] = true; } EFOR;
	FOR(f, a->del) { if (f->end != MAXCOST) reachable[f->id] = true; } EFOR;
      }
    } EFOR;
    // probleme avec airport 3 et parcprinter 10
    if (0&&opt.pddl21) {
      FOR(a, actions) {
	if (a->init != MAXCOST && a->ac_constraints && !reachable[a->ac_constraints[0]->fluent->id])
	  { 
	    free_ac_constraint(a->ac_constraints[0]);
	    if (--a->ac_constraints_nb == 0) cpt_free(a->ac_constraints);
	  }
      } EFOR;
    }

    FOR(a, actions) { 
      if (a->init != MAXCOST) {
	keep_reachable_tab(a->add); 
#ifndef RESOURCES
	if (a != start_action && a != end_action && a->add == NULL) {
#else
	if (!opt.pddl21 && a != start_action && a != end_action && a->add == NULL && !a->resources) {
#endif
	  a->init = MAXCOST; 
	  cont = true; 
	}
      }
    } EFOR;
/*       cont = false; */
  }  
  
  i = 0;
  FOR(a, actions) {
    if (a->init != MAXCOST) actions[i++] = a;
    else free_action(a);
  } EFOR;
  actions_nb = i;
  memmove(actions + actions_nb, actions + actions_orig, (opt.max_plan_length + 1) * sizeof(Action *));
  cpt_realloc(actions, actions_nb + opt.max_plan_length + 1);
  
/*   FOR(f, fluents) { trace(normal, "%s %d\n", fluent_name(f), f->end); } EFOR; */
  FOR(f, fluents) {
    cpt_free(f->consumers);
    cpt_free(f->producers);
    cpt_free(f->deleters);
    cpt_free(f->edeleters);
    f->consumers_nb = 0;
    f->producers_nb = 0;
    f->deleters_nb = 0;
    f->edeleters_nb = 0;
  } EFOR;
  
  keep_reachable_tab(fluents);

  FOR(a, actions) { 
    keep_reachable_tab(a->prec);
    keep_reachable_tab(a->del);
    keep_reachable_tab(a->edel);
  } EFOR;
  
  if (actions[0] != start_action || actions[1] != end_action)
    error(no_plan, "Start or End has been removed");
}


#define realdur(a) (duration(a) + (a->pddl21_next ? a->ac_constraints[0]->min.t : 0))

Action *lastac(Action *a)
{
  while (a->pddl21_next) a = a->pddl21_next;
  return a;
 }

static TimeVal realdiff(Action *a1, Action *a2)
{
  TimeVal d = distance(a1, a2);
  if (d == 0) return 0;
  //if (a1->pddl21_prev &&  distance(a1->pddl21_prev, a2) == 0) return 0;
  //if (a2->pddl21_next &&  distance(a1, a2->pddl21_next) == 0) return 0;
  if (distance(a1, lastac(a2)) == 0) return 0;
  while ((a1 = a1->pddl21_prev)) d += realdur(a1->pddl21_next);
  while ((a2 = a2->pddl21_prev)) d -= realdur(a2);
  return d;
}

static void increase_pddl21_distances()
{
/*   goto a; */
  Action *x, *y;
  TimeVal md, d;
  FOR(a1, actions) {
    if (!a1->pddl21_prev) {
      FOR(a2, actions) {
	if (a1 != a2 && !a2->pddl21_prev) {
	  x = a1;
	  md = 0;
	  do {
	    y = a2;
	    do {
	      maximize(md, realdiff(x, y));
	    } while ((y = y->pddl21_next));
	  }  while ((x = x->pddl21_next));
	  if (md == 0) continue;
	  x = a1;
	  while(x) {
	    y = a2;
	    d = md;
	    do {
	      maximize(distance(x, y), d);
	      d +=  realdur(y);
	    } while ((y = y->pddl21_next));
	    if ((x = x->pddl21_next))
	      md -= x->pddl21_prev->ac_constraints[0]->min.t + duration(x);
	  }
	}
      } EFOR;
    }
  } EFOR;
/*  a: */
/*   FOR(a1, actions) { */
/*     FOR(a2, actions) { */
/*       printf("%s ", action_name(a1)); */
/*       printf("%s ", action_name(a2)); */
/*       printf("%ld\n", distance(a1, a2)); */
/*     } EFOR; */
/*   } EFOR; */

}

void create_problem(void)
{
  long causals_more = 0, max_prec = 0, max_prods = 0, nbc = 0, i;
  
  pddl_domain = parse_domain(opt.ops_file, opt.facts_file);

  begin_monitor("Instantiating operators");
  instantiate_operators(pddl_domain);

  actions = pddl_domain->actions;
  actions_nb = pddl_domain->actions_nb;
  fluents = pddl_domain->fluents;
  fluents_nb = pddl_domain->fluents_nb;
#ifdef RESOURCES
  resources = pddl_domain->resources;
  resources_nb = pddl_domain->resources_nb;
#endif
  end_monitor();

  begin_monitor("Creating initial structures");
  create_structures();
  compute_reachable();
  if (!opt.pddl21) compute_init_rh1_cost();
  keep_reachable();
  create_structures();
  end_monitor();

  begin_monitor("Computing bound");
  switch (opt.initial_heuristic) {
  case 0: compute_init_h0_cost(); break;
  case 1: compute_init_h1_cost(); break;
  case 2: compute_init_h2_cost(); break;
  }
  end_monitor();
  
/*   cpt_malloc(start_action->del, fluents_nb); */
/*   start_action->del_nb = 0; */
/*   FOR(f, fluents) { if (!produces(start_action, f)) start_action->del[start_action->del_nb++] = f; } EFOR; */
  
  if (opt.fluent_mutexes && opt.initial_heuristic < 2) {
    begin_monitor("Computing e-deleters");
    compute_init_edeletes(); 
    end_monitor();
  }
  
  if (opt.fluent_mutexes || opt.initial_heuristic == 2) {
    begin_monitor("Finalizing e-deleters");
    create_edeletes();
    end_monitor();
  }

  begin_monitor("Refreshing structures");
  keep_reachable();
  create_structures();
  end_monitor();

/*   if (!opt.fluent_mutexes && opt.initial_heuristic < 2) { */
/*     FORPAIR(f1, f2, fluents) { */
/*       unset_fmutex(f1, f2); */
/*       if (f1 != f2) { */
/* 	FOR(a, f1->producers) { if (produces(a, f2) || !deletes(a, f2)) goto suite; } EFOR; */
/* 	FOR(a, f2->producers) { if (produces(a, f1) || !deletes(a, f1)) goto suite; } EFOR; */
/* 	set_fmutex(f1, f2); */
/*       } */
/*     suite:; */
/*     } EFORPAIR; */
/*   } */
  if (!opt.yahsp) {
    begin_monitor("Computing distances");
    opt.computing_distances = true;
    if (!opt.pb_restrict) end_action->prec_nb = 0;
    switch (opt.distances) {
    case 1: compute_h1_distances(); break;
    case 2: compute_h2_distances(); break;
    }
    if (!opt.pb_restrict) end_action->prec_nb = fluents_nb;
    end_monitor();
  }

  //if (opt.yahsp) return;

  begin_monitor("Finalizing structures");
  finalize_structures();
  if (opt.pddl21) {
    FOR(a, actions) {
      if (a->ac_constraints && a->ac_constraints[0]->mandatory) {
	FOR(a2, actions) {
	  FOR(p, a2->prec) { if (p == a->ac_constraints[0]->fluent) {
	      a->pddl21_next = a2;
	      a2->pddl21_prev = a;
	      goto found; 
	    }
	  } EFOR;
	} EFOR;
      found:;
      }
    } EFOR;
    increase_pddl21_distances();
  }
  end_monitor();

  if (opt.print_actions) {
    print_pddl_types(cptout, pddl_domain);
    FOR(a, actions) { print_complete_action(a); } EFOR;
    FOR(f, fluents) { print_fluent(f); } EFOR;
#ifdef RESOURCES
    FOR(r, resources) { print_resource(r); } EFOR;
#endif
  }

  if (opt.yahsp) return;

  begin_monitor("Variables creation");
  causals_nb = end_action->prec_nb;
  FORMIN(a, actions, 2) {
    causals_nb += a->prec_nb;
    maximize(max_prec, a->prec_nb);
  } EFOR;

  causals_more = max_prec * opt.max_plan_length;
  
  FOR(f, fluents) { maximize(max_prods, f->producers_nb); } EFOR;

#ifdef RESOURCES
  init_event_queues(causals_nb + causals_more, total_actions_nb + resources_nb + opt.max_plan_length + causals_nb + causals_more);
#else
  init_event_queues(causals_nb + causals_more, total_actions_nb + opt.max_plan_length + causals_nb + causals_more);
#endif

  FORMAX(a, actions, total_actions_nb) {
    create_bound_variable(&a->start, a->init, MAXTIME, a, (PropagationProc) propagate_action, (EmptyProc) exclude_action);
  } EFOR;
  
/*   FORMAX(a, actions, total_actions_nb) { */
/*     if (a->synchro || a->origin != a)  */
/*       create_bound_variable(&a->reslevel, a->init, 10000000, a, (PropagationProc) propagate_resource, (EmptyProc) exclude_resource); */
/*   } EFOR; */
  
  cpt_malloc(causals, causals_nb + causals_more);
  for (i = 0; i < causals_nb + causals_more; i++)
    cpt_calloc(causals[i], 1);
  
  
  FORMAXi(a, i, actions, total_actions_nb) {
    a->causals = causals + nbc;
    a->causals_nb = a->prec_nb;
    a->nb_instances = (i < 2 || opt.max_plan_length == 0 ? 0 : opt.max_plan_length - 1);
    nbc += (i < actions_nb ? a->prec_nb : max_prec);
    a->precedences = bitarray_create(total_actions_nb);
    FORMAXi(c, j, a->causals, (i < actions_nb ? a->prec_nb : max_prec)) {
      c->consumer = a;
      c->origin = c;
      if (i < actions_nb) { c->fluent = a->prec[j]; }
      create_bound_variable(&c->start, 0, MAXTIME, c, (PropagationProc) propagate_causal, (EmptyProc) exclude_causal);
      create_enum_variable(&c->support, (i < actions_nb ? a->prec[j]->producers_nb : max_prods), opt.max_plan_length, c, (PropagationProc) propagate_causal, (EmptyProc) exclude_causal);
    } EFOR;    
  } EFOR;

  FOR(f, fluents) {
    cpt_malloc(f->causals, f->consumers_nb + causals_more);
    cpt_malloc(f->active_causals, f->consumers_nb + causals_more);
  } EFOR;

  FOR(c, causals) {
    c->fluent->causals[c->fluent->causals_nb++] = c;
  } EFOR;
  
#ifdef RESOURCES
  FOR(a, actions) {
    FOR(r, a->resources) {
      FORi(c, i, a->causals) {
	if (c->fluent == r->resource->fluent_available) {
	  r->index_causal = i;
	  break;
	}
      } EFOR;
    } EFOR;
  } EFOR;
#endif

  cpt_malloc(active_causals, causals_nb  + causals_more);
  cpt_malloc(active_actions, total_actions_nb);

  end_monitor();

  init_mutex_sets(actions_nb + opt.max_plan_length);

  if (opt.bad_supporters_pruning) {
    begin_monitor("Bad supporters");
    if (!opt.pb_restrict) end_action->prec_nb = 0;
    compute_bad_supporters();
    if (!opt.pb_restrict) end_action->prec_nb = fluents_nb;
    end_monitor();
  }

  if (opt.distance_boosting) {
    begin_monitor("Distance boosting");
    compute_distance_boosting();
    end_monitor();
  }
}


