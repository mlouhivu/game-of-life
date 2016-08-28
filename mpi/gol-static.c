/**************************************************************************
 * Game of Life (v1.0)
 *   An example implementation of Conway's Game of Life in C using 
 *   static data types.
 *
 * files:
 *   gol-static.c     -- main program
 *   gol-static.h     -- common header file
 *   gol-static-io.c  -- I/O routines
 *
 * usage:
 *   mpicc gol-static.c -o gol-static
 *
 *   mpirun -np X ./gol-static <output-prefix> N M STEPS
 *
 * author: Martti Louhivuori (martti.louhivuori@csc.fi)
 * date:   16.05.2011
 *
 * update to parallel version with 2D decomposition:
 * Jussi Enkovaara (jussi.enkovaara@csc.fi)
 * date:   17.05.2011
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include "gol-static.h"
#include "gol-static-io.c"
#include "gol-parallel.c"

/*
 * Advance the game by one time-step.
 *   prev  --  (state) current generation
 * -> (state) next generation
 */
state update(state prev) {
	int i, j, n;
	state next;

	/* init next generation from previous */
	next.rows = prev.rows;
	next.cols = prev.cols;
	next.generation = prev.generation + 1;
	/* all DEAD by default */
	next.count = 0;
	for (i=0; i < next.rows + 2; i++) 
		for (j=0; j < next.cols + 2; j++) 
			next.space[i][j] = DEAD;

	/* check for ALIVE cells */
	for (i=1; i <= prev.rows; i++) {
		for (j=1; j <= prev.cols; j++) {
			n =   prev.space[i-1][j-1] + prev.space[i-1][j] 
				+ prev.space[i-1][j+1] + prev.space[i][j-1] 
				+ prev.space[i][j+1]   + prev.space[i+1][j-1] 
				+ prev.space[i+1][j]   + prev.space[i+1][j+1];
			if ( (n == 3) || (n == 2 && prev.space[i][j]) ) {
				next.space[i][j] = ALIVE;
				next.count++;
			} else {
				next.space[i][j] = DEAD;
			}
		}
	}
	/* return next generation */
	return next;
}

/*
 * Create a random state.
 *   n  --  (int) no. of columns in the grid
 *   m  --  (int) no. of rows in the grid
 * -> (state) random generation
 */
state create(int n, int m) {
	state world;
	int i, j;
	unsigned int seed;
	
	seed = (unsigned int) time(NULL);
	srand(seed);

	for (i=0; i < n+2; i++) 
		for (j=0; j< m+2; j++) 
			world.space[i][j] = DEAD;

	world.cols = n;
	world.rows = m;
	world.generation = 0;
	for (i=1; i < m+1; i++)
		for (j=1; j < n+1; j++) 
			world.space[i][j] = (int) (rand() % 2);

	return world;
}

/* 
 * Game of Life.
 *   Load an initial state, run for a number of iterations and save the 
 *   end state.
 */
void main(int argc, char *argv[]) {
	state world;
	int i, j, n=-1, m=-1, steps=-1;
	char output[64];

	/* Parallel variables */
	int my_id, ntasks;
	int Px, Py, nloc, mloc, nx, ny;
	parallel_state parallel;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

	/* Determine the 2D process grid */
	Px = (int) sqrt(ntasks);
	Py = ntasks / Px;
	assert(Px*Py == ntasks);

	/* check command line arguments */
	if (argc < 4) {
		fprintf(stderr, "Usage: ./gol-static <output-prefix> <saved-state> STEPS\n");
		fprintf(stderr, "         OR\n");
		fprintf(stderr, "       ./gol-static <output-prefix> N M STEPS\n\n");
		fprintf(stderr, "Example: ./gol-static my-world initial-world 64\n");
		fprintf(stderr, "         ./gol-static my-world 100 100 64\n");
		exit(ARGERR);
	} else if (argc == 4) {
 	        printf("Mode not supported in parallel code\n");
		exit(-1);
		/* load an initial state */
		world = load(argv[2]);
		sscanf(argv[3], "%d", &steps);
		n = m = 0;
	} else {
                /* create a random initial state */
                sscanf(argv[2], "%d", &n); 
                sscanf(argv[3], "%d", &m);
                nloc = n / Px;
                mloc = m / Py;
                assert(nloc * Px == n);
		assert(mloc * Py == m);
		world = create(nloc, mloc);
		sscanf(argv[4], "%d", &steps);
	}
	/* did one of the scans fail? */
	if (n < 0 || m < 0 || steps < 0) {
		fprintf(stderr, "ERROR: invalid command line arguments\n");
		exit(SCANERR);
	}

	/* determine neighbouring cells etc. for parallel case */
	parallel = decompose(my_id, ntasks, Px, Py, nloc, mloc);

	/* save initial state */
	if (my_id == 0) {
	        sprintf(output, "%s-%04d", argv[1], 0);
		save(output, world);
	}

	/* iterate for STEPS generations */
	for (i=0; i < steps; i++) {
	        world = exchange(world, parallel);
		world = update(world);
		/* save current state */
		sprintf(output, "%s-%04d", argv[1], i+1);
		save(output, world);
	}

	MPI_Finalize();
}

