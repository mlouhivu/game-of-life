/**************************************************************************
 * Game of Life (v1.0)
 *   An example implementation of Conway's Game of Life in C.
 *
 * author: Martti Louhivuori (martti.louhivuori@csc.fi)
 * date:   17.05.2011
 **************************************************************************/
#include <stdlib.h>
#include "gol.h"

/*
 * Load a saved GoL state.
 *   world     --  (state *) saved GoL state
 *   filename  --  (string)  name of a file containing a saved GoL state
 */
void load(state *world, char filename[]) {
	FILE *fp;
	int rows, cols, generation;
	int err, i, j, x;

	/* open file for reading */
	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "ERROR: can't open file '%s'\n", filename);
		exit(IOERR);
	}
	/* read and check meta data */
	err = fscanf(fp, "P1\n# generation=%d  alive=%d\n%d %d\n", 
			&(world->generation), &(world->count), 
			&(world->cols), &(world->rows));
	if (err != 4) {
		fprintf(stderr, "ERROR: invalid header in '%s'\n", filename);
		exit(METAERR);
	}
	if ( (world->rows < 1 || world->rows > MAXROWS) 
			|| (world->cols < 1 || world->cols > MAXCOLS) ) {
		fprintf(stderr, "ERROR: grid dimensions too large (max. %d x %d)\n", 
				MAXCOLS, MAXROWS);
		exit(GRIDERR);
	}
	/* allocate memory */
	world->space = malloc((world->rows + 2) * sizeof(short *));
	world->space[0] = malloc((world->rows + 2) * 
			(world->cols + 2) * sizeof(short));
	for (i=1; i < world->rows + 2; i++) 
		world->space[i] = world->space[0] + i * (world->cols + 2);

	/* initialise to zero */
	world->count = 0;
	for (i=0; i < world->rows+2; i++) 
		for (j=0; j < world->cols+2; j++) 
			world->space[i][j] = 0;
	/* read cell grid */
	for (i=0; i < world->rows; i++) {
		for (j=0; j < world->cols; j++) {
			err = fscanf(fp, "%d", &x);
			if (err != 1) {
				fprintf(stderr, "ERROR, syntax error in '%s'\n", filename);
				exit(SYNTAXERR);
			}
			world->space[i+1][j+1] = x;
			if (x) world->count++;  // keep count of alive cells
		}
	}
	/* close file and return the state that was loaded */
	fclose(fp);
}

/*
 * Save GoL state to disk.
 *   filename  --  (string)  name of the file to use
 *   world     --  (state *) generation to save
 * -> (int) 0 on SUCCESS, 1 on FAILURE
 */
int save(char filename[], state *world) {
	FILE *fp;
	int err, i, j;

	/* open file for writing */
	fp = fopen(filename, "w");
	if (fp == NULL) // fopen failed
		return 1;
	/* write metadata */
	fprintf(fp, "P1\n# generation=%d  alive=%d\n%d %d\n", 
			world->generation, world->count, world->cols, world->rows);
	/* write cell grid */
	for (i=0; i < world->rows; i++) {
		for (j=0; j < world->cols; j++) {
			fprintf(fp, "%d ", world->space[i+1][j+1]);
		}
		fprintf(fp, "\n");
	}
	/* close file */
	fclose(fp);
	return 0;
}

/* 
 * Output GoL state to STDOUT.
 *   world  --  (state *) generation to output
 */
void echo(state *world) {
	int i, j;

	printf("P1\n# generation=%d  alive=%d\n%d %d\n", 
			world->generation, world->count, world->cols, world->rows);
	for (i=0; i < world->rows; i++) {
		for (j=0; j < world->cols; j++) {
			printf("%d ", world->space[i+1][j+1]);
		}
		printf("\n");
	}
	printf("\n");
}

