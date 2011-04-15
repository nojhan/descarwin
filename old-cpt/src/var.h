/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : var.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#ifndef VAR_H
#define VAR_H


#include <setjmp.h>


typedef enum {NoEvent, RemovalEvent, InstantiateEvent, BoundEvent} VarEvent;

typedef void (*PropagationProc)(void *);

typedef void (*EmptyProc)(void *);

typedef struct EnumVariable EnumVariable;

struct EnumVariable {
  /* clonable fields */
  Value min;
  Value max;
  Value card;
  Value value;
  long bucket_nb;

  /* static fields */
  Value *bucket;
  void *hook;
  VarEvent event;
  PropagationProc propagate;
  EmptyProc empty;
  EnumVariable **idx_removal;
  EnumVariable **idx_instantiate;
};

#define clone_enum_variable(clone, v)		\
  NEST( (clone)->event = NoEvent;						\
	memcpy(clone, v, offsetof(EnumVariable, bucket));		\
	memcpy((clone)->bucket, (v)->bucket, (v)->bucket_nb * sizeof(Value)); )

typedef struct BoundVariable BoundVariable;

struct BoundVariable {
  /* clonable fields */
  TimeVal inf;
  TimeVal sup;

  /* static fields */
  void *hook;
  VarEvent event;
  PropagationProc propagate;
  EmptyProc empty;
  BoundVariable **idx_bound;
};

#define clone_bound_variable(clone, v)					\
  NEST( (clone)->event = NoEvent;					\
	memcpy(clone, v, offsetof(BoundVariable, hook)); )


extern void create_enum_variable(EnumVariable *v, long n, long sup, void *hook, PropagationProc propagate, EmptyProc empty);
extern void create_bound_variable(BoundVariable *v, TimeVal min, TimeVal max, void *hook, PropagationProc propagate, EmptyProc empty);
extern void remove_val_variable(EnumVariable *v, Value i);
extern void add_val_variable(EnumVariable *v, Value i);
extern void instantiate_variable(EnumVariable *v, Value i);
extern void update_inf_variable(BoundVariable *v, TimeVal i);
extern void update_sup_variable(BoundVariable *v, TimeVal i);
extern void propagate(void);
extern void more_propagations(void);


/* Backtrack and contradiction management */

#define STACK_INC 100000

typedef struct WorldStack WorldStack;

struct WorldStack {
  char *data;
  long world;
  long initial_world; 
  long size;
  long max_size;
  long nodes;
  long nodes_run;
  long backtracks;
  long backtracks_run;
  long backtrack_limit;
  long propagations;
  long propagations_limit;
  bool limit_initial_propagation;
  jmp_buf *env;
};

extern WorldStack wstack;

#define nb_nodes() wstack.nodes
#define nb_backtracks() wstack.backtracks
#define reset_backtracks() (wstack.backtracks = 0)
#define current_world() (wstack.world - wstack.initial_world)
#define world_size() wstack.max_size
#define set_backtrack_limit(n) NEST( wstack.nodes_run = 0; wstack.backtracks_run = 0; wstack.backtrack_limit = n; )
#define backtrack_limit() wstack.backtrack_limit
#define backtrack_limit_reached() (wstack.backtracks_run >= wstack.backtrack_limit)
#define nb_nodes_run() wstack.nodes_run
#define nb_backtracks_run() wstack.backtracks_run

#define increase_propagations() wstack.propagations++
#define nb_propagations() wstack.propagations
#define propagations_limit_reached() (limit_initprop() && wstack.propagations >= wstack.propagations_limit)
#define set_limit_initprop(p, n) NEST( wstack.propagations = 0; wstack.limit_initial_propagation = p; wstack.propagations_limit = n; )
#define limit_initprop() wstack.limit_initial_propagation

#define alloc_stack() cpt_realloc(wstack.data, (wstack.max_size += STACK_INC))
#define push_stack(v) NEST ( if ((wstack.size += sizeof(v)) > wstack.max_size) alloc_stack(); *((typeof(v) *) (wstack.data + wstack.size - sizeof(v))) = v; )
#define pop_stack(type) (*((type *) (wstack.data + (wstack.size -= sizeof(type)))))
#define pop_var(var) (var = pop_stack(typeof(var)))

#define save(v) NEST( if (wstack.world > 0) { push_stack(v); push_stack((char) sizeof(v)); push_stack(&(v)); } )
#define store(v, i) NEST( save(v); v = i; )
#define vstore(t, i) NEST( save(t##_nb); t[t##_nb++] = i; )

