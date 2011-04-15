/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : ratp.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#include "cpt.h"
#include "trace.h"
#include "structs.h"
#include "globs.h"
#include "problem.h"
#include "solve.h"
#include "plan.h"
#include "trace_planner.h"
#include "ratp_structs.h"


SolutionPlan *solution_plan;

static RatpProblem *pb;

static char symbols_buffer[STRING_MAX];


#define MAX_COURSES 10
#define MAX_TRAJECTS 10

#define my_fscanf(x...) NEST( if (fscanf(x) <= 0) error(no_file, "Problem in RATP input file"); )
#define read_string(source, s)  NEST( my_fscanf(source, "%s", symbols_buffer); s = strdup(symbols_buffer); )
#define read_number(source, i) my_fscanf(source, "%ld" , &(i))
#define read_timeval(source, i) my_fscanf(source, "%" TIMEP , &(i))
#define readln_number(source, i) my_fscanf(source, "%ld%*[^\n]", &(i))
#define readln_timeval(source, i) my_fscanf(source, "%" TIMEP "%*[^\n]", &(i))

#define linearval(x,xa,ya,xb,yb) time_round(((x) - (xa)) * (long double) ((yb) - (ya)) / ((xb) - (xa)) + (ya))

static void display_graph();


static TimeVal compute_time_point(TimeVal time, TimePoint *time_points, long time_points_nb)
{
  FORi(p, i, time_points) {
    TimePoint p2 = time_points[i+1];
    if (p.time <= time && time <= p2.time)
      return linearval(time, p.time, p.duration, p2.time, p2.duration);
  } EFOR;
  return -1;
}

static TimeVal compute_interval(Terminus *term, TimeVal time)
{
  return compute_time_point(time, term->freq_tp, term->freq_tp_nb);
}

static TimeVal compute_duration(Mission *mission, TimeVal time)
{
  return compute_time_point(time, mission->duration_tp, mission->duration_tp_nb);
}

static TimeVal compute_time_point_inverse(TimeVal time, TimePoint *time_points, long time_points_nb)
{
  FORi(p, i, time_points) {
    TimePoint p2 = time_points[i+1];
    if (p.time + p.duration <= time && time <= p2.time + p2.duration)
      return linearval(time, p.time+p.duration, p.time, p2.time+p2.duration, p2.time);
  } EFOR;
  return -1;
}

static TimeVal compute_start_time(Mission *mission, TimeVal time)
{
  return compute_time_point_inverse(time, mission->duration_tp, mission->duration_tp_nb);
}

static void print_terminus(Terminus *t)
{
  trace(ratp, "\nTerminus %s :\n", t->name);
  trace(ratp, "\tPremier départ min : %" TIMEP "\n", t->start_min);
  trace(ratp, "\tPremier départ max : %" TIMEP "\n", t->start_max);
  trace(ratp, "\tDernier départ min : %" TIMEP "\n", t->end_min);
  trace(ratp, "\tDernier départ max : %" TIMEP "\n", t->end_max);
  trace(ratp, "\tTolérance d'intervalle : %" TIMEP "\n", t->freq_tol);
  trace(ratp, "\tIntervalles : ");
  FOR(p, t->freq_tp) { trace(ratp, "[%" TIMEP ",%" TIMEP "]", p.time, p.duration); } EFOR;
  trace(ratp, "\n");
  trace(ratp, "\tAttente voiture min : %" TIMEP "\n", t->wait_min);
  trace(ratp, "\tAttente voiture max : %" TIMEP "\n", t->wait_max);
  trace(ratp, "\tNombre de courses max : %ld\n", t->courses_nb);
}

static void read_time_points(FILE *source, TimePoint **time_points, long *time_points_nb) {
  long i;

  read_number(source, *time_points_nb);
  cpt_malloc(*time_points, *time_points_nb);
  for(i = 0; i < *time_points_nb; i++) {
    read_timeval(source, (*time_points)[i].time);
    read_timeval(source, (*time_points)[i].duration);
  }
}

//#define PARTIELS 

static Terminus *read_terminus(FILE *source, RatpProblem *pb, long id)
{
  Terminus *term = cpt_calloc(term, 1);

  term->id = id;
  read_string(source, term->name);
  readln_timeval(source, term->start_min);
#ifdef PARTIELS
  term->start_max = term->start_min;
#else
  readln_timeval(source, term->start_max);
#endif
  readln_timeval(source, term->end_min);
#ifdef PARTIELS
  term->end_max = term->end_min;
#else
  readln_timeval(source, term->end_max);
#endif
  readln_timeval(source, term->freq_tol);
  read_time_points(source, &term->freq_tp, &term->freq_tp_nb);
  readln_timeval(source, term->wait_min);
  readln_timeval(source, term->wait_max);
  term->wait_max = 100000;
  term->courses_nb = 0;
  return term;
}  

