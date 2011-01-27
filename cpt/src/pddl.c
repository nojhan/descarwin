/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : pddl.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#include "cpt.h"
#include "trace.h"
#include "pddl.h"
#include "globs.h"


/*---------------------------------------------------------------------------*/
/* Local type definitions                                                    */
/*---------------------------------------------------------------------------*/


typedef struct Token Token;
  
struct Token {
  const char *name;
  int lineno;
  PDDLModality mod;
  gdsl_list_t sub;
};


/*---------------------------------------------------------------------------*/
/* Local Variables                                                           */
/*---------------------------------------------------------------------------*/


static int lost_int;

static PDDLPredicate predicate_eq;
static PDDLPredicate predicate_total_time;

static const char *sign_modality[] = {"" , "not" };
static const char *function_modality[] = {"" , "=", "<=", ">=", "<", ">", "increase", "decrease", "assign"};
static const char *expression_modality[] = {"", "+", "-", "*", "/", "&"};
static const char *mod_string[] = { "", "@middle", "@end"};


/*---------------------------------------------------------------------------*/
/* Static Functions                                                          */
/*---------------------------------------------------------------------------*/


static long term_name_cmp(PDDLTerm *term, const char *name);
static long terms_cmp(PDDLTerm *term1, PDDLTerm *term2);
static long predicate_name_cmp(PDDLPredicate *predicate, const char *name);
static long predicates_cmp(PDDLPredicate *predicate1, PDDLPredicate *predicate2);

static PDDLType *type_allocate(const char *name);
static int add_type_to_type_list(PDDLType *type, gdsl_location_t loc, PDDLType **types);
static void add_constant_to_types(PDDLTerm *constant, PDDLType **types, int types_nb);

static void parse_domain_requirements(PDDLDomain *domain);
static void parse_domain_types(PDDLDomain *domain); 
static void parse_domain_constants(PDDLDomain *domain, gdsl_list_t tokens, PDDLTerm ***terms, long *terms_nb); 
static void parse_domain_predicates(PDDLDomain *domain, gdsl_list_t tokens, PDDLPredicate ***predicates, long *predicates_nb);
static void parse_domain_operators(PDDLDomain *domain); 
static void parse_atom_list(PDDLDomain *domain, gdsl_list_t tokens, PDDLOperator *ope, PDDLLitteral ***litterals, long *litterals_nb); 
static void parse_expression(PDDLDomain *domain, gdsl_list_t tokens, PDDLOperator *ope, PDDLExpression **exp);
static void parse_number(mpq_t number, const char *src);
static void parse_atom(PDDLDomain *domain, gdsl_list_t tokens, PDDLOperator *ope, PDDLLitteral **litteral);
static void parse_term_list(PDDLDomain *domain, gdsl_list_t list, PDDLTerm ***terms, long *terms_nb);
static void parse_ac_constraints(PDDLDomain *domain, gdsl_list_t tokens, PDDLOperator *ope, PDDLActivityConstraint ***aconstraints, long *aconstraints_nb);
static void print_pddl_domain(FILE *out, PDDLDomain *domain); 
static void print_pddl_problem(FILE *out, PDDLDomain *domain); 
static void print_operator(FILE *out, PDDLOperator *ope);
static void print_types(FILE *out, PDDLType **types, long types_nb);
static void print_term_list(FILE *out, PDDLTerm **terms, long terms_nb, bool printtypes);
static void print_predicate(FILE *out, PDDLPredicate *predicate);
static void print_atom(FILE *out, PDDLAtom *atom);
static void print_expression(FILE *out, PDDLExpression *expr);
static void print_litteral(FILE *out, PDDLLitteral *litteral);
static void print_litteral_list(FILE *out, PDDLLitteral **litterals, long litterals_nb);
static void print_timed_operator(FILE *out, PDDLOperator *ope, TemporalModality mod, bool has_atend, bool has_overall);
static void print_timed_litteral_list(FILE *out, PDDLLitteral **litterals, long litterals_nb, TemporalModality mod);
static void print_ac_constraint(FILE *out, PDDLActivityConstraint *ac);


/*****************************************************************************/

/*  ******************** */
/*  *  Symbol helpers  * */
/*  ******************** */


char *symbol_insert(PDDLDomain *domain, const char *name)
{
  static char buffer[STRING_MAX];
  int i = 0;
  do buffer[i++] = tolower(*name); while (*name++);
  return (char *) gdsl_rbtree_insert(domain->symbols_table, buffer, &lost_int);
}


/*  ******************* */
/*  *  Token helpers  * */
/*  ******************* */


#define token_add(place, tokens, name, sub)				\
  NEST( if (!tokens) tokens = gdsl_list_alloc(NULL, NULL, free); \
	Token *token = cpt_calloc(token, 1); token->name = name; token->sub = sub; token->lineno = parser_get_lineno(); \
	gdsl_list_insert_##place(tokens, token);			\
	return tokens; )							

