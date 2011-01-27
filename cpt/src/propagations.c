/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : propagations.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#include "cpt.h"
#include "structs.h"
#include "problem.h"
#include "propagations.h"
#include "preprocess.h"
#include "scheduling.h"
#include "plan.h"
#include "trace_planner.h"
#include "globs.h"
#ifdef RESOURCES
#include "resources.h"
#endif


/*---------------------------------------------------------------------------*/
/* Static Functions                                                          */
/*---------------------------------------------------------------------------*/


static void synchronize_causal(Causal *c);
static void protect_causal(Causal *c);
static void protect_against(Action *a);
static void make_order_ac(Action *a, Causal *c);
static void protect(Action *a1, Action *a2);


/*****************************************************************************/


void propagate_action(Action *a)
{
  if (a->excluded) return;
  //if (a->excluded || (opt.limit_initial_propagation && first_start(a) > first_start(end_action))) return;
  //if (a == end_action) trace(normal, "%ld ", first_start(end_action));
  trace_proc(propagate_action, a);
  //if (opt.bound > 0 && last_start(end_action) < MAXTIME && first_end(a) > opt.bound) { exclude_action(a); return; }
  protect_against(a);
  if (a->excluded) return;
  FOR(c, a->causals) { activate_causal(c); } EFOR;
  FORCOUPLE(f, a->add, c, f->causals) { if (can_produce(c, a)) activate_causal(c); } EFORCOUPLE;
  //if (a->synchro) propagate_resource(a); 
}

void propagate_causal(Causal *c)
{
  //if (c->excluded || (opt.limit_initial_propagation && first_start(c->consumer) > first_start(end_action))) return;
  if (c->excluded) return;
  trace_proc(propagate_causal, c);
  synchronize_causal(c);
  if (c->excluded) return;
  protect_causal(c);
/*   if (!c->excluded && c->consumer->synchro) propagate_resource(c->consumer);  */
/*   if (c->consumer == end_action && c->fluent->resource && resource_local(c->consumer, c->fluent->resource)) protect_causal_resource(c); */
}

static void synchronize_causal(Causal *c)
{
  Action *prod = get_producer(c);
  
  trace_proc(synchronize_causal, c);
  if (prod) {
    trace_proc(producer_used, c); 
    if (!prod->used && prod->origin->nb_instances > 0) clone_action(prod, c);
    ActivityConstraint *ac = find_ac_constraint(prod, c->fluent);
    if (ac != NULL) {
      TimeVal acmin, acmax;
      evaluate_ac_forward(ac, first_end(prod), last_end(prod), &acmin, &acmax);
      update_inf_a(c->consumer, acmin);
      update_sup_a(c->consumer, acmax);
      evaluate_ac_backward(ac, first_start(c->consumer), last_start(c->consumer), &acmin, &acmax);
      update_inf_a(prod, acmin - duration(prod));
      update_sup_a(prod, acmax - duration(prod));
    }
    update_inf_a(prod, first_start(c));
    update_inf_c(c, first_start(prod));
    update_inf_a(c->consumer, first_start(prod) + delta_aa(prod, c->consumer));
    update_sup_a(prod, mini(last_start(c), last_start(c->consumer) - delta_aa(prod, c->consumer)));
    update_sup_c(c, last_start(prod));
    use_action(prod);
    if (c->consumer->used) make_precede(prod, c->consumer);
  } else {
    TimeVal min_cons_init = MAXTIME, min_prod_init = MAXTIME, max_prod_init = 0, max_cons_init = 0;
    int nb = 0; Action *prod = NULL;

    FORPROD(a, c) {
      if (last_start(a) < first_start(c) || first_start(a) > last_start(c) || cannot_precede_aa(a, c->consumer) ||
	  first_start(c) > last_start(c->consumer) - delta_aa(a, c->consumer))
	rem_producer(c, a);
      else {
	ActivityConstraint *ac = find_ac_constraint(a, c->fluent);
	if (ac != NULL) {
	nb++;
	prod = a;
	  TimeVal acmin, acmax;
	  evaluate_ac_forward(ac, maxi(first_start(a), first_start(c)) + duration(a), 
				 mini(last_start(a), last_start(c)) + duration(a),&acmin, &acmax);
	  minimize(min_cons_init, maxi(acmin, maxi(first_start(a), first_start(c)) + delta_aa(a, c->consumer))); 
	  maximize(max_cons_init, acmax);
	  
	  evaluate_ac_backward(ac, first_start(c->consumer), last_start(c->consumer), &acmin, &acmax);
	  minimize(min_prod_init, maxi(acmin - duration(a), first_start(a)));
	  maximize(max_prod_init, mini(acmax - duration(a), mini(last_start(a), last_start(c->consumer) - delta_aa(a, c->consumer))));
	} else {
	  max_cons_init = MAXTIME;
	  minimize(min_cons_init, maxi(first_start(a), first_start(c)) + delta_aa(a, c->consumer));
	  minimize(min_prod_init, first_start(a));
	  maximize(max_prod_init, mini(last_start(a), last_start(c->consumer) - delta_aa(a, c->consumer)));
	}
      }
    } EFOR;
    update_inf_a(c->consumer, min_cons_init);
    update_inf_c(c, min_prod_init);
    update_sup_c(c, max_prod_init);
    update_sup_a(c->consumer, max_cons_init);
    if (nb == 1) {
      update_inf_a(prod, first_start(c));
      update_sup_a(prod, last_start(c));
    }
  }
}