static void print_mission(Mission *m)
{
  trace(ratp, "\nMission :\n");
  trace(ratp, "\tSource : %s\n", m->src->name);
  trace(ratp, "\tDestination : %s\n", m->dst->name);
  trace(ratp, "\tIntervalles : ");
  FOR(p, m->duration_tp) { 
    trace(ratp, "(%" TIMEP ",%" TIMEP ")", p.time, p.duration); 
  } EFOR;
  trace(ratp, "\n\tFenetres : ");
  FOR(t, m->windows) { 
    trace(ratp, "%" TIMEP " ", t); 
  } EFOR;
  trace(ratp, "\n");
}

static Terminus *find_terminus(RatpProblem *pb, char *name)
{
  FOR(t, pb->terminus) { if (strcmp(t->name, name) == 0) return t; } EFOR;
  return NULL;
}

static Mission *read_mission(FILE *source, RatpProblem *pb, long id)
{
  Mission *m = cpt_calloc(m, 1);
  char *name;
  long i;

  m->id = id;
  read_string(source, name);
  m->src = find_terminus(pb, name);
  free(name);
  read_string(source, name);
  m->dst = find_terminus(pb, name);
  free(name);
  read_time_points(source, &m->duration_tp, &m->duration_tp_nb);
  read_number(source, m->windows_nb);
  m->windows_nb *= 2;
  cpt_malloc(m->windows, m->windows_nb);
  for(i = 0; i < m->windows_nb; i++)
    read_timeval(source, m->windows[i]);
  return m;
}

static Course *find_course(Voiture *v, Terminus *src, Terminus *dst, long idx_course) 
{
  FOR(t, v->trajects) {
    if (t->courses_nb > idx_course && t->courses[idx_course]->mission->src == src && t->courses[idx_course]->mission->dst == dst)
      return t->courses[idx_course];
  } EFOR;
  return NULL;
}

static void print_traject(Traject *t)
{
  FOR(c, t->courses) {
    trace(ratp, "%s->%s ", c->mission->src->name, c->mission->dst->name);
  } EFOR;
}

static void print_voiture(Voiture *v) 
{
  trace(ratp, "Voiture %s \n ", v->name);
  trace(ratp, "\tHeure début : %" TIMEP "\n", v->start);
  trace(ratp, "\tHeure fin : %" TIMEP "\n", v->end);
  trace(ratp, "\tTerminus initial : %s\n", v->start_term->name);
  if (v->final_term)
    trace(ratp, "\tTerminus final : %s\n", v->final_term->name);
  trace(ratp, "\tTrajets :\n");
  FOR(t, v->trajects) { trace(ratp, "\t* "); print_traject(t); trace(ratp, "\n"); } EFOR;
  trace(ratp, "\n");  
}

static void create_traject(Voiture *v, Course *courses, long idx_final)
{
  //if (idx_final > 6) return;
  Traject *t = cpt_calloc(v->trajects[v->trajects_nb++], 1);
  long i;
  cpt_malloc(t->courses, (t->courses_nb = idx_final));
  for (i = 0; i < idx_final; i++) 
    memcpy(cpt_malloc(t->courses[i], 1), &courses[i], sizeof(Course));
  maximize(v->courses_nb, t->courses_nb);
}

static void build_trajects(RatpProblem *pb, Voiture *v, Course *courses, long courses_nb, long idx_final, TimeVal start_init, Terminus *src) 
{
  bool mission_found = false;

  trace(ratp, "\trecherche de candidats, de %s à %" TIMEP " (idx final : %ld) :\n", src->name, start_init, idx_final);
  FOR(m, pb->missions) {
    if (m->src == src && start_init <= m->windows[1]) {
      trace(ratp, "\tmission candidate : %s->%s", m->src->name, m->dst->name);
      trace(ratp, "\n");
      TimeVal start = maxi(start_init, m->windows[0]);
      TimeVal d = compute_duration(m, start);
      if (d != -1 && start + d <= v->end) {
	mission_found = true;
	courses[courses_nb].id = courses_nb;
	courses[courses_nb].voiture = v;
	courses[courses_nb].mission = m;
	courses[courses_nb].min_start = start;
	
	trace(ratp, "    Course %ld :\n", courses_nb);
	trace(ratp, "\tdébut : %" TIMEP " -- fin : %" TIMEP " -- durée : %" TIMEP "\n", start, start + d, d);
	start = start + d + m->dst->wait_min;
	
	long idx = (m->dst == v->final_term ? courses_nb + 1 : idx_final);
	build_trajects(pb, v, courses, courses_nb + 1, idx, start, m->dst);
      }
    }
  } EFOR;
  if (!mission_found && idx_final > 0) create_traject(v, courses, idx_final);
}


static void make_trajects(RatpProblem *pb, Voiture *v)
{
  Course courses[MAX_COURSES];

  trace(ratp, "\n\nCalcul des courses pour le bus %s (%" TIMEP ",%" TIMEP ")\n", v->name, v->start, v->end);
  cpt_malloc(v->trajects, MAX_TRAJECTS);
  build_trajects(pb, v, courses, 0, 0, maxi(v->start, v->start_term->start_min), v->start_term);
  if (v->trajects_nb > 0) {
    trace(ratp, "==> Trajets possibles :\n");
    FOR(t, v->trajects) { trace(ratp, "\t* "); print_traject(t); trace(ratp, "\n"); } EFOR;
  }
}


