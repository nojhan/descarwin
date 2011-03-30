/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : ratp_struct.h
 *
 * Copyright (C) 2005-2011, 2007  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#ifndef RATP_STRUCTS_H
#define RATP_STRUCTS_H


typedef struct RatpProblem RatpProblem;
typedef struct Terminus Terminus;
typedef struct Voiture Voiture;
typedef struct Mission Mission; 
typedef struct Traject Traject;
typedef struct Course Course;
typedef struct TimePoint TimePoint;


struct RatpProblem {
  VECTOR(Terminus *, terminus);
  VECTOR(Mission *, missions);
  VECTOR(Voiture *, voitures);
  TimeVal bound;
  TimeVal max_time_point;
};


struct Terminus {
  long id;
  char *name;
  TimeVal start_min;
  TimeVal start_max;
  TimeVal end_min;
  TimeVal end_max;
  TimeVal freq_tol;
  TimeVal wait_min;
  TimeVal wait_max;
  VECTOR(Course *, courses);
  VECTOR(TimePoint, freq_tp);
};

struct Voiture {
  long id;
  char *name;
  TimeVal start;
  TimeVal end;
  Terminus *start_term;
  Terminus *final_term;
  VECTOR(Traject *, trajects);
  VECTOR(Course *, courses);
};

struct Mission {
  long id;
  Terminus *src;
  Terminus *dst;
  VECTOR(TimePoint, duration_tp);
  VECTOR(TimeVal, windows);
};

struct Traject {
  VECTOR(Course *, courses);
};

struct Course {
  long id;
  Voiture *voiture;
  Mission *mission;
  Step *step;
  TimeVal min_start;
};

struct TimePoint {
  TimeVal time;
  TimeVal duration;
};


#endif /* RATP_STRUCTS_H */