static void protect_causal(Causal *c)
{
  trace_proc(protect_causal, c);
  if (c->required || opt.propagate_inactive_causals) {
    FOR(a, c->fluent->edeleters) { if (!a->excluded) make_order_ac(a, c); if (c->excluded) return; } EFOR;
  }
  if (opt.unique_supports && c->required && actual_event == InstantiateEvent && edeletes(c->consumer, c->fluent)) {
    Action *prod = get_producer(c);
    if (prod && prod->used) {
      FOR(c2, c->fluent->causals) {
	if (c != c2 && edeletes(c2->consumer, c->fluent)) {
	  rem_producer(c2, prod);
	}
      } EFOR;
    }
  }
  if (!c->excluded && opt.local_mutex_sets) local_mutex_sets(c);
}

static void protect_against(Action *a)
{
  trace_proc(protect_against, a);

  if (a->used) { 
    FOR(a2, active_actions) { // pour non optimal : toutes les actions ???
      if (a != a2) {
	if (precedes(a, a2)) order_before_aa(a, a2);
	else if (precedes(a2, a)) order_before_aa(a2, a);
	else if (amutex(a, a2)) {
	  if (cannot_precede_time_aa(a2, a)) order_before_aa(a, a2);
	  else if (cannot_precede_time_aa(a, a2)) order_before_aa(a2, a);
	}
      }
    } EFOR;
    
  if (opt.propagate_inactive_causals) 
    FORCOUPLE(f, a->edel, c, f->causals) { if (!c->excluded) make_order_ac(a, c); if (a->excluded) return; } EFORCOUPLE;
  else 
    FORCOUPLE(f, a->edel, c, f->active_causals) { make_order_ac(a, c); if (a->excluded) return; } EFORCOUPLE;
  }
}

void use_action(Action *a)
{
  if (!a->used) {
    if (a->excluded) contradiction();
    store(a->used, true);
    trace_proc(action_used, a);
    FOR(c, a->causals) { 
      rem_producer(c, a);
      store(c->required, true);
      vstore(active_causals, c);
      vstore(c->fluent->active_causals, c);
      activate_causal(c);
    } EFOR;
    vstore(active_actions, a);
    if (opt.complete_qualprec) {
      FOR(a1, actions) {
	if (precedes(a1, a)) protect(a1, a);
	else if (precedes(a, a1)) protect(a, a1);
      } EFOR;
    }
    if (opt.sequential) { 
      store(total_plan_cost, total_plan_cost + duration(a));
      update_inf_a(end_action, total_plan_cost);
      FOR(a, actions) {
	if (!a->used && total_plan_cost + duration(a) > last_start(end_action))
	  exclude_action(a);
      } EFOR;
    }
    //protect_against(a); // ici ou en dessous ?
    activate_action(a);
  }
  //protect_against(a);
}


#define increment_weight(c) NEST( c->origin->weight++; maximize(c->consumer->origin->weight, c->origin->weight); )

