/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : instantiation.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#include "cpt.h"
#include "trace.h"
#include "structs.h"
#include "problem.h"
#include "globs.h"


/*---------------------------------------------------------------------------*/
/* Local Variables                                                           */
/*---------------------------------------------------------------------------*/


static int lost_int;


/*---------------------------------------------------------------------------*/
/* Static Functions                                                          */
/*---------------------------------------------------------------------------*/


static Fluent *fluent_allocate(PDDLAtom *atom);
static Fluent *instantiate_atom(PDDLDomain *domain, PDDLAtom *atom, PDDLTerm **params);
static long atoms_cmp(PDDLAtom *atom1, PDDLAtom *atom2);
static long litterals_cmp(PDDLLitteral *litteral1, PDDLLitteral *litteral2);
static long litteral_atom_cmp(PDDLLitteral *litteral, PDDLAtom *atom);
static long fluent_atom_cmp(Fluent *fluent, PDDLAtom *atom);
#ifdef RESOURCES
static long resource_atom_cmp(Resource *resource, PDDLAtom *atom);
#endif
static bool verify_constraint(PDDLDomain *domain, PDDLLitteral *litteral, PDDLTerm **params);
static PDDLExpression *evaluate_function(PDDLDomain *domain, PDDLLitteral *litteral, PDDLTerm **params);
static bool evaluate_expression(PDDLDomain *domain, mpq_t res, PDDLExpression *expr, PDDLTerm **params);
static void instantiate_operator(PDDLDomain *domain, PDDLOperator *ope, PDDLTerm **params, long params_nb, long index);
static int add_constant_to_domain(PDDLTerm *term, gdsl_location_t loc, PDDLTerm **terms);
static int compute_ppcm_and_apply_precision(const gdsl_element_t e, gdsl_location_t location, void *user_data);
static int compute_pgcd(const gdsl_element_t e, gdsl_location_t location, void *user_data);
static int compute_duration(const gdsl_element_t e, gdsl_location_t location, void *user_data);
static void normalize_durations(PDDLDomain *domain);


/*---------------------------------------------------------------------------*/
/* Local Macros                                                              */
/*---------------------------------------------------------------------------*/

#define set_atomic(litteral) (litteral->atom->predicate->in_effect = true)
#define atomic(litteral) (litteral->atom->predicate->in_effect)
#define set_constraint(litteral) (litteral->atom->predicate->in_effect2 = true);
#define is_constraint(litteral) (litteral->mod.function == NO_FUNC_MOD && (!atomic(litteral) || litteral->atom->predicate->in_effect2))
#define function(litteral) (litteral->mod.function != NO_FUNC_MOD)

/*****************************************************************************/


/*  ********************* */
/*  *  Fluents helpers  * */
/*  ********************* */


static Fluent *fluent_allocate(PDDLAtom *atom)
{
  Fluent *fluent = cpt_calloc(fluent, 1);
  fluent->atom = atom;
  return fluent;
}

#ifdef RESOURCES
static Resource *resource_allocate(PDDLAtom *atom)
{
  Resource *resource = cpt_calloc(resource, 1);
  resource->atom = atom;
  return resource;
}
#endif

Fluent *search_fluent(PDDLDomain *domain, PDDLPredicate *predicate, PDDLTerm **params)
{
  PDDLAtom a = { predicate, params, predicate->terms_nb };
  return (Fluent *) gdsl_rbtree_search(domain->fluents_table, (gdsl_compare_func_t) fluent_atom_cmp, &a);
}

static Fluent *instantiate_atom(PDDLDomain *domain, PDDLAtom *atom, PDDLTerm **params)
{
  PDDLAtom *atomi = cpt_calloc(atomi, 1);
  atomi->predicate = atom->predicate;
  cpt_malloc(atomi->terms, (atomi->terms_nb = atom->terms_nb));
  FORi(term, i, atom->terms) { atomi->terms[i] = (term->is_var ? params[term->index] : term); } EFOR;
  Fluent *fluent = (Fluent *) gdsl_rbtree_insert(domain->fluents_table, atomi, &lost_int);
  if (atomi != fluent->atom) { cpt_free(atomi->terms); cpt_free(atomi); } 
  else gdsl_queue_insert(domain->fluents_queue, fluent);
  return fluent;
}