static Voiture *read_voiture(FILE *source, RatpProblem *pb, long id)
{
  Voiture *v = cpt_calloc(v, 1);
  char *name;

  v->id = id;
  read_string(source, v->name);
  read_timeval(source, v->start);
  read_timeval(source, v->end);
  maximize(pb->max_time_point, v->end);
  read_string(source, name);
  v->start_term = find_terminus(pb, name);
  free(name);

#ifdef PARTIELS
  read_string(source, name);
  free(name);
#endif

  read_string(source, name);
  v->final_term = find_terminus(pb, name);
  free(name);
  make_trajects(pb, v);
  return v;
}

static void print_ratp_problem(RatpProblem *pb) 
{
  FOR(t, pb->terminus) { print_terminus(t); } EFOR;
  FOR(m, pb->missions) { print_mission(m); } EFOR;
  FOR(v, pb->voitures) { print_voiture(v); } EFOR;
}

static void print_ratp_pddl(RatpProblem *pb)
{
  FILE *out = fopen(opt.facts_file, "w");
  long i, max_courses = 0;
  
  fprintf(out, "(define (problem ratp)\n");
  fprintf(out, "(:domain ratp)\n");
  fprintf(out, "(:objects\n");
  FOR(t, pb->terminus) { fprintf(out, "t%ld ", t->id); } EFOR;
  fprintf(out, "- terminus\n");
  FOR(v, pb->voitures) { 
    fprintf(out, "b%ld ", v->id); 
    maximize(max_courses, v->courses_nb); 
  } EFOR;
  fprintf(out, "- voiture\n");
  for (i = 0; i <= max_courses; i++)  fprintf(out, "c%ld ", i);
  fprintf(out, "- course\n");
  fprintf(out, ")\n(:init\n");
  FOR(t, pb->terminus) { 
    fprintf(out, "(depart_possible t%ld)\n", t->id); 
    fprintf(out, "(= (attente_dernier_depart_min t%ld) %" TIMEP ")\n", t->id, pb->bound - t->end_max);
    fprintf(out, "(= (attente_dernier_depart_max t%ld) %" TIMEP ")\n", t->id, pb->bound - t->end_min);
    fprintf(out, "(= (intervalle_min t%ld) %" TIMEP ")\n", t->id, -t->freq_tol); 
    fprintf(out, "(= (intervalle_max t%ld) %d)\n", t->id, 0); 
    fprintf(out, "(= (intervalle t%ld) (&", t->id);
    FOR(p, t->freq_tp) { fprintf(out, " %" TIMEP " %" TIMEP, p.time, p.duration); } EFOR;
    fprintf(out, "))\n");
    fprintf(out, "(= (battement_min t%ld) %" TIMEP ")\n", t->id, t->wait_min);
    fprintf(out, "(= (battement_max t%ld) %" TIMEP ")\n", t->id, t->wait_max);
  } EFOR;
  FOR(m, pb->missions) {
    fprintf(out, "(mission_possible t%ld t%ld)\n", m->src->id, m->dst->id);
    fprintf(out, "(= (duree_course t%ld t%ld) (&", m->src->id, m->dst->id);
    FOR(p, m->duration_tp) { fprintf(out, " %" TIMEP " %" TIMEP, p.time, p.duration); } EFOR;
    fprintf(out, "))\n");
  } EFOR;
  FOR(v, pb->voitures) {
    if (v->trajects_nb > 0)  {
      fprintf(out, "\n(disponible b%ld c0) ", v->id);
      //fprintf(out, "(en_service b%ld)\n", v->id);
      
      FOR(m, pb->missions) {
	fprintf(out, "(en_service b%ld t%ld t%ld)\n", v->id, m->src->id, m->dst->id);
      } EFOR;
      
      fprintf(out, "(est_en b%ld t%ld)\n", v->id, v->start_term->id);
      FOR(t, v->trajects) {
	long diff = v->courses_nb - t->courses_nb;
	if (diff <= 4) // ICI à baiesser (4 ?) pour plus rapide
	  FORi(c, i, t->courses) {
	    fprintf(out, "(course_suivante b%ld c%ld c%ld t%ld t%ld)\n", 
		    v->id, (i == 0 ? 0 : i + diff), i + 1 + diff, c->mission->src->id, c->mission->dst->id);
	  } EFOR;
      } EFOR;
    }
  } EFOR;
  fprintf(out, ")\n(:constraints (and\n");
  FOR(t, pb->terminus) { fprintf(out, "(active (depart_possible t%ld) %" TIMEP " %" TIMEP ")\n", t->id, t->start_min, t->start_max); } EFOR;
  FOR(v, pb->voitures) {
    if (v->trajects_nb > 0)  {
      // bricolage ici

/*       TimeVal fin = compute_start_time(v->trajects[0]->courses[v->trajects[0]->courses_nb - 1]->mission, v->end); */
/*       fprintf(out, "(active (en_service b%ld) %" TIMEP " %" TIMEP ")\n", v->id, v->start, v->end); */

      FOR(m, pb->missions) {
	TimeVal fin = compute_start_time(m, v->end);
	if (m->dst != v->final_term) fin = 100000;
	fprintf(out, "(active (en_service b%ld t%ld t%ld) %" TIMEP " %" TIMEP ")\n", v->id, m->src->id, m->dst->id, v->start, fin);
      } EFOR;

      fprintf(out, "(active (disponible b%ld c0) %" TIMEP " %" TIMEP ")\n",
	      v->id, v->start, v->start + v->trajects[0]->courses[0]->mission->src->wait_max);
    }
  } EFOR;
  FOR(m, pb->missions) {
    for (i = 0; i < m->windows_nb; i+= 2)
      fprintf(out, "(active (mission_possible t%ld t%ld) %" TIMEP " %" TIMEP ")\n", m->src->id, m->dst->id, m->windows[i], m->windows[i + 1]);
  } EFOR;
  fprintf(out, "))\n(:goal (and\n");
  FOR(t, pb->terminus) { if (t->end_max > 0) fprintf(out, "(fin_journee_terminus t%ld)\n", t->id); } EFOR;
/*     FOR(b, pb->voitures) { fprintf(out, "(est_en b%ld t%ld)\n", b->id, b->final_term->id); } EFOR; */
  FOR(v, pb->voitures) { if (v->courses_nb != 0) fprintf(out, "(derniere_course b%ld c%ld)\n", v->id, v->courses_nb); } EFOR;
  fprintf(out, ")))\n");
  fclose(out);
}

