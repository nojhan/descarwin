/*
 * CPT - a Constraint Programming Temporal planner
 *
 * File : yahsp-mpi.c
 *
 * Copyright (C) 2005-2011  Vincent Vidal <Vincent.Vidal@onera.fr>
 */


#include "cpt.h"
#include "options.h"
#include "trace.h"
#include "structs.h"
#include "globs.h"
#include "mpi.h"
#include "yahsp.h"
#include "yahsp-common.h"
#include "yahsp-mpi.h"


#define REQ_NODE 0     /* master requests a node to another process */
#define REQ_STEPS 1    /* master requests steps of a node to another process */
#define EXCH 2         /* exchange a node between two processes  */

#define COMM MPI_COMM_WORLD
#define NO_STATUS MPI_STATUS_IGNORE
#define BYTE MPI_BYTE
#define INT MPI_INT
#define LONG MPI_LONG
#define LONG_INT MPI_LONG_INT
#define DOUBLE MPI_DOUBLE
#define ANY_SOURCE MPI_ANY_SOURCE
#define PACKED MPI_PACKED

static struct {
  VECTOR(Node*, nodes);
  size_t to_send;
} *cached_nodes;

static const int master = 0;
static int numtasks;
static int rank;
static int mpi_state_size;
static int msg_node_size;

static const size_t cached_nodes_max = 100;
static size_t cached_nodes_read;
static size_t cached_nodes_to_read;
static int cached_nodes_position;

static char *send_buffer;
static char *recv_buffer;
static int buffer_size;
static MPI_Request send_request;

static const size_t cached_steps_incr = 1;
SVECTOR(YStep *, cached_steps);
static size_t cached_steps_max;

static TimeVal mpi_best_makespan = MAXTIME;


#ifdef YAHSP_MT
static omp_lock_t mpi_lock;
#pragma omp threadprivate(send_buffer, send_request, recv_buffer, cached_nodes_read, cached_nodes_to_read, cached_nodes_position, cached_steps, cached_steps_nb, cached_steps_max, cached_steps, cached_nodes)
#endif


void mpi_init(int argc, const char **argv)
{
#ifdef YAHSP_MT
  MPI_Init_thread(&argc,(char ***) &argv, MPI_THREAD_MULTIPLE, NULL);
  omp_init_lock(&mpi_lock);
#else
  MPI_Init(&argc,(char ***) &argv);
#endif
  MPI_Comm_size(COMM, &numtasks);
  MPI_Comm_rank(COMM, &rank);
}

void mpi_finalize()
{ 
#ifndef SCC
  MPI_Barrier(COMM); exit(0);
#endif
  MPI_Finalize(); 
}

void mpi_abort()
{ 
  MPI_Abort(COMM, 0);
}

int mpi_get_numtasks()
{ return numtasks; }

int mpi_get_rank()
{ return rank; }

bool mpi_is_master()
{ return rank == master; }

void mpi_barrier()
{
  MPI_Barrier(COMM);
}

void mpi_yahsp_init()
{
  int i;
  cpt_calloc(cached_nodes, numtasks);
  for(i = 0; i < numtasks; i++) {
    cpt_malloc(cached_nodes[i].nodes, cached_nodes_max);
    cached_nodes[i].to_send = 1;
  }
  for(i = 0; i < numtasks; i++) 
  OMP_SINGLE {
    mpi_state_size = ((fluents_nb - 1) / WORDSIZE + 1) * sizeof(long); 
    msg_node_size = sizeof(Node) + mpi_state_size;
    mpi_best_makespan = MAXTIME;
    buffer_size = sizeof(long) +  msg_node_size * cached_nodes_max;
  }
  cpt_malloc(send_buffer, buffer_size);
  cpt_malloc(recv_buffer, buffer_size);
}