#ifdef RESOURCES
Resource *search_resource(PDDLDomain *domain, PDDLPredicate *predicate, PDDLTerm **params)
{
  PDDLAtom a = { predicate, params, predicate->terms_nb };
  return (Resource *) gdsl_rbtree_search(domain->resources_table, (gdsl_compare_func_t) resource_atom_cmp, &a);
}

static Resource *instantiate_resource(PDDLDomain *domain, PDDLAtom *atom, PDDLTerm **params)
{
  PDDLAtom *atomi = cpt_calloc(atomi, 1);
  atomi->predicate = atom->predicate;
  cpt_malloc(atomi->terms, (atomi->terms_nb = atom->terms_nb));
  FORi(term, i, atom->terms) { atomi->terms[i] = (term->is_var ? params[term->index] : term); } EFOR;
  Resource *resource = (Resource *) gdsl_rbtree_insert(domain->resources_table, atomi, &lost_int);
  if (atomi != resource->atom) { cpt_free(atomi->terms); cpt_free(atomi); } 
  else gdsl_queue_insert(domain->resources_queue, resource);
  return resource;
}
#endif


/*  ******************************* */
/*  *  GDSL helpers (comparison)  * */
/*  ******************************* */


static long atoms_cmp(PDDLAtom *atom1, PDDLAtom *atom2)
{
  /* if (atom1->predicate != atom2->predicate) return (long) atom1->predicate - (long) atom2->predicate; */
  /* /\* if (atom1->predicate < atom2->predicate) return -1; *\/ */
  /* /\* if (atom1->predicate > atom2->predicate) return 1; *\/ */
  /* return memcmp(atom1->terms, atom2->terms, atom1->terms_nb * sizeof(PDDLTerm *)); */
  
  if (atom1->predicate != atom2->predicate) return strcmp(atom1->predicate->name, atom2->predicate->name);
  FOR2(t1, atom1->terms, t2, atom2->terms) {
    if (t1 != t2) return strcmp(t1->name, t2->name);
  } EFOR;
  return 0;
}

static long litterals_cmp(PDDLLitteral *litteral1, PDDLLitteral *litteral2)
{
  return atoms_cmp(litteral1->atom, litteral2->atom);
}

static long litteral_atom_cmp(PDDLLitteral *litteral, PDDLAtom *atom)
{
  return atoms_cmp(litteral->atom, atom);
}

static long fluent_atom_cmp(Fluent *fluent, PDDLAtom *atom)
{
  return atoms_cmp(fluent->atom, atom);
}

#ifdef RESOURCES
static long resource_atom_cmp(Resource *resource, PDDLAtom *atom)
{
  return atoms_cmp(resource->atom, atom);
}
#endif

/*  *************************** */
/*  *  Instantiation helpers  * */
/*  *************************** */


static bool verify_constraint(PDDLDomain *domain, PDDLLitteral *litteral, PDDLTerm **params)
{
  PDDLAtom *atom = litteral->atom;
  if (litteral->mod.equality == EQUAL_MOD) {
    if (litteral->mod.sign == POS_MOD) return params[atom->terms[0]->index] == params[atom->terms[1]->index];
    else return params[atom->terms[0]->index] != params[atom->terms[1]->index];
  }
  PDDLTerm *terms[atom->terms_nb];
  PDDLAtom atomi = { atom->predicate, terms, atom->terms_nb };
  FORi(term, i, atom->terms) { terms[i] = (term->is_var ? params[term->index] : term); } EFOR;
  return gdsl_rbtree_search(domain->constraints_table, (gdsl_compare_func_t) atoms_cmp, &atomi) != NULL;
}

static PDDLExpression *evaluate_function(PDDLDomain *domain, PDDLLitteral *litteral, PDDLTerm **params)
{
  PDDLAtom *atom = litteral->atom;
  PDDLTerm *terms[atom->terms_nb];
  PDDLAtom atomi = { atom->predicate, terms, atom->terms_nb };
  FORi(term, i, atom->terms) { terms[i] = (term->is_var ? params[term->index] : term); } EFOR;
  PDDLLitteral *litt = (PDDLLitteral *) gdsl_rbtree_search(domain->functions_init_table, (gdsl_compare_func_t) litteral_atom_cmp, &atomi);
  return (litt ? litt->value : NULL);
}

