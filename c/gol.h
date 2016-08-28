/**************************************************************************
 * Game of Life (v1.0)
 *   An example implementation of Conway's Game of Life in C.
 *
 * author: Martti Louhivuori (martti.louhivuori@csc.fi)
 * date:   17.05.2011
 **************************************************************************/
#ifndef GOL_H
#define GOL_H 1
#include <stdio.h>

/* max. size of system */
#define MAXROWS 32767
#define MAXCOLS 32767

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
	int   rows;       	/* no. of rows in grid */
	int   cols;       	/* no. of columns in grid */
	short **space;    	/* a pointer to a list of pointers for storing
                      	   a dynamic NxM grid of cells + borders */
	int   generation; 	/* age of system */
	int   count;      	/* no. of alive cells */
} state;

#endif