gdsl_list_t token_add_head(gdsl_list_t tokens, const char *name, gdsl_list_t sub)
{
  token_add(head, tokens, name, sub);
}

gdsl_list_t token_add_tail(gdsl_list_t tokens, const char *name, gdsl_list_t sub)
{
  token_add(tail, tokens, name, sub);
}

gdsl_list_t token_set_sub(gdsl_list_t tokens, gdsl_list_t sub)
{
  Token *token = (Token *) gdsl_list_get_head(tokens);
  token->sub = sub;
  return tokens;
}

gdsl_list_t token_set_sign_mod(gdsl_list_t tokens, SignModality mod)
{
  Token *token = (Token *) gdsl_list_get_head(tokens);
  token->mod.sign = mod;
  return tokens;
}

gdsl_list_t token_set_equality_mod(gdsl_list_t tokens, EqualityModality mod)
{
  Token *token = (Token *) gdsl_list_get_head(tokens);
  token->mod.equality = mod;
  return tokens;
}

gdsl_list_t token_set_temporal_mod(gdsl_list_t tokens, TemporalModality mod)
{
  Token *token = (Token *) gdsl_list_get_head(tokens);
  token->mod.temporal = mod;
  return tokens;
}

gdsl_list_t token_set_function_mod(gdsl_list_t tokens, FunctionModality mod)
{
  Token *token = (Token *) gdsl_list_get_head(tokens);
  token->mod.function = mod;
  if (!token->name) token->name = function_modality[mod];
  return tokens;
}

gdsl_list_t token_set_expression_mod(gdsl_list_t tokens, ExpressionModality mod)
{
  Token *token = (Token *) gdsl_list_get_head(tokens);
  token->mod.expression = mod;
  if (!token->name) token->name = expression_modality[mod];
  return tokens;
}


/*  ******************* */
/*  *  Types helpers  * */
/*  ******************* */


static PDDLType *type_allocate(const char *name)
{
  PDDLType *type = cpt_calloc(type, 1);
  type->name = name;
  type->constants_table = gdsl_rbtree_alloc(NULL, NULL, NULL, (gdsl_compare_func_t) terms_cmp);
  type->subtypes_table = gdsl_rbtree_alloc(NULL, NULL, NULL, (gdsl_compare_func_t) terms_cmp);
  return type;
}

static int add_type_to_type_list(PDDLType *type, gdsl_location_t loc, PDDLType **types)
{
  types[lost_int++] = type;
  return GDSL_MAP_CONT;
}

static void add_constant_to_types(PDDLTerm *constant, PDDLType **types, int types_nb)
{
  FOR(t, types) {
    gdsl_rbtree_insert(t->constants_table, constant, &lost_int);
    add_constant_to_types(constant, t->subtypes, t->subtypes_nb);
  } EFOR;
}	


/*  ******************************* */
/*  *  GDSL helpers (comparison)  * */
/*  ******************************* */


/* static long term_name_cmp(PDDLTerm *term, const char *name) */
/* { */
/*   return (long) term->name - (long) name; */
/* } */

/* static long terms_cmp(PDDLTerm *term1, PDDLTerm *term2) */
/* { */
/*   return (long) term1->name - (long) term2->name; */
/* } */

/* static long predicate_name_cmp(PDDLPredicate *predicate, const char *name) */
/* { */
/*   return (long) predicate->name - (long) name; */
/* } */

/* static long predicates_cmp(PDDLPredicate *predicate1, PDDLPredicate *predicate2) */
/* { */
/*   return (long) predicate1->name - (long) predicate2->name; */
/* } */

static long term_name_cmp(PDDLTerm *term, const char *name)
{
  return strcmp(term->name, name);
}

static long terms_cmp(PDDLTerm *term1, PDDLTerm *term2)
{
  return strcmp(term1->name, term2->name);
}

static long predicate_name_cmp(PDDLPredicate *predicate, const char *name)
{
  return strcmp(predicate->name,name);
}

static long predicates_cmp(PDDLPredicate *predicate1, PDDLPredicate *predicate2)
{
  return strcmp(predicate1->name, predicate2->name);
}


/*  ******************************** */
/*  *  Parsing domain and problem  * */
/*  ******************************** */

void domain_add_token_operator(PDDLDomain *domain, PDDLOperator *ope)
{
  gdsl_list_insert_tail(domain->token_operators, ope);
}