void read_ratp_problem(void)
{
  FILE *source = fopen(opt.ratp_input, "r");
  long i;

  if (source == NULL) error(no_file, "File '%s' does not exist", opt.ratp_input);
  cpt_calloc(pb, 1);
  readln_number(source, pb->terminus_nb);
  cpt_malloc(pb->terminus, pb->terminus_nb);
  for (i = 0; i < pb->terminus_nb; i++) 
    pb->terminus[i] = read_terminus(source, pb, i);
  readln_number(source, pb->missions_nb);
  cpt_malloc(pb->missions, pb->missions_nb);
  for (i = 0; i < pb->missions_nb; i++)
    pb->missions[i] = read_mission(source, pb, i);
  readln_number(source, pb->voitures_nb);
  cpt_malloc(pb->voitures, pb->voitures_nb);
  for (i = 0; i < pb->voitures_nb; i++)
    pb->voitures[i] = read_voiture(source, pb, i);

  FOR(t, pb->terminus) {
    TimeVal start_min = MAXTIME;
    TimeVal end_max = 0;
    FOR(v, pb->voitures) {
      FOR(traj, v->trajects) {
	FOR(c, traj->courses) {
	  if (c->mission->src == t) {
	    minimize(start_min, c->min_start);
	    maximize(end_max, v->end);
	    //break;
	  }
	} EFOR;
      } EFOR;
    } EFOR;
    maximize(t->start_min, start_min);
    maximize(t->start_max, start_min);
    minimize(t->end_min, end_max);
    minimize(t->end_max, end_max);
    maximize(pb->bound, t->end_max);
  } EFOR;
  
  FOR(t, pb->terminus) {
    long i, courses = 0;
    FOR(v, pb->voitures) {
      for (i = 0; i < v->courses_nb; i++) {
	FOR(traj, v->trajects) {
	  if (traj->courses_nb == v->courses_nb && traj->courses[i]->mission->src == t) {
	    courses++;
	    break;
	  }
	} EFOR;
      }
    } EFOR;
    TimeVal freq_min = ceil((double) (t->end_min - t->start_max) / (courses - 1));
    FOR(p, t->freq_tp) {
      maximize(p.duration, freq_min);
      maximize(t->freq_tol, freq_min);
    } EFOR;
  } EFOR;
  
  print_ratp_problem(pb);
  print_ratp_pddl(pb);
/*   pddl_domain->bound.t = pb->bound; */
/*   pddl_domain->max_makespan.t = pb->bound; */
  cpt_malloc(opt.bound, 21);
  cpt_malloc(opt.max_makespan, 21);
  sprintf(opt.bound, "%" TIMEP, pb->bound);
  sprintf(opt.max_makespan, "%" TIMEP, pb->bound);
}

#define param_to_id(step, n) atoi(step->action->parameters[n]->name+1)

static int precedes_in_plan_ratp(const void *a, const void *b)
{
  Step *s1 = *(Step **) a;
  Step *s2 = *(Step **) b;
  int cmp1 = param_to_id(s1, 0) - param_to_id(s2, 0);
  if (cmp1 != 0) return cmp1;
  return param_to_id(s1, 1) - param_to_id(s2, 1);
}

static int precedes_in_courses(const void *a, const void *b)
{
  return (*((Course **) a))->step->init - (*((Course **) b))->step->init;
}

