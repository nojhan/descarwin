/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : var.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#include "cpt.h"
#include "trace.h"
#include "var.h"
#include "globs.h"


/*---------------------------------------------------------------------------*/
/* Local Variables                                                           */
/*---------------------------------------------------------------------------*/


WorldStack wstack;
EnumEventQueue queue_removal, queue_instantiate;
BoundEventQueue queue_bound;
VarEvent actual_event;


/*****************************************************************************/


void create_enum_variable(EnumVariable *v, long n, long sup, void *hook, PropagationProc propagate, EmptyProc empty)
{
  long i;
  
  if (n + sup > MAXVAL) 
    error(bucket, "Too many values in bucket");
  v->min = 0;
  v->max = n - 1;
  v->card = n;
  v->value = (n == 1 ? 0 : VAL_UNKNOWN);
  v->hook = hook;
  v->event = NoEvent;
  v->propagate = propagate;
  v->empty = empty;
  v->bucket_nb = n + sup + 1;
  cpt_malloc(v->bucket, v->bucket_nb);
  for (i = 0; i < v->bucket_nb; i++)
    v->bucket[i] = (i < n ? i : VAL_UNKNOWN);
  register_var(v, removal);
  register_var(v, instantiate);
}

void create_bound_variable(BoundVariable *v, TimeVal inf, TimeVal sup, void *hook, PropagationProc propagate, EmptyProc empty)
{
  v->inf = inf;
  v->sup = sup;
  v->hook = hook;
  v->event = NoEvent;
  v->propagate = propagate;
  v->empty = empty;
  register_var(v, bound);
}

void remove_val_variable(EnumVariable *v, Value i)
{
  Value *dom = v->bucket, prev, suiv;

  if (dom[i] == i) {
    if (v->min == v->max) v->empty(v->hook);
    else {
      store(v->card, v->card - 1);
      if (i == v->min) { store(v->min, dom[i + 1]); store(dom[i], VAL_UNKNOWN); }
      else {
	prev = (dom[i - 1] == i ? i - 2 : dom[i - 1]);
	if (i == v->max) { store(v->max, prev); store(dom[i], VAL_UNKNOWN); } 
	else {
	  suiv = dom[i + 1];
	  if (prev != i - 1) store(dom[i], VAL_UNKNOWN);
	  if (prev + 2 != suiv) store(dom[suiv - 1], prev);
	  store(dom[prev + 1], suiv);
 	  post_removal_event(v);
	  return;
	}
      }
      if (v->min == v->max) { store(v->value, v->min); post_instantiate_event(v); } 
      else post_removal_event(v);
    }
  }
}

void add_val_variable(EnumVariable *v, Value i)
{
  Value *dom = v->bucket, prev = v->max;

  store(dom[i], i);
  store(v->max, i);
  store(v->card, v->card + 1);
  if (prev < i - 1) {
    if (prev != i - 2) store(dom[i - 1], prev);
    store(dom[prev + 1], i);
  }
  if (val_known(v->value)) store(v->value, VAL_UNKNOWN);
}

void instantiate_variable(EnumVariable *v, Value i)
{
  Value *dom = v->bucket;
  
  if (dom[i] != i) v->empty(v->hook);
  else if (val_unknown(v->value)) {
    store(v->value, i);
    store(v->min, i);
    store(v->max, i);
    store(v->card, 1);
    post_instantiate_event(v);
  }
}

void update_inf_variable(BoundVariable *v, TimeVal i)
{
  if (i > v->inf) {
    if (i > v->sup) v->empty(v->hook); 
    else { store(v->inf, i); post_bound_event(v); }
  }
}

void update_sup_variable(BoundVariable *v, TimeVal i)
{
  if (i < v->sup) {
    if (i < v->inf) v->empty(v->hook); 
    else { store(v->sup, i); post_bound_event(v); }
  }
}

#define propagate_event_queue(q) \
  NEST( typeof(*queue_##q.elt) _v;					\
	pop_event(_v, q); if (_v) { actual_event = _v->event; _v->event = NoEvent; _v->propagate(_v->hook); goto loop; } )

#define propagate_all_queues()			\
  NEST(						\
       propagate_event_queue(instantiate);	\
       propagate_event_queue(bound);		\
       propagate_event_queue(removal); )

void propagate()
{
 loop:
  if (limit_initprop()) {
    increase_propagations();
    if (propagations_limit_reached()) return;
  }
  propagate_all_queues();
  more_propagations();
  propagate_all_queues();
}
