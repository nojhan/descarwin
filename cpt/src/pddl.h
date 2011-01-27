/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : pddl.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */

#ifndef PDDL_H
#define PDDL_H 


#ifndef __WIN32__
#define WITHOUT_GDSL_TYPES
#endif
#include "gdsl/include/gdsl.h"


typedef enum {POS_MOD, NEG_MOD} SignModality;
typedef enum {NO_EQUAL_MOD, EQUAL_MOD} EqualityModality;
typedef enum {NO_TEMP_MOD, ATSTART_MOD, OVERALL_MOD, ATEND_MOD} TemporalModality;
typedef enum { NO_EXP_MOD, ADD_MOD, SUB_MOD, MUL_MOD, DIV_MOD, 
	       FAM_MOD, ATOM_MOD, NUM_MOD} ExpressionModality;
typedef enum { NO_FUNC_MOD, EQ_MOD, LTE_MOD, GTE_MOD, LT_MOD, GT_MOD,
	       INC_MOD, DEC_MOD, ASS_MOD} FunctionModality;

typedef struct PDDLDomain PDDLDomain;
typedef struct PDDLProblem PDDLProblem;
typedef struct PDDLOperator PDDLOperator;
typedef struct PDDLLitteral PDDLLitteral;
typedef struct PDDLModality PDDLModality;
typedef struct PDDLAtom PDDLAtom;
typedef struct PDDLExpression PDDLExpression;
typedef struct PDDLActivityConstraint PDDLActivityConstraint ;
typedef struct PDDLType PDDLType;
typedef struct PDDLPredicate PDDLPredicate;
typedef struct PDDLTerm PDDLTerm;

typedef struct TimeStruct TimeStruct;
typedef struct Fluent Fluent;
typedef struct Action Action;
typedef struct Resource Resource;

struct PDDLDomain {
  const char *name;
  const char *probname;
  bool print_domain_only;
  bool strips;
  bool equality;
  bool durative_actions;
  bool typing;
  bool numerical_fluents;
  bool action_costs;
  gdsl_rbtree_t symbols_table;
  gdsl_rbtree_t types_table;
  gdsl_rbtree_t predicates_table;
  gdsl_rbtree_t constants_table;
  gdsl_rbtree_t fluents_table;
  gdsl_rbtree_t resources_table;
  gdsl_rbtree_t constraints_table;
  gdsl_rbtree_t functions_init_table;
  gdsl_rbtree_t action_names;
  VECTOR(PDDLType *, types);
  VECTOR(PDDLOperator *, operators);
  VECTOR(PDDLTerm *, constants_domain);
  VECTOR(PDDLTerm *, constants_problem);
  VECTOR(PDDLTerm *, constants_all);
  VECTOR(PDDLPredicate *, predicates);
  VECTOR(PDDLPredicate *, functions);
  VECTOR(PDDLLitteral *, init);
  VECTOR(PDDLLitteral *, goal);
  VECTOR(PDDLActivityConstraint *, ac_constraints);
  PDDLExpression *metric;
  const char *metric_function;
  gdsl_queue_t actions_queue;
  gdsl_queue_t durations_queue;
  gdsl_queue_t fluents_queue;
  gdsl_queue_t resources_queue;
  VECTOR(Fluent *, fluents);
  VECTOR(Action *, actions);
  VECTOR(Resource *, resources);
  TimeStruct precision;
  TimeStruct precision2;
  mpz_t time_lcm_rat;
  mpz_t time_gcd_rat;
  TimeVal time_lcm;
  TimeVal time_gcd;
  TimeStruct bound;
  TimeStruct max_makespan;

  
  gdsl_list_t token_requirements;
  gdsl_list_t token_types;
  gdsl_list_t token_constants;
  gdsl_list_t token_predicates;
  gdsl_list_t token_functions;
  gdsl_list_t token_operators;
  gdsl_list_t token_objects;
  gdsl_list_t token_init;
  gdsl_list_t token_goal;
  gdsl_list_t token_metric;
  gdsl_list_t token_ac_constraints;
};