static bool try_to_flip(Voiture *v1, Voiture *v2, Course *c1, Course *c2)
{
  Course *nextv1 = c1->id == v1->courses_nb - 1 ? NULL : v1->courses[c1->id + 1];
  Course *nextv2 = c2->id == v2->courses_nb - 1 ? NULL : v2->courses[c2->id + 1];
  TimeVal from1v1 = c1->step->init + compute_duration(c1->mission, c1->step->init);
  TimeVal from1v2 = c2->step->init + compute_duration(c2->mission, c2->step->init);
  TimeVal tov1 = (nextv1 ? nextv1->step->init : v1->end);
  TimeVal tov2 = (nextv2 ? nextv2->step->init : v2->end);
  if ((c1->mission->dst == c2->mission->dst && from1v2 < from1v1 && tov1 < tov2) || (nextv1 && nextv2 && try_to_flip(v1, v2, nextv1, nextv2))) {
/*     if (v1->id == 9 || v2->id == 9) { */
/*       printf("échange : %s %s %s %ld %s %s %s %ld %ld %ld\n", v1->name, c1->mission->src->name, c1->mission->dst->name, c1->step->init, v2->name, c2->mission->src->name, c2->mission->dst->name, c2->step->init, tov1, tov2); */
/*     } */
    exchange(c1->step->init, c2->step->init);
    exchange(c1->mission, c2->mission);
    return true;
  }
  return false;
}

static void flip_courses()
{
  bool flip = true;
  while (flip) {
    flip = false;
    FORCOUPLE(v1, pb->voitures, v2, pb->voitures) {
      FORCOUPLE(c1, v1->courses, c2, v2->courses) {
	if (c1->mission->src == c2->mission->src) {
	  Course *prev1 = (c1->id == 0 ? NULL : v1->courses[c1->id - 1]);
	  Course *prev2 = (c2->id == 0 ? NULL : v2->courses[c2->id - 1]);
	  TimeVal from1v1 = (prev1 ? prev1->step->init + compute_duration(prev1->mission, v1->courses[c1->id - 1]->step->init) : v1->start);
	  TimeVal from2v1 = c1->step->init;
	  TimeVal from1v2 = (prev2 ? prev2->step->init + compute_duration(prev2->mission, v2->courses[c2->id - 1]->step->init) : v2->start);
	  TimeVal from2v2 = c2->step->init;
	  if (from1v1 <= from1v2 && from2v1 > from2v2 && try_to_flip(v1, v2, c1, c2)) flip = true;
	}
      } EFORCOUPLE;
    } EFORCOUPLE;
  }
}

