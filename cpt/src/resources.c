/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : resources.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */

#ifdef RESOURCES

#include "cpt.h"
#include "structs.h"
#include "problem.h"
#include "propagations.h"
#include "preprocess.h"
#include "scheduling.h"
#include "trace_planner.h"
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


#define requires_min_level(r) (time_known(r->min_level))
#define requires_max_level(r) (time_known(r->max_level))
#define increases(r) (time_known(r->increased) && r->increased > 0)
#define decreases(r) (time_known(r->decreased) && r->decreased > 0)
#define assigns(r) time_known(r->assigned)
#define decrease(a, x) NEST( if (x < max_level(a)) { trace_proc(update_sup_a, a, x); update_sup_variable(&(a)->reslevel, x); } )
#define increase(a, x) NEST( if (x > min_level(a)) { trace_proc(update_inf_a, a, x); update_inf_variable(&(a)->reslevel, x); } )
#define assign(a, x) NEST( increase(a, x); decrease(a, x); )
#define causal_available(a, r) a->causals[r->index_causal]


/*---------------------------------------------------------------------------*/
/* Static Functions                                                          */
/*---------------------------------------------------------------------------*/


void protect_causal_resource2(Action *synchro_before);


/*****************************************************************************/


void propagate_resource(Action *a)
{
  protect_causal_resource(a->causals[1]);
  FOR(c, a->add[1]->causals) {
    //if (get_producer(c) == a && c->consumer != end_action) {
    if (!c->excluded && can_produce(c, a) && c->consumer != end_action) {
      protect_causal_resource(c);
      //break;
    }
  } EFOR;
  if (!a->excluded) protect_causal_resource2(a);
}

void exclude_resource(Action *a)
{
  exclude_action(a);
}


void protect_causal_resource(Causal *causal_synchro)
{ 
  Action *synchro_after = causal_synchro->consumer;
  Action *synchro_before = get_producer(causal_synchro);
  Resource *resource = causal_synchro->fluent->resource;
  Causal *causal_modified = synchro_after->causals[0];
  Action *modifier = get_producer(causal_modified);
  long min_level_after = MAXTIME;
  long max_level_after = -1;
  long min_level_before = MAXTIME;
  long max_level_before = -1;


  if (synchro_after == end_action) {
    ResourceLocal *reslocal = resource_local(synchro_after, resource);
    if (synchro_before) {
      //set_producer(causal_available(synchro_after, reslocal), synchro_before);
      if (requires_min_level(reslocal)) increase(synchro_before, reslocal->min_level);
      if (requires_max_level(reslocal)) decrease(synchro_before, reslocal->max_level);
    } else {
      Causal *causal = causal_available(synchro_after, reslocal);
      FORPROD(sync, causal) {
	if ((requires_min_level(reslocal) && max_level(sync) < reslocal->min_level) ||
	    (requires_max_level(reslocal) && min_level(sync) > reslocal->max_level))
	  rem_producer(causal, sync);
      } EFOR;
    }
    return;
  }

  if (modifier) {
    if (modifier == start_action) {
      set_producer(causal_synchro, start_action);
      synchro_before = start_action;
    } else {
      ResourceLocal *reslocal = resource_local(modifier, resource);
      Causal *causal = causal_available(modifier, reslocal);
      Action *sync = get_producer(causal);
      if (sync) {
	set_producer(causal_synchro, sync);
	synchro_before = sync;
      } else if (synchro_before) {
	set_producer(causal, synchro_before);
      }
    }
  } else if (synchro_before) {
    if (synchro_before == start_action) {
      set_producer(causal_modified, start_action);
      modifier = start_action;
    } else {
      FORPROD(prod, causal_modified) {
	ResourceLocal *reslocal = resource_local(prod, resource);
	Causal *causal = causal_available(prod, reslocal);
	if (!can_produce(causal, synchro_before)) rem_producer(causal_modified, prod); //contradiction();
      } EFOR;
    }
  }

  FORPROD(prod, causal_modified) {
    ResourceLocal *reslocal = resource_local(prod, resource);

    if (assigns(reslocal)) { minimize(min_level_after, reslocal->assigned); maximize(max_level_after, reslocal->assigned); }

    if (synchro_before) {
      if ((decreases(reslocal) && (max_level(synchro_before) - reslocal->decreased < min_level(synchro_after))) ||
	  (increases(reslocal) && (min_level(synchro_before) + reslocal->increased > max_level(synchro_after)))) {
	rem_producer(causal_modified, prod);
	continue;
      }
      if (requires_min_level(reslocal)) minimize(min_level_before, reslocal->min_level); else min_level_before = 0;
      if (requires_max_level(reslocal)) maximize(max_level_before, reslocal->max_level); else max_level_before = MAXTIME;
    }

    if (prod != start_action && !assigns(reslocal)) {
      Causal *causal = causal_available(prod, reslocal);
      FORPROD(sync, causal) {
	if (cannot_precede_aa(sync, prod)) continue;
	if ((requires_min_level(reslocal) && max_level(sync) < reslocal->min_level) ||
	    (requires_max_level(reslocal) && min_level(sync) > reslocal->max_level)) {
	  rem_producer(causal, sync);
	} else if (!assigns(reslocal)) {
	  if (decreases(reslocal)) {
	    if (requires_min_level(reslocal)) minimize(min_level_after, max(reslocal->min_level, min_level(sync)) - reslocal->decreased); 
	    else minimize(min_level_after, min_level(sync) - reslocal->decreased);
	    if (requires_max_level(reslocal)) maximize(max_level_after, min(reslocal->max_level, max_level(sync)) - reslocal->decreased);
	    else maximize(max_level_after, max_level(sync) - reslocal->decreased);
	  }  else if (!increases(reslocal)) minimize(min_level_after, min_level(sync));
	  
	  if (increases(reslocal)) {
	    if (requires_max_level(reslocal)) maximize(max_level_after, min(reslocal->max_level, max_level(sync)) + reslocal->increased);
	    else maximize(max_level_after, max_level(sync) + reslocal->increased);
	    if (requires_min_level(reslocal)) minimize(min_level_after, max(reslocal->min_level, min_level(sync)) + reslocal->increased);
	    else minimize(min_level_after, min_level(sync) + reslocal->increased);
	  }  else if (!decreases(reslocal)) maximize(max_level_after, max_level(sync));
	  
	}

      } EFOR;
    }
    
  } EFOR;

  increase(synchro_after, min_level_after);
  decrease(synchro_after, max_level_after);

  if (synchro_before && synchro_before != start_action) {
    increase(synchro_before, min_level_before);
    decrease(synchro_before, max_level_before);
  }


}


