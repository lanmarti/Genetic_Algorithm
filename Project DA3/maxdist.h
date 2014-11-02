/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#ifndef _MAXDIST_HEADER_
#define _MAXDIST_HEADER_

#include "organism.h"

typedef struct {
	individual** population;
	individual* polygon;
	int nr_of_points;
	double x_bound, y_bound;
	double poly_fit;
} opt_problem;

#define POP_SIZE 100
#define NR_OF_IT 100
#define NR_OF_PARENTS 60
#define MUTATION_CHANCE 60

int main();

/* functions for setting up problem */
void init_problem(opt_problem* problem, int nr_of_points, char* filename);
void free_problem(opt_problem* problem);
void create_population(opt_problem* problem);

#endif