static bool evaluate_expression(PDDLDomain *domain, mpq_t res, PDDLExpression *expr, PDDLTerm **params)
{
  mpq_t tmp;
  switch (expr->mod) {
  case NUM_MOD: 
    mpq_set(res, expr->number); 
    return true;
  case ATOM_MOD:
    if (!(expr = evaluate_function(domain, expr->litteral, params))) return false;
    mpq_set(res, expr->number); 
    return true;
  default:
    if (!evaluate_expression(domain, res, expr->terms[0], params)) return false;
    mpq_init(tmp);
    FORMIN(term, expr->terms, 1) {
      if (!evaluate_expression(domain, tmp, term, params)) { mpq_clear(tmp) ; return false; }
      switch (expr->mod) {
      case ADD_MOD: mpq_add(res, res, tmp); break;
      case SUB_MOD: mpq_sub(res, res, tmp); break;
      case MUL_MOD: mpq_mul(res, res, tmp); break;
      default: mpq_div(res, res, tmp);
      }
    } EFOR;
    mpq_clear(tmp);
    return true;
  }
}



/*  ******************* */
/*  *  Instantiation  * */
/*  ******************* */


static void read_actions_from_file(PDDLDomain *domain)
{
    FILE *file = fopen(opt.read_actions_input, "r");
    char buffer[STRING_MAX];
    domain->action_names = gdsl_rbtree_alloc("actions", NULL, NULL, (gdsl_compare_func_t) strcmp);
    char *s;
    do {
      if (fscanf(file, "%[^\n]", buffer) == EOF) s = NULL; 
      else s = strdup(buffer); 
      if (fscanf(file, "\n") != 0) error(no_file, "Action file does not exist");
      if (s && s[0] != '\0') {
	char *s2 = s;
	while (*s2) if (*s2 == '(') s = s2++; else if (*s2 == ')') *++s2 = '\0'; else { *s2 = tolower(*s2); s2++; }
	gdsl_rbtree_insert(domain->action_names, s, &lost_int);
      }
    } while (s != NULL);
    fclose(file);
}

#ifdef RESOURCES
static bool compute_resource(PDDLDomain *domain, PDDLLitteral *litteral, Action *action, PDDLTerm **params, long params_nb)
{
  Resource *resource = instantiate_resource(domain, litteral->atom, params);
  ResourceLocal *rl;
  mpq_t val_tmp;

  FOR(rl2, action->resources) {
    if (rl2->resource == resource) { rl = rl2; goto suite; }
  } EFOR;
  rl = cpt_calloc(action->resources[action->resources_nb++], 1);
  rl->action = action;
  rl->resource = resource;
  rl->min_level = VAL_UNKNOWN;
  rl->max_level = VAL_UNKNOWN;
  rl->increased = VAL_UNKNOWN;
  rl->decreased = VAL_UNKNOWN;
  rl->assigned = VAL_UNKNOWN;
 suite:
  mpq_init(val_tmp);
  if (!evaluate_expression(domain, val_tmp, litteral->value, params)) { mpq_clear(val_tmp); return false; }
  long qty = mpz_get_timeval(mpq_numref(val_tmp)) / mpz_get_timeval(mpq_denref(val_tmp));
  switch (litteral->mod.function) {
  case LT_MOD: qty--;
  case LTE_MOD: rl->max_level = qty; break;
  case GT_MOD: qty++;
  case GTE_MOD: rl->min_level = qty; break;
  case INC_MOD: rl->increased = qty; break;
  case DEC_MOD: rl->decreased = qty; break;
  case EQ_MOD:
  case ASS_MOD: rl->assigned = qty; break;
  }
  mpq_clear(val_tmp);
  return true;
}

static bool compute_resources(PDDLDomain *domain, PDDLOperator *ope, Action *action, PDDLTerm **params, long params_nb)
{
  cpt_malloc(action->resources, ope->resources_nb);
  FOR(litteral, ope->precondition) {
    if (atomic(litteral) && function(litteral) && !compute_resource(domain, litteral, action, params, params_nb)) return  false;
  } EFOR;
  FOR(litteral, ope->effect) {
    if (atomic(litteral) && function(litteral) && !compute_resource(domain, litteral, action, params, params_nb)) return false;
  } EFOR;
  return true;
}
#endif