static void exclude_action2(Action *a, Causal *c)
{
  if (a->excluded) return;
  if (a == end_action && !a->used) return;
  trace_proc(action_excluded, a);
  if (a->used) { 
    if (opt.wdeg) {
      if (c != NULL) {
	increment_weight(c);
	FOR(b, active_actions) { 
	  if (a != b && edeletes(b, c->fluent)) 
	    b->origin->weight++;
	} EFOR;
      } else {
	FORCOUPLE(f, a->add, c, f->active_causals) { 
	  if (get_producer(c) == a) increment_weight(c); 
	} EFORCOUPLE;
	FOR(b, active_actions) { 
	  if (b != a && amutex(a,b) && (precedes(a, b) || precedes(b, a))) 
	    b->origin->weight++; 
	} EFOR;
	a->origin->weight++;
      }
    }
    contradiction(); 
  }
  store(a->excluded, true);
  FORCOUPLE(f, a->add, c, f->causals) { rem_producer(c, a); } EFORCOUPLE;
  FOR(c, a->causals) { store(c->excluded, true); } EFOR;
}

void exclude_causal(Causal *c)
{
  exclude_action2(c->consumer, c);
}


void exclude_action(Action *a)
{
  exclude_action2(a, NULL);
}


static void make_order_ac(Action *a, Causal *c)
{
  if (a != (c)->consumer && ((!(a)->excluded && c->required) || (a->used && !(c)->excluded))) {
    if (cannot_precede_ca(c, a)) order_before_ac(a, c);
    if (cannot_precede_ac(a, c)) order_before_ca(c, a);
  }
}

void order_before_aa(Action *a1, Action *a2)
{
  TimeVal d = delta_aa(a1, a2);

  trace_proc(order_before_aa, a1, a2);
  if (a1->used) update_inf_a(a2, first_start(a1) + d);
  if (a2->used) update_sup_a(a1, last_start(a2) - d);
  //if (!precedes(a1, a2) && (!duration(a1) || !duration(a2))) FOR(c, a1->causals) { rem_producer(c, a2); } EFOR; // rajout pour actions de synchro
  make_precede(a1, a2);
}

void order_before_ca(Causal *c, Action *a)
{
  trace_proc(order_before_ca, c, a);
  order_before_aa(c->consumer, a);
}

void order_before_ac(Action *a, Causal *c)
{
  Action *prod = get_producer(c);

  trace_proc(order_before_ac, a, c);
  if (prod) {
    if (a != prod && prod->used) order_before_aa(a, prod);
  } else {
    if (can_produce(c, a)) {
      if (c->required) update_sup_a(a, last_start(c));
      if (a->used) update_inf_c(c, first_start(a));
    } else {
      TimeVal d = delta_ac(a, c);
      if (c->required) update_sup_a(a, last_start(c) - d);
      if (a->used) update_inf_c(c, first_start(a) + d);
    }
    make_precede(a, c->consumer);
  }
}

void make_precede(Action *a1, Action *a2)
{
  Action **act;
  long act_nb;
  
  if (a1 == a2 || precedes(a1, a2)) return;
  if (precedes(a2, a1)) {
    if (a1->used) { exclude_action(a2); return; }
    else if (a2->used) { exclude_action(a1); return; }
  }
  if (opt.complete_qualprec) { 
    if ((a1)->excluded || (a2)->excluded) return; 
    act = actions; act_nb = actions_nb;
  } else {
    if (!(a1)->used || !(a2)->used)  return;
    act = active_actions; act_nb = active_actions_nb;
  }
  set_precedes(a1, a2);
  if (!a1->used && !a2->used) return;
  protect(a1, a2);
  FOR(x, act) {
    if (!x->excluded && x != a1 && x != a2) {
      if (a2->used && precedes(a2, x) && !precedes(a1, x)) {
	order_before_aa(a1, x);
	if (a1->excluded) return;
	if (x->excluded) continue;
      }
      if (a1->used && precedes(x, a1) && !precedes(x, a2)) {
	order_before_aa(x, a2);
	if (a2->excluded) return;
	if (x->excluded) continue;
      }
    }
  } EFOR;
  protect(a1, a2);
  activate_action(a1);
  activate_action(a2);
}