#define new_world(count) ({ jmp_buf _env; push_stack(wstack.env); wstack.env = &_env; push_stack((void*) NULL); wstack.world++; \
      if (count) { wstack.nodes++; wstack.nodes_run++; trace_proc(new_world); } else wstack.initial_world = wstack.world; (setjmp(_env) == 0); })

#define new_world2(count) ({ jmp_buf _env; push_stack(wstack.env); wstack.env = &_env; push_stack((void*) NULL); wstack.world++; \
      trace_proc(new_world); (setjmp(_env) == 0); })

#define protected() ({ jmp_buf _env; push_stack(wstack.env); wstack.env = &_env; push_stack((void*) NULL); (setjmp(_env) == 0); })

#define backtrack(ret)							\
  NEST(									\
       void *_ptr; jmp_buf *_env = wstack.env;				\
       while (pop_var(_ptr)) {						\
	 switch (pop_stack(char)) {					\
	 case 4: pop_var(*(long *) _ptr); break;			\
	 case 2: pop_var(*(short *) _ptr); break;			\
	 case 1: pop_var(*(char *) _ptr); break;			\
	 case 8: pop_var(*(long long *) _ptr); break;			\
	 }}								\
       pop_var(wstack.env);						\
       wstack.world--;							\
       if (ret) {							\
	 raz_event_queues();						\
	 if (wstack.world >= wstack.initial_world) trace_proc(backtrack); \
	 longjmp(*_env, 1); } )

#define contradiction()							\
  NEST(									\
       if (wstack.world > wstack.initial_world && !backtrack_limit_reached()) { wstack.backtracks++; wstack.backtracks_run++; } \
       if (backtrack_limit_reached()) while (wstack.world > wstack.initial_world) backtrack(false); backtrack(true); )

#define restore_initial_world(w) NEST( while (wstack.world > (w)) backtrack(false); raz_event_queues(); )


/* Event queues management */

#define EVENT_QUEUE_TYPE(type)			\
  typedef struct  {					\
    type##Variable **elt, **first, **last, **end;	\
  } type##EventQueue;

EVENT_QUEUE_TYPE(Enum);
EVENT_QUEUE_TYPE(Bound);

extern EnumEventQueue queue_removal, queue_instantiate;
extern BoundEventQueue queue_bound;

extern VarEvent actual_event;

#define init_event_queues(ne, nb) NEST( init_queue(ne, removal); init_queue(ne, instantiate); init_queue(nb, bound); )
#define raz_event_queues() NEST( actual_event = NoEvent; raz_queue(removal); raz_queue(instantiate); raz_queue(bound); )

#define init_queue(n, q) _init_queue(n, queue_##q, idx_##q)
#define register_var(v, q) _register_var(v, queue_##q, idx_##q)
#define pop_event(v, q) _pop_event(v, queue_##q)
#define rem_event(v, q)  _rem_event(v, queue_##q, idx_##q)
#define raz_queue(q)  _raz_queue(queue_##q) 
#define post_event(v, q) _post_event(v, queue_##q, idx_##q)

#define _init_queue(n, q, idx) NEST( cpt_malloc(q.elt, n + 1); *q.elt = NULL; q.first = q.last = q.end = q.elt; )
#define _register_var(v, q, idx) NEST( *++q.end = v; (v)->idx = q.end; )
#define _shift(ptr, q) NEST( q.ptr = (q.ptr == q.end ? q.elt : q.ptr + 1); )
#define _pop_event(v, q) NEST( if (q.first != q.last) { v = *q.first; _shift(first, q); } else v = NULL; )
#define _rem_event(v, q, idx) NEST( (*q.first)->idx = (v)->idx; *(v)->idx = *q.first; *q.first = v; (v)->idx = q.first; _shift(first, q); )
#define _raz_queue(q) NEST( while (q.first != q.last) { (*q.first)->event = NoEvent; _shift(first, q); } )
#define _post_event(v, q, idx) NEST( *(v)->idx = *q.last; if (*q.last) (*q.last)->idx = (v)->idx; *q.last = v; (v)->idx = q.last; _shift(last, q); )

#define post_instantiate_event(v)						\
  NEST( if ((v)->event == NoEvent) { (v)->event = InstantiateEvent; post_event(v, instantiate); } \
	else if ((v)->event == RemovalEvent) { (v)->event = InstantiateEvent; rem_event(v, removal);  post_event(v, instantiate); } )
#define post_removal_event(v) NEST( if ((v)->event == NoEvent) { (v)->event = RemovalEvent; post_event(v, removal); } )
#define post_bound_event(v) NEST( if ((v)->event == NoEvent) { (v)->event = BoundEvent; post_event(v, bound); } )


#endif /* VAR_H */

