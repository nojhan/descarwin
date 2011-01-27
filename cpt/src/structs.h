/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : structs.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#ifndef STRUCT_H
#define STRUCT_H


#include "var.h"
#include "pddl.h"


#ifndef PDDL_H
typedef struct Fluent Fluent;
typedef struct Action Action;
typedef struct Resource Resource;
#endif
typedef struct Causal Causal;
typedef struct ActivityConstraint ActivityConstraint;
#ifdef RESOURCES
typedef struct ResourceData ResourceData;
typedef struct ResourceLocal ResourceLocal;
#endif


struct Fluent {
  long id;
  long bit_index;
  long bit_mask;
  PDDLAtom *atom;
  Value *indac;
  TimeVal *pair_cost;
  BitArray mutex;
#ifdef RESOURCES
  Resource *resource;
  bool no_branching;
#endif

  TimeVal init;
  TimeVal end;
  VECTOR(Action *, consumers);
  VECTOR(Action *, producers);
  VECTOR(Action *, deleters);
  VECTOR(Action *, edeleters);
  VECTOR(Causal *, active_causals);
  VECTOR(Causal *, causals);
};

struct Action {
  /* clonable fields */
  PDDLOperator *ope;
  VECTOR(PDDLTerm *, parameters);
  Action *origin;
  TimeStruct dur;
  TimeStruct rdur;
  Action *pddl21_next;
  Action *pddl21_prev;
  TimeVal init;
  bool used;
  bool excluded;
  VECTOR(Fluent *, prec);
  VECTOR(Fluent *, add);
  VECTOR(Fluent *, del);
  VECTOR(Fluent *, edel);
  VECTOR(ActivityConstraint *, ac_constraints);
  long causals_nb;
  TimeVal *distances;
  long weight;
#ifdef RESOURCES
  VECTOR(ResourceLocal *, resources);
  bool synchro;
#endif
  BitArray consumes;
  BitArray produces;
  BitArray deletes;
  BitArray edeletes;
  BitArray mutex;
  BitArray deletes_included;

  /* static fields */
  long id;
  long bit_index;
  long bit_mask;
  BoundVariable start;
  Causal **causals;
  long nb_instances;
  BitArray precedences;
#ifdef RESOURCES
  BoundVariable reslevel;
#endif
};

#define CSIZE_ACTION offsetof(Action, id)

struct Causal {
  /* clonable fields */
  Fluent *fluent;
  Causal *origin;
  long weight;

  /* static fields */
  bool required;
  bool excluded;
  Action *consumer;
  Action *best_producer;
  EnumVariable support;
  BoundVariable start;
};

#define CSIZE_CAUSAL offsetof(Causal, required)

struct ActivityConstraint {
  Fluent *fluent;
  TimeStruct min;
  TimeStruct max;
  TimeVal real_min;
  bool mandatory;
  VECTOR(TimeStruct, time);
  VECTOR(TimeStruct, dur);
};

#ifdef RESOURCES
struct Resource {
  long id;
  PDDLAtom *atom;
  Fluent *fluent_available;
  Fluent *fluent_modified;
  Fluent *fluent_synchro;
  VECTOR(ResourceLocal *, reslocals);
};

struct ResourceLocal {
  Resource *resource;
  Action *action;
  long index_causal;
  TimeVal min_level;
  TimeVal max_level;
  TimeVal increased;
  TimeVal decreased;
  TimeVal assigned;
};
#endif

#define FORPROD(a, c) do { Value _i; for (_i = (c)->support.min; _i <= (c)->support.max; _i = (c)->support.bucket[_i+1]) { Action *a = (c)->fluent->producers[_i];

#define set_consumes(a, f) bitarray_set((a)->consumes, f)
#define unset_consumes(a, f) bitarray_unset((a)->consumes, f)
#define consumes(a, f) bitarray_get((a)->consumes, f)

#define set_produces(a, f) bitarray_set((a)->produces, f)
#define unset_produces(a, f) bitarray_unset((a)->produces, f)
#define produces(a, f) bitarray_get((a)->produces, f)

#define set_deletes(a, f) NEST( bitarray_set((a)->deletes, f); bitarray_set((a)->edeletes, f); )
#define deletes(a, f) bitarray_get((a)->deletes, f)

#define set_edeletes(a, f) bitarray_set((a)->edeletes, f)
#define set_temp_edeletes(a, f) bitarray_save_and_set((a)->edeletes, f)
#define edeletes(a, f) bitarray_get((a)->edeletes, f)

#define set_amutex(a1, a2) NEST( if ((a1)->id < (a2)->id) bitarray_set((a2)->mutex, a1); else bitarray_set((a1)->mutex, a2); )
#define amutex(a1, a2) ((a1)->origin->id < (a2)->origin->id ? bitarray_get((a2)->mutex, a1->origin) : bitarray_get((a1)->mutex, a2->origin))

#define set_fmutex(a1, a2) NEST( if ((a1)->id < (a2)->id) bitarray_set((a2)->mutex, a1); else bitarray_set((a1)->mutex, a2); )
#define unset_fmutex(a1, a2) NEST( if ((a1)->id < (a2)->id) bitarray_unset((a2)->mutex, a1); else bitarray_unset((a1)->mutex, a2); )
#define fmutex(a1, a2) ((a1)->id < (a2)->id ? bitarray_get((a2)->mutex, a1) : bitarray_get((a1)->mutex, a2))

