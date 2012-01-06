/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : yahsp-mpi.h
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#ifndef YAHSP_MPI_H
#define YAHSP_MPI_H


extern void mpi_init(int argc, const char **argv);
extern void mpi_finalize();
extern int mpi_get_numtasks();
extern int mpi_get_rank();
extern bool mpi_is_master();
extern void mpi_barrier();
extern void mpi_abort();
extern void mpi_yahsp_init();
extern void mpi_yahsp_cleanup();
extern bool mpi_send_node(Node *n, int mpi_dest);
extern void mpi_flush_cached_nodes();
extern Node *mpi_get_cached_node();
extern Node *mpi_recv_node(bool *synchro_required);
extern void mpi_call_synchronize();
extern Node *mpi_synchronize_solution(Node *solution_node, TimeVal *best_makespan, bool *stop_on_timer);
extern void mpi_compute_statistics();

#endif /* YAHSP_MPI_H */