void mpi_yahsp_cleanup()
{
  OMP_BARRIER;
  cpt_free(send_buffer);
  cpt_free(recv_buffer);
  FOR(s, cached_steps) { cpt_free(s); } EFOR;
  cached_steps_nb = 0;
  int i;
  for (i = 0; i < numtasks; i++) {
    FOR(node, cached_nodes[i].nodes) { node_free(node); } EFOR;
    cpt_free(cached_nodes[i].nodes);
  }
  cpt_free(cached_nodes);
}

static void send_buffer_wait_request()
{
  if (cached_steps_nb != 0) {
    OMP(omp_set_lock(&mpi_lock));
    MPI_Wait(&send_request, NO_STATUS);
    OMP(omp_unset_lock(&mpi_lock));
  }
}

static void send_buffer_send(int size, int dest, int tag)
{
  OMP(omp_set_lock(&mpi_lock));
  OMP_ATOMIC stats.data_sent += size;
  MPI_Isend(send_buffer, size, PACKED, dest, tag, COMM, &send_request);
  OMP(omp_unset_lock(&mpi_lock));
}

static void send_buffer_pack_node(Node *node, int *position)
{
  MPI_Pack(node, sizeof(Node), BYTE, send_buffer, buffer_size, position, COMM);
  MPI_Pack(node->state, mpi_state_size, BYTE, send_buffer, buffer_size, position, COMM);
}

static Node *recv_buffer_unpack_node(int *position)
{
  Node *node = cpt_malloc(node, 1);
  MPI_Unpack(recv_buffer, buffer_size, position, node, sizeof(Node), BYTE, COMM);
  cpt_malloc(node->state, mpi_state_size);
  MPI_Unpack(recv_buffer, buffer_size, position, node->state, mpi_state_size, BYTE, COMM);
  return node;
}

static void cached_nodes_send(int dest)
{
  int position = 0;
  send_buffer_wait_request();
  MPI_Pack(&cached_nodes[dest].nodes_nb, 1, LONG, send_buffer, buffer_size, &position, COMM);
  FOR(node, cached_nodes[dest].nodes) {
    if (cached_steps_nb == cached_steps_max) cpt_realloc(cached_steps, cached_steps_max += cached_steps_incr);
    cached_steps[cached_steps_nb++] = node->steps;
    send_buffer_pack_node(node, &position);
    node->steps = NULL;
    node_free(node);
  } EFOR;
  send_buffer_send(sizeof(long) + cached_nodes[dest].nodes_nb * msg_node_size, dest, EXCH);
  cached_nodes[dest].nodes_nb = 0;
  OMP_ATOMIC stats.sends++;
}

static void cached_nodes_add(Node *node, int dest)
{
  cached_nodes[dest].nodes[cached_nodes[dest].nodes_nb++] = node;
  if (cached_nodes[dest].nodes_nb == cached_nodes[dest].to_send) {
    cached_nodes_send(dest);
    cached_nodes[dest].to_send = mini(cached_nodes_max, cached_nodes[dest].to_send + 1);
  }
}

void mpi_flush_cached_nodes()
{
  int best_dest = 0;
  int dest;
  for (dest = 1; dest < numtasks; dest++)
    if (cached_nodes[dest].nodes_nb > cached_nodes[best_dest].nodes_nb)
      best_dest = dest;
  if (cached_nodes[best_dest].nodes_nb > 0)
    cached_nodes_send(best_dest);
}

Node *mpi_get_cached_node()
{
  int dest; 
  for (dest = 0; dest < mpi_get_numtasks(); dest++) {
    if (cached_nodes[dest].nodes_nb != 0)
      return cached_nodes[dest].nodes[--cached_nodes[dest].nodes_nb];
  }
  return NULL;
}

static void mpi_send_node_dest(Node *node, int dest, int tag)
{
  int position = 0;
  send_buffer_wait_request();
  send_buffer_pack_node(node, &position);
  send_buffer_send(msg_node_size, dest, tag);
}

bool mpi_send_node(Node *node, int mpi_dest)
{
  if (mpi_dest == rank) return false;
  node->fvalue -= 10000000;
  cached_nodes_add(node, mpi_dest);
  return true;
}