static void instantiate_operator(PDDLDomain *domain, PDDLOperator *ope, PDDLTerm **params, long params_nb, long index)
{
  if (index == params_nb) {
    if (opt.read_actions && ope != domain->operators[0] && ope != domain->operators[1]) {
      char *name = make_name(ope->name, params, params_nb);
      if (!gdsl_rbtree_search(domain->action_names, (gdsl_compare_func_t) strcmp, (void *) name)) return;
    }
    mpq_t dur_tmp, dur_tmp2;
    mpq_init(dur_tmp);
    if (ope->duration) { if (!evaluate_expression(domain, dur_tmp, ope->duration, params)) { mpq_clear(dur_tmp); return; } }
    else if (gdsl_queue_get_size(domain->actions_queue) >= 2) mpq_set_si(dur_tmp, (domain->action_costs ? 0 : 1), 1);

    if (ope->real_duration) {
      mpq_init(dur_tmp2);
      if (!evaluate_expression(domain, dur_tmp2, ope->real_duration, params)) { mpq_clear(dur_tmp); mpq_clear(dur_tmp2); return; }
    }

    Action *action = cpt_calloc(action, 1);
    action->ope = ope;
#ifdef RESOURCES
    if (ope->resources_nb > 0 && !compute_resources(domain, ope, action, params, params_nb)) {
      FOR(r, action->resources) { cpt_free(r); } EFOR;
      cpt_free(action->resources);
      cpt_free(action);
      mpq_clear(dur_tmp);
      if (ope->real_duration) mpq_init(dur_tmp2);
      return;
    }
    if (domain->action_costs && action->resources) {
      if (val_known(action->resources[0]->increased)) mpq_set_si(dur_tmp, action->resources[0]->increased, 1);
      cpt_free(action->resources[0]);
      cpt_free(action->resources);
      action->resources_nb = 0;
    }
#endif
    mpq_set(duration_rat(action), dur_tmp);
    gdsl_queue_insert(domain->durations_queue, &action->dur);
    mpq_clear(dur_tmp);

    if (ope->real_duration) {
      mpq_set(action->rdur.q, dur_tmp2);
      gdsl_queue_insert(domain->durations_queue, &action->rdur);
      mpq_clear(dur_tmp2);
    }

    vector_copy(action->parameters, params);
    //gdsl_queue_insert(domain->actions_queue, action);
#ifndef RESOURCES
    cpt_malloc(action->prec, ope->prec_nb);
    cpt_malloc(action->add, ope->add_nb);  
    cpt_malloc(action->del, ope->del_nb);
#else
    cpt_malloc(action->prec, ope->prec_nb + action->resources_nb);
    cpt_malloc(action->add, ope->add_nb + action->resources_nb);  
    cpt_malloc(action->del, ope->del_nb + action->resources_nb);
#endif
    FOR(litteral, ope->precondition) {
      if (atomic(litteral) && !function(litteral)) {
	Fluent *fluent = instantiate_atom(domain, litteral->atom, params);
	FOR(f, action->prec) { if (f == fluent) goto suite; } EFOR;
	action->prec[action->prec_nb++] = fluent;
      suite:;
      }
    } EFOR;
    FOR(litteral, ope->effect) {
      if (atomic(litteral) && !function(litteral)) {
	Fluent *fluent = instantiate_atom(domain, litteral->atom, params);
	if (litteral->mod.sign == POS_MOD) action->add[action->add_nb++] = fluent;
	if (litteral->mod.sign == NEG_MOD || litteral->mod.function == ASS_MOD) action->del[action->del_nb++] = fluent;
      }
    } EFOR;
    if (cpt_malloc(action->ac_constraints, (action->ac_constraints_nb = ope->ac_constraints_nb))) {
      FORi(ac, i, ope->ac_constraints) {
	ActivityConstraint *aci = cpt_calloc(action->ac_constraints[i], 1);
	aci->fluent = instantiate_atom(domain, ac->litteral->atom, params);
	aci->mandatory = strchr(ac->litteral->atom->predicate->name, '@') != NULL;
	FOR(f, action->add) { if (f == aci->fluent) goto good; } EFOR;
	error(parser, "Activity constraint %s of action %s unsupported by action effects", 
	      fluent_name(aci->fluent), strdup(action_name(action)));
      good:
	mpq_init(aci->min.q);
	evaluate_expression(domain, aci->min.q, ac->min, params);
	gdsl_queue_insert(domain->durations_queue, &aci->min);
	mpq_init(aci->max.q);
	evaluate_expression(domain, aci->max.q, ac->max, params);
	gdsl_queue_insert(domain->durations_queue, &aci->max);
	if (ac->inter) {
	  PDDLExpression *expr = evaluate_function(domain, ac->inter->litteral, params);
	  if (expr) { // sinon : il faudrait détruire l'action !!!!!
	    if (expr->time == NULL) {
	      cpt_malloc(expr->time, (expr->time_nb = expr->terms_nb / 2));
	      cpt_malloc(expr->dur, (expr->dur_nb = expr->terms_nb / 2));
	      FORi(term, i, expr->terms) {
		TimeStruct *ts = &((i % 2 == 0 ? expr->time : expr->dur)[i / 2]);
		mpq_init(ts->q);
		evaluate_expression(domain, ts->q, term, params);
		gdsl_queue_insert(domain->durations_queue, ts);
	      } EFOR;
	    }
	    aci->time = expr->time;
	    aci->time_nb = expr->time_nb;
	    aci->dur = expr->dur;
	    aci->dur_nb = expr->dur_nb;
#ifdef RATP
	    if (opt.ratp) expr->time = NULL;
#endif
	  }
	}
      } EFOR;
    }
    gdsl_queue_insert(domain->actions_queue, action);
  } else {
    long i;
    VECTOR(PDDLTerm *, constants); 
    if (ope->parameters[index]->types) {
      constants = ope->parameters[index]->types[0]->constants;
      constants_nb = ope->parameters[index]->types[0]->constants_nb;
    } else {
      constants = domain->constants_all;
      constants_nb = domain->constants_all_nb;
    }
    FOR(c, constants) {
      params[index] = c;
      for (i = 0; i < ope->constraints_nb[index]; i++) 
	if (!verify_constraint(domain, ope->constraints[index][i], params)) goto abort;
      instantiate_operator(domain, ope, params, params_nb, index + 1);
    abort:;
    } EFOR;
  }
}