PDDLDomain *parse_domain(char *dom_name, char *prob_name)
{
  PDDLDomain *domain = cpt_calloc(domain, 1);
  int pipefd[2];

  domain->token_operators = gdsl_list_alloc(NULL, NULL, NULL);
  domain->symbols_table = gdsl_rbtree_alloc(NULL, (gdsl_alloc_func_t) strdup, NULL, (gdsl_compare_func_t) strcmp);
  domain->types_table = gdsl_rbtree_alloc(NULL, (gdsl_alloc_func_t) type_allocate, NULL, (gdsl_compare_func_t) term_name_cmp);
  domain->predicates_table = gdsl_rbtree_alloc(NULL, NULL, NULL, (gdsl_compare_func_t) predicates_cmp);
  domain->constants_table = gdsl_rbtree_alloc(NULL, NULL, NULL, (gdsl_compare_func_t) terms_cmp);

  if (opt.pddl21) {
    if (pipe(pipefd) != 0) error(no_file,"PDDL 2.1 pipe error");
    switch (fork()) {
    case 0: close(pipefd[0]); domain->print_domain_only = true; break;
    default: close(pipefd[1]); break;
    }
  }
  
  if (!domain->print_domain_only) begin_monitor("Parsing domain");

  parser_read_pddl_file(domain, opt.pddl21 && !domain->print_domain_only ? NULL : dom_name, pipefd[0]);

  parse_domain_requirements(domain);
  if (domain->equality) {
    predicate_eq.name = "=";
    predicate_eq.terms_nb = 2;
    gdsl_rbtree_insert(domain->predicates_table, &predicate_eq, &lost_int);
  }
  if (domain->durative_actions) {
    opt.sequential = false;
    predicate_total_time.name = symbol_insert(domain, "total-time"); 
    predicate_total_time.terms_nb = 0; 
    gdsl_rbtree_insert(domain->predicates_table, &predicate_total_time, &lost_int);
  } else {
    if (opt.yahsp) opt.sequential = true;
    if (opt.precision) cpt_free(opt.precision); 
    if (opt.precision2) cpt_free(opt.precision2); 
  }
  opt.fluent_mutexes = (!opt.yahsp || opt.dae);
  
  parse_domain_types(domain);
  parse_domain_constants(domain, domain->token_constants, &domain->constants_domain, &domain->constants_domain_nb);
  parse_domain_predicates(domain, domain->token_predicates, &domain->predicates, &domain->predicates_nb);
  parse_domain_predicates(domain, domain->token_functions, &domain->functions, &domain->functions_nb);
  parse_domain_operators(domain);
  if (opt.precision) parse_number(domain->precision.q, opt.precision);
  //print_pddl_domain(cptout, domain);
  if (domain->print_domain_only) { print_pddl_domain(fdopen(pipefd[1], "w"), domain); exit(0); }
  end_monitor();

  if (opt.print_actions) print_pddl_domain(cptout, domain);

  begin_monitor("Parsing problem");
  parser_read_pddl_file(domain, prob_name, 0);
  parse_domain_constants(domain, domain->token_objects, &domain->constants_problem, &domain->constants_problem_nb);
  parse_atom_list(domain, domain->token_init, NULL, &domain->init, &domain->init_nb);
  parse_atom_list(domain, domain->token_goal, NULL, &domain->goal, &domain->goal_nb);
  parse_expression(domain, domain->token_metric, NULL, &domain->metric);
  parse_ac_constraints(domain, domain->token_ac_constraints, NULL, &domain->ac_constraints, &domain->ac_constraints_nb);
  end_monitor();

  if (opt.print_actions) print_pddl_problem(cptout, domain); 

  cpt_malloc(domain->constants_all, domain->constants_domain_nb + domain->constants_problem_nb);
  domain->constants_all_nb = 0;
  FOR(c, domain->constants_domain) { domain->constants_all[domain->constants_all_nb++] = c; } EFOR;
  FOR(c, domain->constants_problem) { domain->constants_all[domain->constants_all_nb++] = c; } EFOR;

  parse_number(domain->bound.q, opt.bound);
  if (opt.max_makespan) parse_number(domain->max_makespan.q, opt.max_makespan);
  if (opt.precision2) parse_number(domain->precision2.q, opt.precision2);

  trace(normal, "domain : %s\n", domain->name);
  trace(normal, "problem : %s\n", domain->probname);
  return domain;
}

static void parse_domain_requirements(PDDLDomain *domain)
{
  Token *token;
  if (!domain->token_requirements) return;
  while ((token = (Token *) gdsl_list_remove_head(domain->token_requirements))) {
    const char *feature = token->name;
    if (strcmp(feature, ":strips") == 0) domain->strips = true;
    else if (strcmp(feature, ":equality") == 0) domain->equality = true;
    else if (strcmp(feature, ":durative-actions") == 0) domain->durative_actions = true;
    else if (strcmp(feature, ":typing") == 0) domain->typing = true;
    else if (strcmp(feature, ":fluents") == 0) domain->numerical_fluents = true;
    else if (strcmp(feature, ":action-costs") == 0) domain->action_costs = true;
    cpt_free(token);
  }
  gdsl_list_free(domain->token_requirements);
}

