/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : main.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */

#include "cpt.h"
#include "structs.h"
#include "solve.h"
#ifdef YAHSP_MPI
#include "plan.h"
#include "yahsp.h"
#include "yahsp-mpi.h"
#endif
int main(int argc, const char **argv)
{
#ifdef YAHSP_MPI
  mpi_init(argc, argv);
#endif
  int return_code = cpt_main(argc, argv);
#ifdef YAHSP_MPI
  mpi_finalize();
#endif
  return return_code;
}