static int add_constant_to_domain(PDDLTerm *term, gdsl_location_t loc, PDDLTerm **terms)
{
  terms[lost_int++] = term;
  return GDSL_MAP_CONT;
} 

static void compute_nb_resources(PDDLOperator *ope, PDDLLitteral *litteral)
{
  ope->resources_nb++;
  switch (litteral->mod.function) {
  case LTE_MOD: 
  case LT_MOD: ope->inf_needed_nb++; break;
  case GTE_MOD: 
  case GT_MOD: ope->sup_needed_nb++; break;
  case INC_MOD: ope->increased_nb++; break;
  case DEC_MOD: ope->decreased_nb++; break;
  case EQ_MOD:
  case ASS_MOD: ope->assigned_nb++; break;
  }
}

#ifdef RESOURCES
Fluent *create_resource_fluent(PDDLDomain *domain, Resource *resource, const char *append)
{
  PDDLAtom *atom = cpt_calloc(atom, 1);
  PDDLPredicate *pred = cpt_calloc(pred, 1);
  char name[strlen(resource->atom->predicate->name) + strlen(append) + 1];
  sprintf(name, "%s%s", resource->atom->predicate->name, append);
  pred->name = strdup(name);
  atom->terms = resource->atom->terms;
  atom->terms_nb = resource->atom->terms_nb;
  atom->predicate = pred;
  Fluent *fluent = (Fluent *) gdsl_rbtree_insert(domain->fluents_table, atom, &lost_int);
  fluent->resource = resource;
  gdsl_queue_insert(domain->fluents_queue, fluent);
  return fluent;
}
#endif

static int compute_ppcm_and_apply_precision(const gdsl_element_t e, gdsl_location_t location, void *user_data)
{
  TimeStruct *ts = (TimeStruct *) e;
  PDDLDomain* domain = (PDDLDomain *) user_data;
  if (opt.precision && mpq_sgn(domain->precision.q) != 0) {
    mpz_t tmp, tmp2;
    mpz_init_set(tmp, mpq_numref(ts->q));
    mpz_mul(tmp, tmp, mpq_denref(domain->precision.q));
    mpz_init_set(tmp2, mpq_denref(ts->q));
    mpz_mul(tmp2, tmp2, mpq_numref(domain->precision.q));
    mpz_cdiv_q(tmp, tmp, tmp2);
    mpz_mul(tmp, tmp, mpq_numref(domain->precision.q));
    mpq_set_num(ts->q, tmp);  
    mpq_set_den(ts->q, mpq_denref(domain->precision.q));
    mpq_canonicalize(ts->q);
    mpz_clear(tmp);
    mpz_clear(tmp2);
  }
  mpz_lcm(domain->time_lcm_rat, domain->time_lcm_rat, mpq_denref(ts->q));
  return GDSL_MAP_CONT;
}