static bool recv_buffer_recv(int tag, bool *synchro_required)
{
  int flag = 0;
  MPI_Status status;
  int size = 0;
  if (synchro_required != NULL) *synchro_required = false;
  //OMP(omp_set_lock(&mpi_lock)); {
  OMP(if (omp_test_lock(&mpi_lock))) {
    MPI_Iprobe(ANY_SOURCE, tag, COMM, &flag, &status);
    if (flag == 1) {
      MPI_Get_count(&status, BYTE, &size);
      if (size == 0) *synchro_required = true;
      else {
	MPI_Recv(recv_buffer, size, PACKED, ANY_SOURCE, tag, COMM, NO_STATUS);
      }
    }
    OMP(omp_unset_lock(&mpi_lock));
  }
  return flag != 0 && size != 0;
}

Node *mpi_recv_node(bool *synchro_required)
{
  if (cached_nodes_read < cached_nodes_to_read) {
    cached_nodes_read++;
    return recv_buffer_unpack_node(&cached_nodes_position);
  } else if (recv_buffer_recv(EXCH, synchro_required)) {
    cached_nodes_read = 0;
    cached_nodes_position = 0;
    MPI_Unpack(recv_buffer, buffer_size, &cached_nodes_position, &cached_nodes_to_read, 1, LONG, COMM);
  }
  return NULL;
}
  
void mpi_call_synchronize()
{
  MPI_Request req;
  int dest;
  for (dest = 0; dest < numtasks; dest++)
    MPI_Isend(NULL, 0, BYTE, dest, EXCH, COMM, &req);
}

static Node *node_copy(Node *node)
{
  Node *n = cpt_malloc(n, 1);
  *n = *node;
  state_clone(n->state, node->state);
  return n;
}

static Node *gather_node(Node *node, int rank_node)
{
  int position = 0;
  size_t i;
  if (rank_node == rank) node = node_copy(node);
  else {
    MPI_Send(&node, sizeof(Node *), BYTE, rank_node, REQ_NODE, COMM);
    MPI_Probe(rank_node, REQ_NODE, COMM, NO_STATUS);
    recv_buffer_recv(REQ_NODE, NULL);
    node = recv_buffer_unpack_node(&position);
  }
  if (node->parent != NULL) {
    if (node->rank == rank) {
      VECTOR(YStep, steps);
      vector_copy(steps, node->steps);
      node->steps = steps;
    } else {
      MPI_Send(&node->steps, sizeof(YStep *), BYTE, node->rank, REQ_STEPS, COMM);
      MPI_Send(&node->steps_nb, 1, LONG, node->rank, REQ_STEPS, COMM);
      cpt_malloc(node->steps, node->steps_nb);
      MPI_Recv(node->steps, node->steps_nb * sizeof(YStep), BYTE, node->rank, REQ_STEPS, COMM, NO_STATUS);
    }
  }
  node->applicable = NULL;
  for (i = 0; i < node->steps_nb; i++) 
    node->steps[i].action = actions[(long) node->steps[i].action];
  return node;
}