void protect_causal_resource2(Action *synchro_before)
{ //return;
  Fluent *fluent_synchro = synchro_before->add[1];
  Resource *resource = fluent_synchro->resource;
  long min_level_before = MAXTIME;
  long max_level_before = -1;



  FOR(causal_synchro, fluent_synchro->causals) {
    //Action *synchro_after = causal_synchro->consumer;
    //Causal *causal_modified = synchro_after->causals[0];
    if (!can_produce(causal_synchro, synchro_before)) continue;
    goto suite;
  } EFOR;
  print_action(synchro_before);
  printf("\n");
 suite:

  FOR(causal_synchro, fluent_synchro->causals) {
    Action *synchro_after = causal_synchro->consumer;
    Causal *causal_modified = synchro_after->causals[0];

    if (!can_produce(causal_synchro, synchro_before)) continue;
    if (synchro_after == end_action) { return; }

    if (synchro_after == synchro_before) continue;

    FORPROD(prod, causal_modified) {
      ResourceLocal *reslocal = resource_local(prod, resource);
      
      if (cannot_precede_aa(synchro_before, prod)) continue;
      if (!requires_min_level(reslocal) && !requires_max_level(reslocal) && assigns(reslocal)) return;

      if (requires_min_level(reslocal)) {
	if (increases(reslocal)) minimize(min_level_before, max(reslocal->min_level, min_level(synchro_after)) - reslocal->increased);
	else if (decreases(reslocal)) minimize(min_level_before, max(reslocal->min_level, min_level(synchro_after)) + reslocal->decreased);
	else minimize(min_level_before, max(reslocal->min_level, min_level(synchro_after)));
      } else {
	if (increases(reslocal)) minimize(min_level_before, min_level(synchro_after) - reslocal->increased);
	if (decreases(reslocal)) minimize(min_level_before, min_level(synchro_after) + reslocal->decreased);
	if (assigns(reslocal)) minimize(min_level_before, min_level(synchro_before));
      }

      if (requires_max_level(reslocal)) {
	if (increases(reslocal)) maximize(max_level_before, min(reslocal->max_level, max_level(synchro_after)) - reslocal->increased);
	else if (decreases(reslocal)) maximize(max_level_before, min(reslocal->max_level, max_level(synchro_after)) + reslocal->decreased);
	else maximize(max_level_before, min(reslocal->max_level, max_level(synchro_after)));
      } else {
	if (increases(reslocal)) maximize(max_level_before, max_level(synchro_after) - reslocal->increased);
	if (decreases(reslocal)) maximize(max_level_before, max_level(synchro_after) + reslocal->decreased);
	if (assigns(reslocal)) maximize(max_level_before, max_level(synchro_before));
      }

      
      print_action(synchro_before);
      print_action(synchro_after);
      printf("\n\t");
      print_action(prod);
      printf("  %ld %ld\n", min_level_before, max_level_before);
      
      
    } EFOR;

/*     print_action(synchro_before); */
/*     print_action(synchro_after); */
/*     printf("  %d %d\n", min_level_before, max_level_before); */
    
} EFOR;

 if (min_level_before > min_level(synchro_before) || max_level_before < max_level(synchro_before)) {
    print_action(synchro_before);
    printf("  %ld %ld\n", min_level_before, max_level_before);
  }
  printf("\n============\n\n");


  //if (max_level_before == -1) return;
  increase(synchro_before, min_level_before);
  if (max_level_before < 100000)
    decrease(synchro_before, max_level_before);
}
	
#endif