static int compute_pgcd(const gdsl_element_t e, gdsl_location_t location, void *user_data)
{
  TimeStruct *ts = (TimeStruct *) e;
  PDDLDomain* domain = (PDDLDomain *) user_data;
  mpz_t tmp;
  mpz_init_set(tmp, mpq_numref(ts->q));
  mpz_mul(tmp, tmp, domain->time_lcm_rat);
  mpz_div(tmp, tmp, mpq_denref(ts->q));
  if (mpz_sgn(domain->time_gcd_rat) == 0) mpz_set(domain->time_gcd_rat, tmp);	
  else mpz_gcd(domain->time_gcd_rat, domain->time_gcd_rat, tmp);
  mpz_clear(tmp);
  return GDSL_MAP_CONT;
}


static int compute_duration(const gdsl_element_t e, gdsl_location_t location, void *user_data)
{
  TimeStruct *ts = (TimeStruct *) e;
  PDDLDomain* domain = (PDDLDomain *) user_data;
  mpz_t tmp, tmp2;
  mpz_init_set(tmp2, mpq_numref(ts->q));
  mpz_mul(tmp2, tmp2, domain->time_lcm_rat);
  mpz_init_set(tmp, mpq_denref(ts->q));
  mpz_mul(tmp, tmp, domain->time_gcd_rat);
  mpz_cdiv_q(tmp2, tmp2, tmp);
  ts->t = mpz_get_timeval(tmp2);
  mpz_clear(tmp2);
  mpz_clear(tmp);
  return GDSL_MAP_CONT;
}

static void normalize_durations(PDDLDomain *domain)
{
  mpz_set_ui(domain->time_lcm_rat, 1);
  mpz_set_ui(domain->time_gcd_rat, 0);
  gdsl_queue_map_forward(domain->durations_queue, compute_ppcm_and_apply_precision, domain);
  domain->time_lcm = mpz_get_timeval(domain->time_lcm_rat);
  gdsl_queue_map_forward(domain->durations_queue, compute_pgcd, domain);
  domain->time_gcd = mpz_get_timeval(domain->time_gcd_rat);
  maximize(domain->time_gcd, 1);
#ifdef RATP
  if (opt.ratp) domain->time_gcd = 1;
#endif
  gdsl_queue_map_forward(domain->durations_queue, compute_duration, domain);
  FOR(a, domain->actions) {
    FOR(ac, a->ac_constraints) {
      if (ac->dur) {
	ac->real_min = MAXTIME;
	FOR(d, ac->dur) { minimize(ac->real_min, ac->min.t + d.t); } EFOR;
      } else ac->real_min = ac->min.t;
    } EFOR;
  } EFOR;
}

