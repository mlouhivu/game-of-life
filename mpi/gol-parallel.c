/**************************************************************************
 * Game of Life (v1.0)
 *   An example implementation of Conway's Game of Life in C using
 *   static data types.
 *
 * Routines for parallel calculations
 * author: Jussi Enkovaara (jussi.enkovaara@csc.fi)
 * date:   17.05.2011
 **************************************************************************/
#include <stdlib.h>
#include "gol-static.h"
#include <mpi.h>

typedef struct {
        int   nbr_left;
        int   nbr_right;
        int   nbr_up;
        int   nbr_down;
        MPI_Datatype rowtype;
        MPI_Datatype coltype;
        MPI_Comm cart_comm;
} parallel_state;

parallel_state decompose(int my_id, int ntasks,  int Px, int Py,
			 int nloc, int mloc)
{
  parallel_state parallel;
  int dims[2] = {Px, Py};
  int periods[2] = {0, 0};

  /* Cartesian process grid */
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &parallel.cart_comm);
  MPI_Cart_shift(parallel.cart_comm, 0, 0,
		 &parallel.nbr_left, &parallel.nbr_right);
  MPI_Cart_shift(parallel.cart_comm, 1, 0,
		 &parallel.nbr_up, &parallel.nbr_down);

  /* Datatypes for boundaries */
  MPI_Type_vector(nloc, 1, MAXCOLS+2, MPI_SHORT, &parallel.coltype);
  MPI_Type_commit(&parallel.coltype);
  MPI_Type_contiguous(mloc, MPI_SHORT, &parallel.rowtype);
  MPI_Type_commit(&parallel.rowtype);

  return parallel;
}

state exchange(state world, parallel_state parallel)
{
  int tag_l=0, tag_r=1, tag_u=2, tag_d=3;
  int i;

  /* Communicate in y-direction */
  MPI_Sendrecv(&world.space[world.rows][1], 1, parallel.rowtype,
	       parallel.nbr_up, tag_u, &world.space[0][1], 1,
	       parallel.rowtype, parallel.nbr_down, tag_u,
	       MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  MPI_Sendrecv(&world.space[1][1], 1, parallel.rowtype,
	       parallel.nbr_down, tag_d, &world.space[world.rows+1][1], 1,
	       parallel.rowtype, parallel.nbr_up, tag_d,
	       MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  /* Communicate in x-direction */
  MPI_Sendrecv(&world.space[1][1], 1, parallel.coltype,
	       parallel.nbr_left, tag_l, &world.space[1][world.cols+1], 1,
	       parallel.coltype, parallel.nbr_right, tag_l,
	       MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  MPI_Sendrecv(&world.space[1][world.cols], 1, parallel.coltype,
	       parallel.nbr_right, tag_r, &world.space[1][0], 1,
	       parallel.coltype, parallel.nbr_left, tag_r,
	       MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  return world;
}