static void print_ratp_sol(SolutionPlan *p)
{
  qsort(p->steps, p->steps_nb, sizeof(Step *), precedes_in_plan_ratp);
  
  FOR(v, pb->voitures) { 
      cpt_malloc(v->courses, v->courses_nb);
      v->courses_nb = 0; 
  } EFOR;
  FOR(step, p->steps) {
    Voiture *v = pb->voitures[param_to_id(step, 0)];
    Terminus *src = pb->terminus[param_to_id(step, 3)];
    Terminus *dst = pb->terminus[param_to_id(step, 4)];
    Course *course = find_course(v, src, dst, v->courses_nb);
    course->id = v->courses_nb;
    v->courses[v->courses_nb++] = course;
    course->step = step;
    src->courses_nb++;
  } EFOR;

  flip_courses();

  TimeVal wait_inf[pb->terminus_nb];
  TimeVal wait_sup[pb->terminus_nb];
  FOR(t, pb->terminus) {
    cpt_malloc(t->courses, t->courses_nb);
    t->courses_nb = 0;
    wait_inf[t->id] = MAXTIME;
    wait_sup[t->id] = 0;
  } EFOR;
  FOR(v, pb->voitures) {
    FOR(c, v->courses) {
      Terminus *term = c->mission->src;
      term->courses[term->courses_nb++] = c;
    } EFOR;
  } EFOR;

  trace(ratp, "\nDéparts Voitures\n");
  trace(ratp, "================\n");
  FOR(v, pb->voitures) {
    trace(ratp, "\nvoiture %ld - %s [%" TIMEP "..%" TIMEP "]", v->id, v->name, v->start, v->end);
    TimeVal d2 = 0;
    FOR(c, v->courses) {
      trace(ratp, "\n\t%s->%s", c->mission->src->name, c->mission->dst->name);
      TimeVal d = compute_duration(c->mission, c->step->init);
      TimeVal wait = c->step->init - (c->id == 0 ? c->voiture->start : v->courses[c->id - 1]->step->init  + d2);
      if (c->id != 0) minimize(wait_inf[c->mission->src->id], wait);
      maximize(wait_sup[c->mission->src->id], wait);
      trace(ratp, " -- battement %4" TIMEP " ", wait);
      trace(ratp, " -- départ %5" TIMEP " -- arrivée %5" TIMEP " -- durée %5" TIMEP, c->step->init, c->step->init + d, d);
      d2 = d;
    } EFOR;
    trace(ratp, "\n");
  } EFOR;

  trace(ratp, "\nBattements Voitures\n");
  trace(ratp, "===================\n");
  FOR(t, pb->terminus) {
    trace(ratp, "\n%s : min %" TIMEP " -- max %" TIMEP "\n", t->name, wait_inf[t->id], wait_sup[t->id]);
  } EFOR;
  trace(ratp, "\n");

  trace(ratp, "\nDéparts Terminus\n");
  trace(ratp, "================\n");

  long nb_courses;
  TimeVal interval_inf[pb->terminus_nb];
  TimeVal interval_sup[pb->terminus_nb];
  TimeVal degradation = 0;
  FOR(t, pb->terminus) {
    nb_courses = 0;
    trace(ratp, "\n%s (%ld courses)", t->name, t->courses_nb);
    interval_inf[t->id] = MAXTIME;
    interval_sup[t->id] = 0;
    qsort(t->courses, t->courses_nb, sizeof(Course *), precedes_in_courses);
    FORi(c, i, t->courses) {
      if (c->step != NULL) {
	trace(ratp, "\n\tvoiture %3s -- départ %5" TIMEP, c->voiture->name, c->step->init);
	nb_courses++;
	if (i < t->courses_nb - 1) {
	  TimeVal interval = compute_interval(t, c->step->init);
	  TimeVal interv = t->courses[i + 1]->step->init - c->step->init;
	  minimize(interval_inf[t->id], interv);
	  maximize(interval_sup[t->id], interv);
	  trace(ratp, " -- intervalle %4" TIMEP " (spec : %" TIMEP ")", interv, interval);
	  if (interv > interval) {
	    trace(ratp, " --> diff : %" TIMEP, interv - interval);
	    degradation += interv - interval;
	  }
	}
      }
    } EFOR;
    trace(ratp, " -- spec : (%" TIMEP ", %" TIMEP ")\n", t->end_min, t->end_max);
  } EFOR;

  trace(ratp, "\nIntervalles Terminus\n");
  trace(ratp, "====================\n");
  FOR(t, pb->terminus) {
    trace(ratp, "\n%s : min %" TIMEP " -- max %" TIMEP " -- spec : ", t->name, interval_inf[t->id], interval_sup[t->id]);
    FOR(p, t->freq_tp) { trace(ratp, "(%" TIMEP ",%" TIMEP ") ", p.time, p.duration); } EFOR;
    trace(ratp, "\n");
  } EFOR;
  trace(ratp, "\n\nSomme des dégradations : %" TIMEP "\n", degradation);

  if (opt.ratp_output) {
    FILE *out = fopen(opt.ratp_output, "w");
    FOR(v, pb->voitures) {
      fprintf(out, "%ld ", v->courses_nb);
      FOR(c, v->courses) {
	fprintf(out,"%" TIMEP " %s %s ", c->step->init, c->mission->src->name, c->mission->dst->name);
      } EFOR;
      fprintf(out, "\n");
    } EFOR;
    fclose(out);
  }
}

TIMER(total);
TIMER(search);

/* static void modify_battement(TimeVal mod)  */
/* { */
/*   long i; */
/*   for (i = 0; i < pb->voitures_nb; i++) */
/*     start_action->ac_constraints[i*2+1]->min += mod; */
/*   FORMIN(a, actions, 2) { */
/*     a->ac_constraints[2]->min += 1; */
/*   } EFOR; */
/* } */

static void modify_interval(TimeVal mod)
{
  long i;

/*   for (i = 0; i < pb->terminus_nb; i++) */
/*     start_action->ac_constraints[i]->max += mod; */
  FORMIN(a, actions, 2) {
    a->ac_constraints[1]->max.t += mod; // nouveau
    a->ac_constraints[0]->min.t -= mod;
    for(i = 0; i < a->ac_constraints[0]->dur_nb; i++)
      a->ac_constraints[0]->dur[i].t += mod;
  } EFOR;
}

static void modify_tolerance(TimeVal mod)
{
  FORMIN(a, actions, 2) {
    a->ac_constraints[0]->min.t -= mod;
  } EFOR;
}

#define param_to_id2(action, n) atoi(action->parameters[n]->name+1)

static void modify_interval_mission(Mission *m, TimeVal mod)
{
  long i;
  FORMIN(a, actions, 2) {
    if (param_to_id2(a, 3) == m->src->id && param_to_id2(a, 4) == m->dst->id) {
      for(i = 0; i < a->ac_constraints[0]->dur_nb; i++)
	a->ac_constraints[0]->dur[i].t += mod;
    }
  } EFOR;
}

static void modify_interval_terminus(Terminus *t, TimeVal mod)
{
  //long i;
  FORMIN(a, actions, 2) {
    if (param_to_id2(a, 3) == t->id) {
      a->ac_constraints[1]->max.t += mod; // nouveau
/*       for(i = 0; i < a->ac_constraints[0]->dur_nb; i++) */
/* 	a->ac_constraints[0]->dur[i] += mod; */
    }
  } EFOR;
}