void instantiate_operators(PDDLDomain *domain)
{
  long i;

  domain->fluents_table = gdsl_rbtree_alloc(NULL, (gdsl_alloc_func_t) fluent_allocate, NULL, (gdsl_compare_func_t) fluent_atom_cmp);
  domain->constraints_table = gdsl_rbtree_alloc(NULL, NULL, NULL, (gdsl_compare_func_t) atoms_cmp);
  domain->functions_init_table = gdsl_rbtree_alloc(NULL, NULL, NULL, (gdsl_compare_func_t) litterals_cmp);
  domain->actions_queue = gdsl_queue_alloc(NULL, NULL, NULL);
  domain->fluents_queue = gdsl_queue_alloc(NULL, NULL, NULL);
  domain->durations_queue = gdsl_queue_alloc(NULL, NULL, NULL);
#ifdef RESOURCES
  domain->resources_table = gdsl_rbtree_alloc(NULL, (gdsl_alloc_func_t) resource_allocate, NULL, (gdsl_compare_func_t) resource_atom_cmp);
  domain->resources_queue = gdsl_queue_alloc(NULL, NULL, NULL);
#endif

  FOR(type, domain->types) {
    cpt_malloc(type->constants, (type->constants_nb = gdsl_rbtree_get_size(type->constants_table)));
    lost_int = 0;
    gdsl_rbtree_map_prefix(type->constants_table, (gdsl_map_func_t) add_constant_to_domain, (void *) type->constants);
  } EFOR;

  FOR(ope, domain->operators) {
    FOR(litteral, ope->effect) { 
      if (litteral->mod.sign == POS_MOD) set_atomic(litteral); 
      else if (litteral->mod.function == NO_FUNC_MOD) set_constraint(litteral);
    } EFOR;
    FOR(ac, ope->ac_constraints) { set_atomic(ac->litteral); } EFOR;
    FOR(ac, domain->ac_constraints) { set_atomic(ac->litteral); } EFOR;
  } EFOR;

  FOR(predicate, domain->predicates) {
    if (predicate->in_effect) predicate->in_effect2 = false;
    if (predicate->in_effect2) predicate->in_effect = true;
  } EFOR;

  FOR(litteral, domain->init) {
    if (litteral->mod.function == EQ_MOD) gdsl_rbtree_insert(domain->functions_init_table, litteral, &lost_int);
    else if (is_constraint(litteral)) gdsl_rbtree_insert(domain->constraints_table, litteral->atom, &lost_int);
  } EFOR;

  domain->operators -= 2;
  domain->operators_nb += 2;
  cpt_calloc(domain->operators[0], 1);
  cpt_calloc(domain->operators[1], 1);
  domain->operators[0]->name = "Start";
  domain->operators[0]->effect = domain->init;
  domain->operators[0]->effect_nb = domain->init_nb;
  domain->operators[0]->ac_constraints = domain->ac_constraints;
  domain->operators[0]->ac_constraints_nb = domain->ac_constraints_nb;
  domain->operators[1]->name = "End";
  domain->operators[1]->precondition = domain->goal;
  domain->operators[1]->precondition_nb = domain->goal_nb;

  if (opt.read_actions) read_actions_from_file(domain);

  FOR(ope, domain->operators) {
    cpt_calloc(ope->constraints_nb, maxi(1, ope->parameters_nb));
    FOR(litteral, ope->precondition) {
      if (atomic(litteral)) ope->prec_nb++;
      if (is_constraint(litteral)) {
	litteral->max_index = 0;
	FOR(term, litteral->atom->terms) {
	  maximize(litteral->max_index, term->index);
	} EFOR;
	ope->constraints_nb[litteral->max_index]++;
      }
      if (function(litteral)) compute_nb_resources(ope, litteral);
    } EFOR;
    FOR(litteral, ope->effect) {
      if (atomic(litteral) && !function(litteral)) {
	if (litteral->mod.sign == POS_MOD) ope->add_nb++;
	else ope->del_nb++;
      }
      if (function(litteral)) compute_nb_resources(ope, litteral);
    } EFOR;
    cpt_malloc(ope->constraints, maxi(1, ope->parameters_nb));
    for (i = 0; i < maxi(1, ope->parameters_nb); i++) {
      cpt_malloc(ope->constraints[i], ope->constraints_nb[i]);
      ope->constraints_nb[i] = 0;
    }
    FOR(litteral, ope->precondition) {
      if (is_constraint(litteral))
	ope->constraints[litteral->max_index][ope->constraints_nb[litteral->max_index]++] = litteral;
    } EFOR;
    PDDLTerm *params[ope->parameters_nb];
    instantiate_operator(domain, ope, params, ope->parameters_nb, 0);
  } EFOR;

#ifdef RESOURCES
  if (!domain->action_costs) {
    cpt_malloc(domain->resources, (domain->resources_nb = gdsl_queue_get_size(domain->resources_queue)));
    for (i = 0; i < domain->resources_nb; i++)
      domain->resources[i] = (Resource *) gdsl_queue_remove(domain->resources_queue);
    gdsl_queue_free(domain->resources_queue);
  }

  FOR(resource, domain->resources) {
    Fluent *fluent_available = create_resource_fluent(domain, resource, "-available");
    Fluent *fluent_modified = create_resource_fluent(domain, resource, "-modified");
    Fluent *fluent_synchro = create_resource_fluent(domain, resource, "-synchro");
    resource->fluent_available = fluent_available;
    resource->fluent_modified = fluent_modified;
    resource->fluent_synchro = fluent_synchro;
    //fluent_modified->no_branching = true;
    //fluent_synchro->no_branching = true;
    fluent_available->no_branching = true;
    Action *action = (Action *) gdsl_queue_insert(domain->actions_queue, cpt_calloc(action, 1));
    mpq_init(duration_rat(action));
    action->synchro = true;
    cpt_calloc(action->ope, 1);
    char name [strlen(resource->atom->predicate->name) + 9];
    sprintf(name, "synchro-%s", resource->atom->predicate->name);
    action->ope->name = strdup(name);
    action->parameters_nb = fluent_modified->atom->terms_nb;
    action->parameters = fluent_modified->atom->terms;
    cpt_malloc(action->prec, (action->prec_nb = 2));
    cpt_malloc(action->add, (action->add_nb = 2));
    cpt_malloc(action->del, (action->del_nb = 3));
    action->prec[0] = fluent_modified;
    action->prec[1] = fluent_synchro;
    action->add[0] = fluent_available;
    action->add[1] = fluent_synchro;
    action->del[0] = fluent_modified;
    action->del[1] = fluent_available;
    action->del[2] = fluent_synchro;
  } EFOR;
#endif

  cpt_calloc(domain->actions, (domain->actions_nb = gdsl_queue_get_size(domain->actions_queue)) + opt.max_plan_length + 1);
  for (i = 0; i < domain->actions_nb; i++) {
    domain->actions[i] = (Action *) gdsl_queue_remove(domain->actions_queue);
    domain->actions[i]->origin = domain->actions[i];
  }
  gdsl_queue_free(domain->actions_queue);

  //trace(normal, "ac : %ld\n", domain->actions_nb);

  for (i = domain->actions_nb; i < domain->actions_nb + opt.max_plan_length; i++)
    cpt_calloc(domain->actions[i], 1);
  
#ifdef RESOURCES  
  Action *start = domain->actions[0];
  cpt_realloc(start->add, start->add_nb + domain->resources_nb * 2);
  FOR(r, domain->resources) {
    start->add[start->add_nb++] = r->fluent_synchro;
  } EFOR;

  Action *end = domain->actions[1];
  cpt_realloc(end->prec, end->prec_nb + domain->resources_nb*2 + end->resources_nb);
  FOR(r, domain->resources) {
    end->prec[end->prec_nb++] = r->fluent_available;
    end->prec[end->prec_nb++] = r->fluent_synchro;
  } EFOR;

  FOR(a, domain->actions) {
    FOR(r, a->resources) {
      if (a != start) a->prec[a->prec_nb++] = r->resource->fluent_available;
      if (a != end) a->add[a->add_nb++] = r->resource->fluent_modified;
      if (a != start && a != end && (1||val_known(r->assigned))) a->del[a->del_nb++] = r->resource->fluent_available;
    } EFOR;
  } EFOR;
#endif
  
  cpt_malloc(domain->fluents, (domain->fluents_nb = gdsl_queue_get_size(domain->fluents_queue)));
  for (i = 0; i < domain->fluents_nb; i++)
    domain->fluents[i] = (Fluent *) gdsl_queue_remove(domain->fluents_queue);
  gdsl_queue_free(domain->fluents_queue);

  if (opt.precision) gdsl_queue_insert(domain->durations_queue, &domain->precision);
  if (!opt.precision2) {
    gdsl_queue_insert(domain->durations_queue, &domain->bound);
    if (opt.max_makespan) gdsl_queue_insert(domain->durations_queue, &domain->max_makespan);
  }

  mpz_init(domain->time_lcm_rat);
  mpz_init(domain->time_gcd_rat);
  normalize_durations(domain);

  if (opt.pddl21 && opt.precision2) {
    char *prec = opt.precision;
    opt.precision = NULL;
    gdsl_queue_insert(domain->durations_queue, &domain->bound);
    gdsl_queue_insert(domain->durations_queue, &domain->precision2);
    if (opt.max_makespan) gdsl_queue_insert(domain->durations_queue, &domain->max_makespan);
    normalize_durations(domain);
    opt.precision = prec;
  }

  if (!opt.max_makespan) pddl_domain->max_makespan.t = MAXTIME;

  if (opt.pddl21 && pddl_domain->max_makespan.t < MAXTIME) 
    pddl_domain->max_makespan.t += pddl_domain->precision.t;
  gdsl_queue_free(domain->durations_queue);
}
