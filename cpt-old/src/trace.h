/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : trace.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#ifndef TRACE_H
#define TRACE_H


#define TR_normal 1
#define TR_error 1
#define TR_options 1
#define TR_monitor 1
#define TR_begin_bound 1
#define TR_end_bound 1
#define TR_restart 1
#define TR_problem_stats 1
#define TR_solution_plan 1
#define TR_plan_stats 1
#define TR_search_stats 1
#define TR_yahsp_anytime_search_stats 1
#define TR_new_world 2
#define TR_backtrack 2
#define TR_conflict_choice 4
#define TR_support_choice 4
#define TR_start_time_choice 4
#define TR_mutex_choice 4
#define TR_action_used 8
#define TR_action_excluded 8
#define TR_producer_used 16
#define TR_rem_producer 16
#define TR_order_before_aa 32
#define TR_order_before_ca 32
#define TR_order_before_ac 32
#define TR_propagate_action 64
#define TR_propagate_causal 64
#define TR_synchronize_causal 64
#define TR_protect_causal 64
#define TR_protect_against 64
#define TR_update_sup_a 128
#define TR_update_inf_a 128
#define TR_update_sup_c 128
#define TR_update_inf_c 128
#define TR_ratp 256


#ifdef NOTRACE

#define begin_monitor(...)
#define end_monitor(...)
#define trace(v, args...)
#define trace_proc(...)

#else

#define begin_monitor _begin_monitor
#define end_monitor _end_monitor
#define trace(v, args...) NEST( if (TR_##v & opt.verbosity) { fprintf(cptout,args); fflush(cptout); } )
#define trace_proc(v, args...) NEST( if (TR_##v & opt.verbosity) { trace_##v(args); fflush(cptout); } )

#endif

extern void _begin_monitor(const char *s);
extern void _end_monitor(void);


#define ERR_allocation 100
#define ERR_bucket 101
#define ERR_no_plan 102
#define ERR_user_interruption 103
#define ERR_timer_interruption 104
#define ERR_no_file 105
#define ERR_parser 106
#define ERR_max_plan_length 107
#define ERR_options 108
#define ERR_station 109

#define error(errno, args...) NEST( trace(error, "\n\n"); trace(error, args); trace(error, "...\n\n"); exit(ERR_##errno); )


#endif /* #define TRACE_H */