static void modify_tolerance_terminus(Terminus *t, TimeVal mod)
{
  long i;
  FORMIN(a, actions, 2) {
    if (param_to_id2(a, 3) == t->id) {
      for(i = 0; i < a->ac_constraints[0]->dur_nb; i++)
	a->ac_constraints[0]->min.t -= mod;
    }
  } EFOR;
}

static void modify_tolerance_mission(Mission *m, TimeVal mod)
{
  FORMIN(a, actions, 2) {
    if (param_to_id2(a, 3) == m->src->id && param_to_id2(a, 4) == m->dst->id)
      a->ac_constraints[0]->min.t -= mod;
  } EFOR;
}

int main(int argc, const char **argv)
{
  int res;
  TimeVal tol_incr = 0;
  TimeVal inter_incr = 0;

  cpt_main(argc, argv);

  res = cpt_basic_search();

  if (opt.ratp_incr > 0) {
    while (res != PLAN_FOUND) {
      modify_tolerance(-opt.ratp_incr);
      tol_incr += opt.ratp_incr;
/*       FOR(a, actions) { a->weight = 0; } EFOR; */
/*       FOR(c, causals) { c->weight = 0; } EFOR; */
      res = cpt_basic_search();
      if (res != PLAN_FOUND && (res != BACKTRACK_LIMIT || res == INIT_PROP_FAILED)) {
	if (opt.ratp_interval_incr == 0) break;
	modify_tolerance(tol_incr);
	tol_incr = 0;
	modify_interval(opt.ratp_interval_incr);
	inter_incr += opt.ratp_interval_incr;
	trace(ratp, "Degradation : %" TIMEP "\n", inter_incr);
	res = cpt_basic_search();
      }
    }
  }

  if (res != PLAN_FOUND) error(no_plan, "No plan found");

  trace(ratp, "Optimisation de la solution :\n");

  if (opt.ratp_interval_incr_optim > 0) {

    trace(ratp, "- tolérance globale");
    do {
      trace(ratp, ".");
      modify_tolerance(-opt.ratp_interval_incr_optim);
      res = cpt_basic_search();
    } while (res == PLAN_FOUND);
    modify_tolerance(opt.ratp_interval_incr_optim);

    trace(ratp, "\n- tolérance par terminus");
    FOR(t, pb->terminus) {
      do {
	trace(ratp, ".");
	modify_tolerance_terminus(t, -opt.ratp_interval_incr_optim);
	res = cpt_basic_search();
      } while (res == PLAN_FOUND);
      modify_tolerance_terminus(t, opt.ratp_interval_incr_optim);
    } EFOR;

    trace(ratp, "\n- intervalle par terminus");
    FOR(t, pb->terminus) {
      do {
	trace(ratp, ".");
	modify_interval_terminus(t, -opt.ratp_interval_incr_optim);
	res = cpt_basic_search();
      } while (res == PLAN_FOUND);
      modify_interval_terminus(t, opt.ratp_interval_incr_optim);
    } EFOR;

    trace(ratp, "\n- tolérance par mission");
    FOR(m, pb->missions) {
      do {
	trace(ratp, ".");
	modify_tolerance_mission(m, -opt.ratp_interval_incr_optim);
	res = cpt_basic_search();
      } while (res == PLAN_FOUND);
      modify_tolerance_mission(m, opt.ratp_interval_incr_optim);
    } EFOR;

    trace(ratp, "\n- intervalle par mission");
    FOR(m, pb->missions) {
      do {
	trace(ratp, ".");
	modify_interval_mission(m, -opt.ratp_interval_incr_optim);
	res = cpt_basic_search();
      } while (res == PLAN_FOUND);
      modify_interval_mission(m, opt.ratp_interval_incr_optim);
    } EFOR;
    printf("\n");
  }

  modify_interval(-inter_incr);

  trace_proc(solution_plan, solution_plan);
  trace_proc(search_stats, get_timer(search), get_timer(total));
  if (opt.ratp_input[0] != '\0') print_ratp_sol(solution_plan);
#ifndef WIN32
  display_graph();
#endif
  return res;
}

#ifndef WIN32

#include<X11/Xlib.h>

int xoffset;
int yoffset;
int xborder;
int screen_width;
int screen_height;
int win_width;
int win_height;

#define abs2coord(t) ((t) * (screen_width - xborder * 2) / pb->max_time_point + xoffset)

/* #define term2coord(t) ((t->id == 0 ? 600 : (t->id == 1 ? 450 : 50)) + yoffset) */
/* #define term2coord2(t) ((t->id == 0 ? 650 : (t->id == 1 ? 500 : 0)) + yoffset) */
/* #define term2coord3(t) ((t->id == 0 ? 620 : (t->id == 1 ? 470 : 30)) + yoffset) */

int ABS1[4] = {600, 450, 50};
int ABS2[4] = {620, 470, 30};
int ABS3[4] = {650, 500, 0};

int ABS4[4] = {600, 400, 250, 50};
int ABS5[4] = {620, 370, 280, 30};
int ABS6[4] = {650, 350, 300, 0};