static void parse_domain_types(PDDLDomain *domain) 
{
  if (!domain->token_types) return;
  Token *token;
  PDDLType *tmp[gdsl_list_get_size(domain->token_types)];
  long tmp_nb = 0;
  while ((token = (Token *) gdsl_list_remove_head(domain->token_types))) {
    PDDLType *type = (PDDLType *) gdsl_rbtree_insert(domain->types_table, (void *) token->name, &lost_int);
    tmp[tmp_nb++] = type;
    if (token->sub) {
      Token *token2;
      while ((token2 = (Token *) gdsl_list_remove_head(token->sub))) {
	PDDLType *subtype = (PDDLType *) gdsl_rbtree_insert(domain->types_table, (void *) token2->name, &lost_int);
	FOR(t, tmp) { gdsl_rbtree_insert(t->subtypes_table, subtype, &lost_int); } EFOR;
	cpt_free(token2);
      }
      gdsl_list_free(token->sub);
      tmp_nb = 0;
    }
    cpt_free(token);
  }
  gdsl_list_free(domain->token_types);

  cpt_malloc(domain->types, (domain->types_nb = gdsl_rbtree_get_size(domain->types_table)));
  lost_int = 0;
  gdsl_rbtree_map_prefix(domain->types_table, (gdsl_map_func_t) add_type_to_type_list, (void *) domain->types);

  FOR(t, domain->types) {
    if (gdsl_rbtree_get_size(t->subtypes_table) != 0) {
      cpt_malloc(t->subtypes, (t->subtypes_nb = gdsl_rbtree_get_size(t->subtypes_table)));
      lost_int = 0;
      gdsl_rbtree_map_prefix(t->subtypes_table, (gdsl_map_func_t) add_type_to_type_list, (void *) t->subtypes);
    }
  } EFOR;
  
  long src = 0, dest = domain->types_nb - 1;
  while (src < dest) {
    if (!domain->types[src]->subtypes) {
      PDDLType *tmp = domain->types[src];
      domain->types[src] = domain->types[dest];
      domain->types[dest--] = tmp;
    } else src++;
  }
}
	
static void parse_domain_constants(PDDLDomain *domain, gdsl_list_t tokens, PDDLTerm ***terms, long *terms_nb) 
{
  if (!tokens) return;
  parse_term_list(domain, tokens, terms, terms_nb);
  FOR(constant, *terms) {
    gdsl_rbtree_insert(domain->constants_table, constant, &lost_int);
    add_constant_to_types(constant, constant->types, constant->types_nb);
  } EFOR;
}

static void parse_domain_predicates(PDDLDomain *domain, gdsl_list_t tokens, PDDLPredicate ***predicates, long *predicates_nb) 
{
  Token *token_init;
  if (!tokens) return;
  cpt_malloc(*predicates, gdsl_list_get_size(tokens));
  while ((token_init = (Token *) gdsl_list_remove_head(tokens))) {
    Token *token = (Token *) gdsl_list_remove_head(token_init->sub);
    PDDLPredicate *predicate = cpt_calloc((*predicates)[(*predicates_nb)++], 1);
    predicate->name = token->name;
    gdsl_rbtree_insert(domain->predicates_table, predicate, &lost_int);
    parse_term_list(domain, token_init->sub, &predicate->terms, &predicate->terms_nb);
    cpt_free(token);
    cpt_free(token_init);
  }
  gdsl_list_free(tokens);
}

static void parse_ac_constraints(PDDLDomain *domain, gdsl_list_t tokens, PDDLOperator *ope, PDDLActivityConstraint ***aconstraints, long *aconstraints_nb) 
{
  Token *token;
  if (!tokens) return;
  cpt_malloc(*aconstraints, gdsl_list_get_size(tokens) / 2);
  while ((token = (Token *) gdsl_list_remove_head(tokens))) {
    Token *token_expr = (Token *) gdsl_list_remove_head(tokens);
    PDDLActivityConstraint *ac = cpt_calloc((*aconstraints)[(*aconstraints_nb)++], 1); 
    parse_atom(domain, token->sub, ope, &ac->litteral);
    cpt_free(token);
    token =  (Token *) gdsl_list_remove_head(token_expr->sub);
    parse_expression(domain, token->sub, ope, &ac->min);
    cpt_free(token);
    token = (Token *) gdsl_list_remove_head(token_expr->sub);
    parse_expression(domain, token->sub, ope, &ac->max);
    cpt_free(token);
    token = (Token *) gdsl_list_remove_head(token_expr->sub);
    if (token) parse_expression(domain, token->sub, ope, &ac->inter);
    cpt_free(token);
    gdsl_list_free(token_expr->sub);
    cpt_free(token_expr);
  }
  gdsl_list_free(tokens);
}