static void protect(Action *a1, Action *a2)
{
  Causal **caus;
  long caus_nb;

  if (a2->used)
    FOR(f, a1->add) {
      if (edeletes(a2, f)) {
	if  (opt.complete_qualprec) { caus = f->causals; caus_nb = f->causals_nb; }
	else { caus = f->active_causals; caus_nb = f->active_causals_nb; }
	FOR(c, caus) {
	  if (precedes(a2, c->consumer)) {
	    rem_producer(c, a1);
	  }
	} EFOR;
      }
    } EFOR;
    
  if (a1->used)
    FOR(c, a2->causals) {
      if (edeletes(a1, c->fluent))
	FORPROD(prod, c) {
	  if (precedes(prod, a1))
	    rem_producer(c, prod);
	} EFOR;
    } EFOR;
}


#define linearval(x,xa,ya,xb,yb) time_round(((x) - (xa)) * (long double) ((yb) - (ya)) / ((xb) - (xa)) + (ya))

void evaluate_ac_forward(ActivityConstraint *ac, TimeVal fs, TimeVal ls, TimeVal *acmin, TimeVal *acmax)
{
  if (ac->time) {
    TimeVal xa, ya, xb, yb, tmp, dur;
    long i;
    *acmin = MAXTIME;
    *acmax = 0;
    dur = ac->dur[0].t;
    xb = ac->time[0].t;
    yb = xb + dur;
    if (fs < xb) { minimize(*acmin, fs + dur); maximize(*acmax, fs + dur); }
    if (ls < xb) { minimize(*acmin, ls + dur); maximize(*acmax, ls + dur); }
    else {
      for (i = 1; i < ac->time_nb; i++) {
	dur = ac->dur[i].t;
	xa = xb;
	ya = yb;
	xb = ac->time[i].t;
	yb = xb + dur;
	if (xa <= fs && fs < xb) { tmp = linearval(fs, xa, ya, xb, yb); minimize(*acmin, tmp); maximize(*acmax, tmp); }
	if (fs <= xb && xb < ls) { minimize(*acmin, yb); maximize(*acmax, yb); }
	if (ls <= xb) { tmp = linearval(ls, xa, ya, xb, yb); minimize(*acmin, tmp); maximize(*acmax, tmp); break; }
      }
      if (xb <= fs) { minimize(*acmin, fs + dur); maximize(*acmax, fs + dur); }
      if (xb <= ls) { minimize(*acmin, ls + dur); maximize(*acmax, ls + dur); }
    }
    *acmin += ac->min.t;
    *acmax += ac->max.t;
    //printf("ICI : min = %lld max = %lld -- fs = %lld ls = %lld\n", *acmin, *acmax, fs, ls); fflush(cptout);
  } else {
    *acmin = fs + ac->min.t;
    *acmax = ls + ac->max.t;
  }
}

void evaluate_ac_backward(ActivityConstraint *ac, TimeVal fs, TimeVal ls, TimeVal *acmin, TimeVal *acmax)
{
  if (ac->time) {
    TimeVal xa, ya, xb, yb, dur;
    long i;
    *acmin = -1;
    fs -= ac->max.t;
    ls -= ac->min.t;
    dur = ac->dur[0].t;
    xb = ac->time[0].t;
    yb = xb + dur;
    if (fs <= yb) *acmin = fs - dur;
    if (ls <= yb) *acmax = ls - dur;
    for (i = 1; i < ac->time_nb; i++) {
      dur = ac->dur[i].t;
      xa = xb;
      ya = yb;
      xb = ac->time[i].t;
      yb = xb + dur;
      if (*acmin == -1 && ya <= fs && fs <= yb) *acmin = linearval(fs, ya, xa, yb, xb);
      if (ya <= ls && ls <= yb) { *acmax = linearval(ls, ya, xa, yb, xb); }
    }
    if (yb <= fs) *acmin = fs - dur;
    if (yb <= ls) *acmax = ls - dur;
    //printf("ICI : min = %lld max = %lld -- fs = %lld ls = %lld\n\n", *acmin, *acmax, fs, ls); fflush(cptout);
  } else {
    *acmin = fs - ac->max.t;
    *acmax = ls - ac->min.t;
  }
}