struct PDDLActivityConstraint {
  PDDLLitteral *litteral;
  PDDLExpression *min;
  PDDLExpression *max;
  PDDLExpression *inter;
};

struct PDDLOperator {
  const char *name;
  PDDLExpression *duration;
  PDDLExpression *real_duration;
  VECTOR(PDDLTerm *, parameters);
  VECTOR(PDDLLitteral *, precondition);
  VECTOR(PDDLLitteral *, effect);
  PDDLLitteral ***constraints;
  long *constraints_nb;
  long prec_nb;
  long add_nb;
  long del_nb;
  VECTOR(PDDLActivityConstraint *, ac_constraints);
  long resources_nb;
  long inf_needed_nb;
  long sup_needed_nb;
  long increased_nb;
  long decreased_nb;
  long assigned_nb;
  

  gdsl_list_t token_parameters;
  gdsl_list_t token_duration;
  gdsl_list_t token_real_duration;
  gdsl_list_t token_precondition;
  gdsl_list_t token_effect;
  gdsl_list_t token_ac_constraints;
};

struct PDDLModality {
  SignModality sign:1;
  EqualityModality equality:1;
  TemporalModality temporal:2;
  ExpressionModality expression:3;
  FunctionModality function:4;
};

struct PDDLLitteral {
  PDDLAtom *atom;
  PDDLModality mod;
  PDDLExpression *value;
  long max_index;
};

struct PDDLAtom {
  PDDLPredicate *predicate;
  VECTOR(PDDLTerm *, terms);
};

struct PDDLExpression {
  ExpressionModality mod;
  union {
    mpq_t number;
    PDDLLitteral *litteral;
    struct {
      VECTOR(PDDLExpression *, terms);
    };
  };
  VECTOR(TimeStruct, time);
  VECTOR(TimeStruct, dur);
};

struct PDDLType {
  const char *name;
  VECTOR(PDDLType *, subtypes);
  VECTOR(PDDLTerm *, constants);
  gdsl_rbtree_t subtypes_table;
  gdsl_rbtree_t constants_table;
};

struct PDDLPredicate {
  const char *name;
  long num;
  struct {
    bool in_effect:1;
    bool in_effect2:1;
  };
  VECTOR(PDDLTerm *, terms);
};

struct PDDLTerm {
  const char *name;
  bool is_var;
  long index;
  VECTOR(PDDLType *, types);
  bool flag;
};


extern void parser_read_pddl_file(PDDLDomain *domain, char *file, int pipefd);
extern int parser_get_lineno(void);
extern void parser_reset_lineno(void);

extern char *symbol_insert(PDDLDomain *domain, const char *name);
extern gdsl_list_t token_add_head(gdsl_list_t tokens, const char *name, gdsl_list_t sub);
extern gdsl_list_t token_add_tail(gdsl_list_t tokens, const char *name, gdsl_list_t sub);
extern gdsl_list_t token_set_sub(gdsl_list_t tokens, gdsl_list_t sub);
extern gdsl_list_t token_set_sign_mod(gdsl_list_t tokens, SignModality mod);
extern gdsl_list_t token_set_equality_mod(gdsl_list_t tokens, EqualityModality mod);
extern gdsl_list_t token_set_temporal_mod(gdsl_list_t tokens, TemporalModality mod);
extern gdsl_list_t token_set_function_mod(gdsl_list_t tokens, FunctionModality mod);
extern gdsl_list_t token_set_expression_mod(gdsl_list_t tokens, ExpressionModality mod);
extern void domain_add_token_operator(PDDLDomain *domain, PDDLOperator *ope);
extern PDDLDomain *parse_domain(char *dom_name, char *prob_name);

extern char *make_name(const char *first_name, PDDLTerm **terms, long terms_nb);
extern void print_pddl_types(FILE *output, PDDLDomain *domain);


#endif /* PDDL_H  */