static void parse_domain_operators(PDDLDomain *domain) 
{
  PDDLOperator *ope;
  cpt_malloc(domain->operators, gdsl_list_get_size(domain->token_operators) + 2);
  domain->operators += 2;
  while ((ope = (PDDLOperator *) gdsl_list_remove_head(domain->token_operators))) {
    domain->operators[domain->operators_nb++] = ope;
    parse_term_list(domain, ope->token_parameters, &ope->parameters, &ope->parameters_nb);
    FORi(v, i, ope->parameters) { v->index = i; } EFOR;
    parse_expression(domain, ope->token_duration, ope, &ope->duration);
    parse_expression(domain, ope->token_real_duration, ope, &ope->real_duration);
    parse_atom_list(domain, ope->token_precondition, ope, &ope->precondition, &ope->precondition_nb);
    parse_atom_list(domain, ope->token_effect, ope, &ope->effect, &ope->effect_nb);
    parse_ac_constraints(domain, ope->token_ac_constraints, ope, &ope->ac_constraints, &ope->ac_constraints_nb);
  }
  gdsl_list_free(domain->token_operators);
}

static void parse_expression(PDDLDomain *domain, gdsl_list_t tokens, PDDLOperator *ope, PDDLExpression **exp)
{
  if (!tokens) return;
  PDDLExpression *expr = cpt_calloc(*exp, 1);
  Token *token = (Token *) gdsl_list_get_head(tokens);
  expr->mod = token->mod.expression;
  switch (expr->mod) {
  case NUM_MOD: {
    parse_number(expr->number, token->name);
    gdsl_list_free(tokens);
    break;
  }
  case ATOM_MOD: 
    parse_atom(domain, tokens, ope, &expr->litteral);
    break;
  default: {
    cpt_malloc(expr->terms, gdsl_list_get_size(tokens));
    while ((token = (Token *) gdsl_list_remove_head(tokens))) {
      parse_expression(domain, token->sub, ope, expr->terms + expr->terms_nb++);
      cpt_free(token);
    }
    gdsl_list_free(tokens);
  }
  }
}

static void parse_number(mpq_t number, const char *src)
{
  char buffer[strlen(src) * 2 + 1];
  char *dst = buffer;
  long nb = 0;
  while (*src && *src != '.') *dst++ = *src++;
  if (*src) src++;
  while (*src) { *dst++ = *src++; nb++; }
  if (nb > 0) {
    *dst++ = '/';
    *dst++ = '1';
    while (nb-- > 0) *dst++ = '0';
  }
  *dst = 0;
  mpq_init(number);
  mpq_set_str(number, buffer, 10);
  mpq_canonicalize(number);
}

static void parse_atom(PDDLDomain *domain, gdsl_list_t tokens, PDDLOperator *ope, PDDLLitteral **litteral) 
{
  cpt_calloc(*litteral, 1);
  PDDLAtom *atom = cpt_calloc((*litteral)->atom, 1);
  Token *token = (Token *) gdsl_list_remove_head(tokens);
  if (!(atom->predicate = (PDDLPredicate *) gdsl_rbtree_search(domain->predicates_table, (gdsl_compare_func_t) predicate_name_cmp, (void *) token->name)))
    error(parser, "Undefined predicate %s at line %d", token->name, token->lineno);
  (*litteral)->mod = token->mod;
  parse_expression(domain, token->sub, ope, &(*litteral)->value);
  cpt_free(token);
  PDDLTerm **terms = cpt_calloc(atom->terms, (atom->terms_nb = gdsl_list_get_size(tokens)));
  while ((token = (Token *) gdsl_list_remove_head(tokens))) {
    if (token->name[0] == '?') {
      FOR(v, ope->parameters) { if (v->name == token->name) { *terms = v; break; } } EFOR;
    } else *terms = (PDDLTerm *) gdsl_rbtree_search(domain->constants_table, (gdsl_compare_func_t) term_name_cmp, (void *) token->name);
    if (!*terms++) error(parser, "Undefined term %s at line %d", token->name, token->lineno);
    cpt_free(token);
  }
  gdsl_list_free(tokens);
}

static void parse_atom_list(PDDLDomain *domain, gdsl_list_t tokens, PDDLOperator *ope, PDDLLitteral ***litterals, long *litterals_nb) 
{
  Token *token;
  if (!tokens) return;
  cpt_calloc(*litterals, gdsl_list_get_size(tokens));
  while ((token = (Token *) gdsl_list_remove_head(tokens))) {
    parse_atom(domain, token->sub, ope, *litterals + (*litterals_nb)++);
    cpt_free(token);
  }
  gdsl_list_free(tokens);
}