void clone_action(Action *a, Causal *causal)
{
  Action *clone = actions[actions_nb];

  if (!clone) error(max_plan_length, "Increase the maximum plan length (actually %ld)", opt.max_plan_length);
  store(a->origin->nb_instances, a->origin->nb_instances - 1);
  memcpy(clone, a, CSIZE_ACTION); 
  clone_bound_variable(&clone->start, &a->start);
  bitarray_copy(clone->precedences, a->precedences, total_actions_nb);
  store(actions_nb, actions_nb + 1);
#ifdef RESOURCES
  if (a->synchro) clone_bound_variable(&clone->reslevel, &a->reslevel);
#endif

  FOR2(c1, clone->causals, c2, a->causals) { 
    memcpy(c1, c2, CSIZE_CAUSAL); 
    clone_bound_variable(&c1->start, &c2->start);
    clone_enum_variable(&c1->support, &c2->support);
    store(causals_nb, causals_nb + 1);
    vstore(c1->fluent->causals, c1);
  } EFOR;

  FOR(f, a->add) {
    FOR(c, f->causals) {
      if (c != causal && can_produce(c, a)) {
	store(c->fluent->indac[clone->id], f->producers_nb);
	add_val_variable(&c->support, (Value) f->producers_nb);
      }
    } EFOR;
    vstore(f->producers, clone);
  } EFOR;

  FOR(f, clone->del) { vstore(f->deleters, clone); } EFOR;
  FOR(f, clone->edel) { vstore(f->edeleters, clone); } EFOR;
  FOR(f, clone->prec) { vstore(f->consumers, clone); } EFOR;
  if (opt.complete_qualprec) {
    FOR(a2, actions) {
      if (precedes(a2, a)) set_precedes(a2, clone);
      if (precedes(a, a2)) set_precedes(clone, a2);
    } EFOR;
  }
  activate_action(clone);
}


void more_propagations(void)
{
  if (opt.pddl21) {
    FOR(a, active_actions) {
      FOR(ac, a->ac_constraints) {
        if (ac->mandatory) {
          FOR(c, ac->fluent->causals) {
            if (can_produce(c, a)) {
              if (!c->consumer->used) {
                if (c->consumer->origin->nb_instances > 0) clone_action(c->consumer, NULL);
                set_producer(c, a);
                use_action(c->consumer);
                return;
              }
              goto ac_found;
            }
          } EFOR;
          contradiction();
        }
      ac_found:;
      } EFOR;
    } EFOR;
  }
  if (opt.global_mutex_sets) global_mutex_sets();
  if (opt.task_intervals) task_intervals();
}

void restrict_problem(Fluent **init, long init_nb, Fluent **goal, long goal_nb)
{
  static Causal **end_action_causals;
  static int end_action_causals_nb;
  bool is_goal[fluents_nb];
  bool is_init[fluents_nb];
  bool cqp = opt.complete_qualprec;
  bool gms = opt.global_mutex_sets;

  if (goal_nb != -1) {
    if (end_action_causals == NULL) vector_copy(end_action_causals, end_action->causals);
    FOR(f, fluents) { is_init[f->id] = false; is_goal[f->id] = false; } EFOR;
    FOR(f, init) { is_init[f->id] = true; } EFOR;
    FOR(f, goal) { is_goal[f->id] = true; } EFOR;
    FOR(f, fluents) { if (!is_init[f->id]) FOR(c, f->causals) { rem_producer(c, start_action); } EFOR; } EFOR;
    end_action->causals_nb = 0;
    FOR(c, end_action_causals) {
      if (is_goal[c->fluent->id]) end_action->causals[end_action->causals_nb++] = c;
      else store(c->excluded, true);
    } EFOR;
  }
  use_action(start_action);
  use_action(end_action);
  update_sup_a(start_action, 0);
  update_sup_a(end_action, pddl_domain->max_makespan.t);
  
#ifndef RATP
  opt.global_mutex_sets = true;
#endif
  opt.complete_qualprec = true;

  FOR(a, actions) {
    make_precede(start_action, a);
    make_precede(a, end_action);
  } EFOR;

  propagate();
  
  opt.global_mutex_sets = gms;
  opt.complete_qualprec = cqp;
}