Node *mpi_synchronize_solution(Node *node, TimeVal *best_makespan, bool *stop_on_timer)
{
  Node *root = NULL;
  OMP_BARRIER;
  OMP_MASTER {
    MPI_Recv(NULL, 0, BYTE, ANY_SOURCE, EXCH, COMM, NO_STATUS);
    // problem here for int64_t on 32 bits machines
    //struct { TimeVal makespan; int rank; } in = {(node == NULL ? MAXTIME : node->makespan), rank}, out;
    struct { long makespan; int rank; } in = {(node == NULL ? LONG_MAX : node->makespan), rank}, out;
    MPI_Allreduce(&in, &out, 1, LONG_INT, MPI_MINLOC, COMM);
    int stop = *stop_on_timer;
    MPI_Allreduce(&stop, stop_on_timer, 1, INT, MPI_LOR, COMM);
    if (out.makespan < mpi_best_makespan)  {
      mpi_best_makespan = out.makespan;
      if (!opt.yahsp_duplicate_search) *best_makespan = mpi_best_makespan;
      MPI_Bcast(&node, 1, LONG, out.rank, COMM);
      if (node == NULL) exit(55);
      if (rank == master) {
	int dest;
	node = root = gather_node(node, out.rank);
	while (node->parent != NULL)
	  node = node->parent = gather_node(node->parent, node->rank);
	for (dest = 0; dest < numtasks; dest++)
	  if (dest != master) MPI_Send(NULL, 0, BYTE, dest, REQ_NODE, COMM);
      } else {
	int size = -1;
	while(size != 0) {
	  MPI_Status status;
	  int flag;
	  MPI_Iprobe(master, REQ_NODE, COMM, &flag, &status);
	  if (flag != 0) {
	    MPI_Get_count(&status, BYTE, &size);
	    if (size != 0) {
	      MPI_Recv(&node, sizeof(Node *), BYTE, master, REQ_NODE, COMM, NO_STATUS);
	      mpi_send_node_dest(node, master, REQ_NODE);
	    }
	  }
	  MPI_Iprobe(master, REQ_STEPS, COMM, &flag, &status);
	  if (flag != 0) {
	    VECTOR(YStep, steps);
	    MPI_Recv(&steps, sizeof(YStep *), BYTE, master, REQ_STEPS, COMM, NO_STATUS);
	    MPI_Recv(&steps_nb, 1, LONG, master, REQ_STEPS, COMM, NO_STATUS);
	    MPI_Send(steps, steps_nb * sizeof(YStep), BYTE, master, REQ_STEPS, COMM);
	  }
	}
	MPI_Recv(NULL, 0, BYTE, master, REQ_NODE, COMM, NO_STATUS);
      }
    }
  }
  OMP_BARRIER;
  return root;
}

void mpi_compute_statistics()
{
  long x;
  MPI_Reduce(&stats.computed_nodes, &x, 1, LONG, MPI_SUM, master, COMM);
  stats.computed_nodes = x;
  MPI_Reduce(&stats.evaluated_nodes, &stats.min_eval_proc, 1, LONG, MPI_MIN, master, COMM);
  MPI_Reduce(&stats.evaluated_nodes, &stats.max_eval_proc, 1, LONG, MPI_MAX, master, COMM);
#ifdef YAHSP_MT
  MPI_Reduce(&stats.min_eval_thread, &x, 1, LONG, MPI_MIN, master, COMM);
  stats.min_eval_thread = x;
  MPI_Reduce(&stats.max_eval_thread, &x, 1, LONG, MPI_MAX, master, COMM);
  stats.max_eval_thread = x;
#endif
  MPI_Reduce(&stats.evaluated_nodes, &x, 1, LONG, MPI_SUM, master, COMM);
  stats.evaluated_nodes = x;
  MPI_Reduce(&stats.expanded_nodes, &x, 1, LONG, MPI_SUM, master, COMM);
  stats.expanded_nodes = x;

  double t;

  t = stats.wsearch;
  MPI_Reduce(&t, &stats.wsearch, 1, DOUBLE, MPI_MAX, master, COMM);

  stats.nodes_by_sec = stats.evaluated_nodes / stats.wsearch;
  stats.nodes_by_sec_proc = stats.nodes_by_sec / (numtasks * opt.yahsp_threads * opt.yahsp_teams);
  MPI_Reduce(&stats.sends, &x, 1, LONG, MPI_SUM, master, COMM);
  stats.sends = x;
  MPI_Reduce(&stats.data_sent, &x, 1, LONG, MPI_SUM, master, COMM);
  stats.data_sent = x;

  t = get_timer(stats.search);
  MPI_Reduce(&t, &stats.usearch, 1, DOUBLE, MPI_SUM, master, COMM);
  t = get_timer(stats.total);
  MPI_Reduce(&t, &stats.utotal, 1, DOUBLE, MPI_SUM, master, COMM);

}
