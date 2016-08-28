/**************************************************************************
 * Game of Life (v1.0)
 *   An example implementation of Conway's Game of Life in C.
 *
 * files:
 *   gol.c     -- main program
 *   gol.h     -- common header file
 *   gol-io.c  -- I/O routines
 *
 * usage:
 *   gcc gol.c -o gol
 *   ./gol <output-prefix> <saved-state> STEPS
 *     OR
 *   ./gol <output-prefix> N M STEPS
 *
 * author: Martti Louhivuori (martti.louhivuori@csc.fi)
 * date:   17.05.2011
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gol.h"
#include "gol-io.c"

/*
 * Advance the game by one time-step.
 *   prev  --  (state *) previous generation
 *   next  --  (state *) next generation
 */
void update(state *prev, state *next) {
	int i, j, n;

	/* init next generation from previous */
	next->rows = prev->rows;
	next->cols = prev->cols;
	next->generation = prev->generation + 1;
	/* all DEAD by default */
	next->count = 0;
	/* clear the borders */
	for (i=0; i < next->rows + 2; i++) {
		next->space[i][0] = DEAD;
		next->space[i][next->cols+1] = DEAD;
	}
	for (j=1; j < next->cols + 1; j++) {
		next->space[0][j] = DEAD;
		next->space[next->rows+1][j] = DEAD;
	}

	/* check for ALIVE cells */
	for (i=1; i <= prev->rows; i++) {
		for (j=1; j <= prev->cols; j++) {
			n =   prev->space[i-1][j-1] + prev->space[i-1][j]
				+ prev->space[i-1][j+1] + prev->space[i][j-1]
				+ prev->space[i][j+1]   + prev->space[i+1][j-1]
				+ prev->space[i+1][j]   + prev->space[i+1][j+1];
			if ( (n == 3) || (n == 2 && prev->space[i][j]) ) {
				next->space[i][j] = ALIVE;
				next->count++;
			} else {
				next->space[i][j] = DEAD;
			}
		}
	}
}

/*
 * Create a random state.
 *   world  --  (state *) new random state
 *   n      --  (int)     no. of rows in the grid
 *   m      --  (int)     no. of columns in the grid
 */
void create(state *world, int n, int m) {
	int i, j;
	unsigned int seed;

	/* init pseudo-random number generator */
	seed = (unsigned int) time(NULL);
	srand(seed);

	/* initialise metadata */
	world->rows = n;
	world->cols = m;
	world->generation = 0;
	/* allocate memory for target */
	world->space = malloc((world->rows + 2) * sizeof(short *));
	world->space[0] = malloc((world->rows + 2) *
			(world->cols + 2) * sizeof(short));
	for (i=1; i < world->rows + 2; i++)
		world->space[i] = world->space[0] + i * (world->cols + 2);
	/* clear the borders */
	for (i=0; i < n+2; i++) {
		world->space[i][0] = DEAD;
		world->space[i][m+1] = DEAD;
	}
	for (j=1; j < m+1; j++) {
		world->space[0][j] = DEAD;
		world->space[n+1][j] = DEAD;
	}
	/* initialise the grid */
	for (i=1; i < n+1; i++)
		for (j=1; j < m+1; j++)
			world->space[i][j] = (int) (rand() % 2);
}

/*
 * Clone one state to another.
 *   source  --  (state *) original state
 *   target  --  (state *) duplicate state
 */
void clone(state *source, state *target) {
	int i, j;

	/* copy metadata */
	target->cols = source->cols;
	target->rows = source->rows;
	target->generation = source->generation;
	/* allocate memory for target */
	target->space = malloc((target->rows + 2) * sizeof(short *));
	target->space[0] = malloc((target->rows + 2) *
			(target->cols + 2) * sizeof(short));
	for (i=1; i < target->rows + 2; i++)
		target->space[i] = target->space[0] + i * (target->cols + 2);
	/* copy cell grid */
	for (i=0; i < target->rows + 2; i++)
		for (j=0; j < target->cols + 2; j++)
			target->space[i][j] = source->space[i][j];
}

/*
 * Game of Life.
 *   Load (or create) an initial state, run for a number of iterations
 *   and save a time-line of the system's evolution.
 */
void main(int argc, char *argv[]) {
	state world_a, world_b;
	state *prev, *next, *swap;
	int i, j, n=-1, m=-1, steps=-1;
	char output[64];

	/* check command line arguments */
	if (argc < 4) {
		fprintf(stderr, "Usage: ./gol <output-prefix> <saved-state> STEPS\n");
		fprintf(stderr, "         OR\n");
		fprintf(stderr, "       ./gol <output-prefix> N M STEPS\n\n");
		fprintf(stderr, "Example: ./gol my-world initial-world 64\n");
		fprintf(stderr, "         ./gol my-world 100 100 64\n");
		exit(ARGERR);
	} else if (argc == 4) {
		/* load an initial state */
		load(&world_a, argv[2]);
		sscanf(argv[3], "%d", &steps);
		n = m = 0;
	} else {
		/* parse metadata */
		sscanf(argv[2], "%d", &n);
		sscanf(argv[3], "%d", &m);
		sscanf(argv[4], "%d", &steps);
		/* check array dimensions */
		if ( (m < 1 || m > MAXROWS) || (n < 1 || n > MAXCOLS) ) {
			fprintf(stderr,
					"ERROR: grid dimensions too large (max. %d x %d)\n",
					MAXCOLS, MAXROWS);
			exit(GRIDERR);
		}
		/* create a random initial state */
		create(&world_a, n, m);
	}
	/* did one of the scans fail? */
	if (n < 0 || m < 0 || steps < 0) {
		fprintf(stderr, "ERROR: invalid command line arguments\n");
		exit(SCANERR);
	}
	/* init world B based on world A */
	clone(&world_a, &world_b);
	/* set initial state pointers */
	prev = &world_a;
	next = &world_b;

	/* save initial state */
	sprintf(output, "%s-%04d", argv[1], 0);
	save(output, prev);
	/* iterate for STEPS generations */
	for (i=0; i < steps; i++) {
		update(prev, next);
		/* swap state pointers */
		swap = prev;
		prev = next;
		next = swap;
		/* save current state */
		sprintf(output, "%s-%04d", argv[1], i+1);
		save(output, prev);
	}

	/* de-allocate memory */
	free((void *) world_a.space[0]);
	free((void *) world_a.space);
	free((void *) world_b.space[0]);
	free((void *) world_b.space);
}