static void parse_term_list(PDDLDomain *domain, gdsl_list_t tokens, PDDLTerm ***terms, long *terms_nb)
{
  long i;
  Token *token;
  if (!tokens) return;
  cpt_calloc(*terms, gdsl_list_get_size(tokens));
  while ((token = (Token *) gdsl_list_remove_head(tokens))) {
    PDDLTerm *term = cpt_calloc((*terms)[*terms_nb], 1);
    term->name = token->name;
    term->is_var = (token->name[0] == '?');
    if (token->sub) {
      Token *token2;
      cpt_calloc(term->types, gdsl_list_get_size(token->sub));
      while ((token2 = (Token *) gdsl_list_remove_head(token->sub))) {
	if (!(term->types[term->types_nb++] = 
	      (PDDLType *) gdsl_rbtree_search(domain->types_table, (gdsl_compare_func_t) term_name_cmp, (void *) token2->name)))
	  error(parser, "Undefined type %s at line %d", token2->name, token2->lineno);
	cpt_free(token2);
      }
      gdsl_list_free(token->sub);
      for (i = *terms_nb - 1; i >= 0 && !(*terms)[i]->types; i--) {
	(*terms)[i]->types = term->types;
	(*terms)[i]->types_nb = term->types_nb;
      }
    }
    (*terms_nb)++;
    cpt_free(token);
  }
  gdsl_list_free(tokens);
}


/*  ********************************* */
/*  *  Printing domain and problem  * */
/*  ********************************* */


char *make_name(const char *name, PDDLTerm **terms, long terms_nb)
{
  static char buffer[STRING_MAX];
  long i = 1;
  buffer[0] = '(';
  while (*name != '\0') buffer[i++] = *name++;
  FOR(term, terms) {
    name = term->name;
    buffer[i++] = ' ';
    while (*name != '\0') buffer[i++] = *name++;
  } EFOR;
  buffer[i++] = ')';
  buffer[i++] = '\0';
  return buffer;
}

static void print_pddl_domain(FILE *out, PDDLDomain *domain) 
{
  fprintf(out, "(define (domain %s)\n\n", domain->name);
  fprintf(out, "(:requirements");
  if (domain->strips) fprintf(out, " :strips");
  if (domain->equality) fprintf(out, " :equality");
  if (domain->durative_actions) fprintf(out, " :durative-actions");
  if (domain->typing) fprintf(out, " :typing");
  if (domain->numerical_fluents) fprintf(out, " :fluents");
  fprintf(out, ")\n\n");
  if (domain->types) {
    fprintf(out, "(:types");
    FORMIN(type, domain->types, 0) {
      fprintf(out, "\n  %s", type->name);
      print_types(out, type->subtypes, type->subtypes_nb);
    } EFOR;
    fprintf(out, "\n)\n\n");
  }
  if (domain->constants_domain) {
    fprintf(out, "(:constants\n");
    FOR(type, domain->constants_domain) {
      fprintf(out, "\n  %s", type->name);
      print_types(out, type->types, type->types_nb);
    } EFOR;
    fprintf(out, ")\n\n");
  }
  if (domain->predicates) {
    fprintf(out, "(:predicates\n");
    FOR(predicate, domain->predicates) {
      fprintf(out, "  ");
      print_predicate(out, predicate);
      fprintf(out, "\n");
    } EFOR;
    if (domain->print_domain_only) {
      FOR(ope, domain->operators) {
	fprintf(out, "  (trigger-%s%s ", ope->name, mod_string[ATSTART_MOD]);
	print_term_list(out, ope->parameters, ope->parameters_nb, false);
	fprintf(out, ")\n");
	fprintf(out, "  (trigger-%s%s ", ope->name, mod_string[OVERALL_MOD]);
	print_term_list(out, ope->parameters, ope->parameters_nb, false);
	fprintf(out, ")\n");
      } EFOR;
    }
    fprintf(out, ")\n\n");
  }
  if (domain->functions) {
    fprintf(out, "(:functions\n");
    FOR(function, domain->functions) {
      fprintf(out, "  ");
      print_predicate(out, function);
      fprintf(out, "\n");
    } EFOR;
    fprintf(out, ")\n\n");
  }
  FOR(ope, domain->operators) {
    if (!domain->print_domain_only) print_operator(out, ope);
    else {
      bool has_atend = false;
      bool has_overall = false;
      FOR(e, ope->precondition) { if (e->mod.temporal == OVERALL_MOD) { has_overall = true; break; } } EFOR;
      FOR(e, ope->effect) { if (e->mod.temporal == ATEND_MOD) { has_atend = true; break; } } EFOR;
      print_timed_operator(out, ope, ATSTART_MOD, has_atend, has_overall);
      if (has_overall) print_timed_operator(out, ope, OVERALL_MOD, has_atend, has_overall);
      if (has_atend) print_timed_operator(out, ope, ATEND_MOD, has_atend, has_overall);
    }
    fprintf(out, "\n");
  } EFOR;
  fprintf(out, ")\n");
}