#define term2coord(t) (pb->terminus_nb == 3 ? ABS1[t->id] : ABS4[t->id])
#define term2coord2(t) (pb->terminus_nb == 3 ? ABS2[t->id] : ABS5[t->id])
#define term2coord3(t) (pb->terminus_nb == 3 ? ABS3[t->id] : ABS6[t->id])

void draw_battement(XPoint *points, Terminus *t, Time t1, Time t2)
{
  points[0].x = points[1].x = abs2coord(t1);
  points[0].y = points[3].y = term2coord(t);
  points[1].y = term2coord2(t);
  points[2].x = points[3].x = abs2coord(t2);
  points[2].y = term2coord3(t);
}

static void change_colors(unsigned long int *colors, int colors_nb, unsigned long white)
{
  unsigned long color = white;
  int i;
  for (i = 0; i < colors_nb; i++) {
    color -= 300200300;
    colors[i] = color;
  }
}

void set_win_width(Display *d, Window w)
{
  Window root_return;
  int x_return, y_return;
  unsigned int width_return, height_return;
  unsigned int border_width_return;
  unsigned int depth_return;

  XGetGeometry(d, w, &root_return, &x_return, &y_return, &width_return, &height_return, &border_width_return, &depth_return);
  win_width = width_return;
}

static void draw_voitures(Display *d, Window w, GC gc, unsigned long int *colors)
{
  XClearWindow(d, w);
  FOR(v, pb->voitures) {
    if (v->courses_nb > 0) {
      int points_nb = (v->courses_nb + 1) * 4;
      XPoint points[points_nb];
      draw_battement(points, v->start_term, v->start, v->courses[0]->step->init);
      FOR(c, v->courses) {
	Time t1 = c->step->init + compute_duration(c->mission, c->step->init);
	Time t2 = (c->id == v->courses_nb - 1 ? v->end : v->courses[c->id + 1]->step->init);
	draw_battement(points + (c->id + 1) * 4, c->mission->dst, t1, t2);
      } EFOR;
      XSetForeground(d, gc, colors[v->id]);
      XDrawLines(d, w, gc, points, points_nb, CoordModeOrigin); 
    }
  } EFOR;
}

static void display_graph() 
{ 
  Display *d = XOpenDisplay(NULL);
  if (d == NULL) return;
  int s = DefaultScreen(d); 
  Screen *screen = DefaultScreenOfDisplay(d);
  Window w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, 100, 100, 1, WhitePixel(d, s), BlackPixel(d, s)); 
  GC gc = DefaultGC(d, s);
  XEvent xev, xev2; 

  win_width = screen_width = WidthOfScreen(screen);
  win_height = screen_height = HeightOfScreen(screen);
  XSelectInput(d, w, ExposureMask | KeyPressMask); 
  XMapWindow(d, w); 
  XSetLineAttributes(d, gc, 2, LineSolid, CapRound, 0);

  xev2.xclient.type=ClientMessage;
  xev2.xclient.serial = 0;
  xev2.xclient.send_event=True;
  xev2.xclient.window=w;
  xev2.xclient.message_type=XInternAtom(d, "_NET_WM_STATE", False); ;
  xev2.xclient.format=32;
  xev2.xclient.data.l[0] = 1;
  xev2.xclient.data.l[1] = XInternAtom(d, "_NET_WM_STATE_FULLSCREEN", False); ;
  xev2.xclient.data.l[2] = 0;
  XSendEvent(d, DefaultRootWindow(d), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev2); 

  unsigned long int colors[pb->voitures_nb];
  change_colors(colors, pb->voitures_nb, WhitePixel(d, s));

  xoffset = 50;
  xborder = 50;
  yoffset = 50;
  int accel = 50;
  bool redraw = true;

  do { 
    XNextEvent(d, &xev); 
    redraw = false;
    if (xev.type == Expose) redraw = true;
    if (xev.type == KeyPress) {
      switch (xev.xkey.keycode) {
      case 54: 
	change_colors(colors, pb->voitures_nb, colors[pb->voitures_nb - 1]);
	redraw = true;
	break;
      case 41: 
	xev2.xclient.data.l[0] = 2;
	XSendEvent(d, DefaultRootWindow(d), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev2); 
	break;
      case 111: // top
	screen_width += accel; 
	set_win_width(d, w);
	xoffset -= ceil(accel * (win_width / 2-xoffset) / (double)screen_width); 
	redraw = true;
	break;
      case 116: // bottom
	screen_width -= accel; 
	set_win_width(d, w);
	xoffset += ceil(accel * (win_width / 2-xoffset) / (double)screen_width); 
	redraw = true;
	break;
      case 113: // right
	xoffset -= accel; 
	redraw = true;
	break;
      case 114: //left
	xoffset += accel; 
	redraw = true;
	break;
      }
    }
    if (redraw) draw_voitures(d, w, gc, colors); 
    //if (xev.type == KeyPress) printf("%d\n", xev.xkey.keycode);
  } while (xev.type != KeyPress || xev.xkey.keycode != 38);
  XUnmapWindow(d, w);
  XDestroyWindow(d, w);
  XCloseDisplay(d);
}

#endif