#define set_precedes(a1, a2) bitarray_save_and_set((a1)->precedences, a2)
#define precedes(a1, a2) bitarray_get((a1)->precedences, a2)

#define set_distance(a1, a2, d) (a1)->distances[(a2)->id] = d
#define distance(a1, a2) (a1)->distances[(a2)->origin->id]
#define store_distance(a1, a2, d) store((a1)->distances[(a2)->num], d)

#define distance_ca(c, a) distance((c)->consumer, a)
#define distance_ac(a, c) ({TimeVal _d = MAXCOST; FORPROD(_a2, c) { minimize(_d, distance(a, _a2)); } EFOR; _d; })

#define set_pair_cost(a1, a2, d) NEST( (a1)->pair_cost[(a2)->id] = d; (a2)->pair_cost[(a1)->id] = d; )
#define pair_cost(a1, a2) (a1)->pair_cost[(a2)->id]

#define set_deletes_included(a1, a2) bitarray_set((a1)->deletes_included, a2)
#define deletes_included(a1, a2) bitarray_get((a1)->deletes_included, a2)

#define duration(a) ((a)->dur.t)
#define duration_rat(a) ((a)->dur.q)
#define first_start(a) ((a)->start.inf)
#define last_start(a) ((a)->start.sup)
#define first_end(a) (first_start(a) + duration(a))
#define last_end(a) (last_start(a) + duration(a))
#define delta_aa(a1, a2) (duration(a1) + distance(a1,a2))
#define delta_ca(c, a) delta_aa((c)->consumer, a)
#define delta_ac(a, c) (duration(a) + distance_ac(a, c))
#define slack_aa(a1, a2) (last_start(a2) - (first_start(a1) + delta_aa(a1, a2)))
#define slack_ca(c, a) slack_aa((c)->consumer, a)
#define slack_ac(a, c) (last_start(c) - (first_start(a) + delta_ac(a, c)))

#define cannot_precede_time_aa(a1, a2) (first_start(a1) + delta_aa(a1, a2) > last_start(a2))
#define cannot_precede_aa(a1, a2) (precedes(a2, a1) || cannot_precede_time_aa(a1, a2))
#define cannot_precede_ca(c, a) cannot_precede_aa((c)->consumer, a)


#define cannot_precede_ac(a, c)						\
  (cannot_precede_aa(a, (c)->consumer) ||				\
   (!can_produce(c, a) &&						\
    ({ bool _r = true;							\
      FORPROD(a2, c) {						\
	if (!precedes(a2, a) && first_start(a) + delta_aa(a, a2) <= mini(last_start(a2), last_start(c))) \
	  { _r = false; break; }} EFOR; _r;})))

/* #define cannot_precede_ac(a, c) 						\ */
/*   (cannot_precede_aa(a, (c)->consumer) || (!can_produce(c, a) && first_start(a) + delta_ac(a, c) > last_start(c))) */

#define can_precede_aa(a1, a2) (first_start(a1) + delta_aa(a1, a2) <= first_start(a2))
#define can_precede_ca(c, a) can_precede_aa((c)->consumer, a)
#define can_precede_ac(a, c) (can_produce(c, a) || first_start(a) + delta_ac(a, c) <= first_start(c))

#define update_sup_a(v, x) NEST( if (x < last_start(v)) { trace_proc(update_sup_a, v, x); update_sup_variable(&(v)->start, x); } )
#define update_sup_c(v, x) NEST( if (x < last_start(v)) { trace_proc(update_sup_c, v, x); update_sup_variable(&(v)->start, x); } )
#define update_inf_a(v, x) NEST( if (x > first_start(v)) { trace_proc(update_inf_a, v, x); update_inf_variable(&(v)->start, x); } )
#define update_inf_c(v, x) NEST( if (x > first_start(v)) { trace_proc(update_inf_c, v, x); update_inf_variable(&(v)->start, x); } )
#define increment_inf_a(v, x) update_inf_a(v, first_start(v) + x)
#define decrement_sup_a(v, x) update_sup_a(v, last_start(v) - x)

#define producer2value(c, a) (c)->fluent->indac[(a)->id]
#define value2producer(c, i) (c)->fluent->producers[i]

#define set_producer(c, a) instantiate_variable(&(c)->support, producer2value(c, a))
#define rem_producer(c, a) NEST( if (can_produce(c, a)) { trace_proc(rem_producer, c, a); remove_val_variable(&(c)->support, producer2value(c, a)); } )

#define can_produce(c, a) ({ Value _v = producer2value(c, a); (!(c)->excluded && val_known(_v) && ((val_unknown((c)->support.value) && (c)->support.bucket[_v] == _v) || (c)->support.value == _v));  })
#define is_produced(c) val_known((c)->support.value)

#define get_producer(c) ((c)->required && is_produced(c) ? value2producer(c, (c)->support.value) : NULL)
#define get_nbprods(c) (c)->support.card
#define activate_action(a) post_bound_event(&(a)->start);
#define activate_causal(c) NEST( if (is_produced(c)) post_instantiate_event(&(c)->support); else post_removal_event(&(c)->support); )

#define find_ac_constraint(a, f) ({ ActivityConstraint *_ac = NULL; FOR(aci, a->ac_constraints) { if (aci->fluent == f) { _ac = aci; break; }} EFOR; _ac; })

#ifdef RESOURCES
#define resource_local(a, r) r->reslocals[a->origin->id]
#define min_level(a) (a->reslevel.inf)
#define max_level(a) (a->reslevel.sup)
#endif


#endif /* STRUCTS_H */