static void print_pddl_problem(FILE *out, PDDLDomain *domain) 
{
  fprintf(out, "(define (problem %s)\n", domain->probname);
  fprintf(out, "(:domain %s)\n", domain->name);
  if (domain->constants_problem) {
    fprintf(out, "(:objects\n");
    FOR(type, domain->constants_problem) {
      fprintf(out, "  %s\n", type->name);
      print_types(out, type->types, type->types_nb);
    } EFOR;
    fprintf(out, ")\n");
  }
  fprintf(out, "(:init\n");
  print_litteral_list(out, domain->init, domain->init_nb);
  fprintf(out, ")\n");
  fprintf(out, "(:goal (and\n");
  print_litteral_list(out, domain->goal, domain->goal_nb);
  fprintf(out, "))\n");
  if (domain->ac_constraints) {
    fprintf(out, "(:constraints (and\n");
    FOR(ac, domain->ac_constraints) {
      print_ac_constraint(out, ac);
    } EFOR;
    fprintf(out, "))\n");
  }
  if (domain->metric) {
    fprintf(out, "(:metric %s ", domain->metric_function);
    print_expression(out, domain->metric);
    fprintf(out, ")\n");
  }
  fprintf(out, ")\n");
}

static void print_operator(FILE *out, PDDLOperator *ope)
{
  fprintf(out, "(:action %s\n", ope->name);
  fprintf(out, " :parameters (");
  print_term_list(out, ope->parameters, ope->parameters_nb, true);
  fprintf(out, ")\n");
  if (ope->duration) {
    fprintf(out, " :duration (= ?duration ");
    print_expression(out, ope->duration);
    fprintf(out, ")\n");
  }
  if (ope->precondition) {
    fprintf(out, " :precondition (and\n");
    print_litteral_list(out, ope->precondition, ope->precondition_nb);
    fprintf(out, " )\n");
  }
  if (ope->effect) {
    fprintf(out, " :effect (and\n");
    print_litteral_list(out, ope->effect, ope->effect_nb);
    fprintf(out, " )\n");
  }
  if (ope->ac_constraints) {
    fprintf(out, " :constraints (and\n");
    FOR(ac, ope->ac_constraints) {
      print_ac_constraint(out, ac);
    } EFOR;
    fprintf(out, " )\n");
  }
  fprintf(out, ")\n");
}

void print_pddl_types(FILE *out, PDDLDomain *domain)
{
  FOR(type, domain->types) {
    fprintf(out, "TYPE %s :\n", type->name);
    FOR(constant, type->constants) {
      fprintf(out, "  %s\n", constant->name);
    } EFOR;
    fprintf(out, "\n");
  } EFOR;
}

static void print_types(FILE *out, PDDLType **types, long types_nb)
{
  if (types) {
    fprintf(out, " -");
    if (types_nb > 1) fprintf(out, " (either");
    FOR(subtype, types) {
      fprintf(out, " %s", subtype->name);
    } EFOR;
    if (types_nb > 1) fprintf(out, ")");
  }
}

static void print_term_list(FILE *out, PDDLTerm **terms, long terms_nb, bool printtypes)
{
  FORi(term, i, terms) {
    fprintf(out, "%s", term->name);
    if (printtypes) print_types(out, term->types, term->types_nb);
    if (i < terms_nb - 1) fprintf(out, " ");
  } EFOR;
 }

static void print_predicate(FILE *out, PDDLPredicate *predicate)
{
  fprintf(out, "(%s", predicate->name);
  if (predicate->terms_nb > 0) fprintf(out, " ");
  print_term_list(out, predicate->terms, predicate->terms_nb, true);
  fprintf(out, ")");
}

static void print_atom(FILE *out, PDDLAtom *atom)
{
  fprintf(out, "(%s", atom->predicate->name);
  if (atom->predicate->terms_nb > 0) fprintf(out, " ");
  print_term_list(out, atom->terms, atom->terms_nb, false);
  fprintf(out, ")");
}

static void print_expression(FILE *out, PDDLExpression *expr)
{
  switch (expr->mod) {
  case NUM_MOD: {
    bool isint = mpz_cmp_si(mpq_denref(expr->number), 1) == 0;
    if (!isint) fprintf(out, "(/ ");
    mpz_out_str(out, 10, mpq_numref(expr->number));
    if (isint) break;
    fprintf(out, " ");
    mpz_out_str(out, 10, mpq_denref(expr->number));
    fprintf(out, ")");
    break;
  }
  case ATOM_MOD:
    print_atom(out, expr->litteral->atom);
    break;
  default:
    fprintf(out, "(%s", expression_modality[expr->mod]);
    FOR(term, expr->terms) {
      fprintf(out, " ");
      print_expression(out, term);
    } EFOR;
    fprintf(out, ")");
    break;
  }
}

static void print_litteral(FILE *out, PDDLLitteral *litteral)
{
  PDDLModality mod = litteral->mod;
  if (mod.function != NO_FUNC_MOD) fprintf(out, "(%s ", function_modality[mod.function]);
  if (mod.sign == NEG_MOD) fprintf(out, "(%s ", sign_modality[mod.sign]);
  print_atom(out, litteral->atom);
  if (mod.sign == NEG_MOD) fprintf(out, ")");
  if (mod.function != NO_FUNC_MOD) {
    fprintf(out, " ");
    print_expression(out, litteral->value);
    fprintf(out, ")");
  }
}

