/**************************************************************************
 * Game of Life (v1.0)
 *   An example implementation of Conway's Game of Life in C using
 *   static data types.
 *
 * author: Martti Louhivuori (martti.louhivuori@csc.fi)
 * date:   16.05.2011
 **************************************************************************/
#ifndef GOL_STATIC_H
#define GOL_STATIC_H 1
#include <stdio.h>

/* max. size of system */
#define MAXROWS 1000
#define MAXCOLS 1000

/* use 1 for alive cells and 0 for dead ones */
#define ALIVE 1
#define DEAD  0

/* error codes */
#define ARGERR    -1
#define SCANERR   -2
#define IOERR     -3
#define METAERR   -4
#define GRIDERR   -5
#define SYNTAXERR -6

/* container for the state of a system */
typedef struct {
	int   rows;                        	/* no. of rows in grid */
	int   cols;                        	/* no. of columns in grid */
	short space[MAXROWS+2][MAXCOLS+2]; 	/* NxM grid of cells + borders */
	int   generation;                 	/* age of system */
	int   count;                       	/* no. of alive cells */
} state;

#endif