static void print_litteral_list(FILE *out, PDDLLitteral **litterals, long litterals_nb)
{
  FOR(litteral, litterals) {
    fprintf(out, "  ");
    print_litteral(out, litteral);
    fprintf(out, "\n");
  } EFOR;
}

static void print_duration_minus_epsilon(FILE *out, PDDLOperator *ope)
{
  fprintf(out,"(- ");
  print_expression(out, ope->duration);
  fprintf(out," %s)", opt.precision);
}

static void print_timed_operator(FILE *out, PDDLOperator *ope, TemporalModality mod, bool has_atend, bool has_overall)
{
  const char *string_next = mod_string[mod != ATSTART_MOD || has_overall ? mod : mod + 1];
  //bool trigger_next = mod == ATSTART_MOD || (mod == OVERALL_MOD && has_atend);
  bool trigger_next = (mod == ATSTART_MOD && (has_overall || has_atend)) || (mod == OVERALL_MOD && has_atend);

  fprintf(out, "(:action %s%s\n", ope->name, mod_string[mod - 1]);
  fprintf(out, " :parameters (");
  print_term_list(out, ope->parameters, ope->parameters_nb, true);
  fprintf(out, ")\n");
  if (ope->duration) {
    fprintf(out, " :duration (= ?duration ");
    switch (mod) {
    case ATSTART_MOD: fprintf(out, "%s", opt.precision); break;
    case ATEND_MOD: fprintf(out, "%s", opt.precision); break;
    case OVERALL_MOD: print_duration_minus_epsilon(out, ope); break;
    case NO_TEMP_MOD: print_expression(out, ope->duration); break;
    }
    fprintf(out, ")\n");
  }
  if (mod == ATSTART_MOD) {
    fprintf(out, " :real-duration ");
    print_expression(out, ope->duration);
    fprintf(out, "\n");
  }
  if (ope->precondition || mod > ATSTART_MOD) {
    fprintf(out, " :precondition (and\n");
    print_timed_litteral_list(out, ope->precondition, ope->precondition_nb, mod);
    if (mod > ATSTART_MOD) {
      fprintf(out, "  (trigger-%s%s ", ope->name, mod_string[mod - 1]);
      print_term_list(out, ope->parameters, ope->parameters_nb, false);
      fprintf(out, ")\n");
    }
    fprintf(out, " )\n");
  }
  if (ope->effect || mod < ATEND_MOD) {
    fprintf(out, " :effect (and\n");
    print_timed_litteral_list(out, ope->effect, ope->effect_nb, mod);
    if (trigger_next) {
      fprintf(out, "  (trigger-%s%s ", ope->name, string_next);
      print_term_list(out, ope->parameters, ope->parameters_nb, false);
      fprintf(out, ")\n");
    }
    if (mod > ATSTART_MOD) {
      fprintf(out, "  (not (trigger-%s%s ", ope->name, mod_string[mod - 1]);
      print_term_list(out, ope->parameters, ope->parameters_nb, false);
      fprintf(out, "))\n");
    }
    fprintf(out, " )\n");
  }
  if (trigger_next || ope->ac_constraints) {
    fprintf(out, " :constraints (and\n");
    if (trigger_next) {
      fprintf(out, "  (active (trigger-%s%s ", ope->name, string_next);
      print_term_list(out, ope->parameters, ope->parameters_nb, false);
      fprintf(out, ") ");
      if (mod == ATSTART_MOD && !has_overall) {
	print_duration_minus_epsilon(out, ope);
	fprintf(out, " ");
	print_duration_minus_epsilon(out, ope);
      } else fprintf(out, "0 0");
      fprintf(out, ")\n");
    }
    FOR(a, ope->ac_constraints) {
      if (a->litteral->mod.temporal == mod) print_ac_constraint(out, a);
    } EFOR;
    fprintf(out, " )\n");
  }
  fprintf(out, ")\n");
}

static void print_timed_litteral_list(FILE *out, PDDLLitteral **litterals, long litterals_nb, TemporalModality mod)
{
  FOR(litteral, litterals) {
    if (litteral->mod.temporal == NO_TEMP_MOD || mod == litteral->mod.temporal) {
      fprintf(out, "  ");
      print_litteral(out, litteral);
      fprintf(out, "\n");
    }
  } EFOR;
}

static void print_ac_constraint(FILE *out, PDDLActivityConstraint *ac)
{
  fprintf(out, "  (active ");
  print_litteral(out, ac->litteral); 
  fprintf(out, " ");
  print_expression(out, ac->min); 
  fprintf(out, " ");
  print_expression(out, ac->max); 
  if (ac->inter) {
    fprintf(out, " ");
    print_expression(out, ac->inter); 
  }
  fprintf(out, ")\n");
}
