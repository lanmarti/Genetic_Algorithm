/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#ifndef _MAXDIST_MPI_HEADER_
#define _MAXDIST_MPI_HEADER_

#include "organism.h"

int POP_SIZE;
int NR_OF_IT;
int NR_OF_PARENTS;
int MUTATION_CHANCE;

int main();

/* functions for setting up problem */
void init_problem(opt_problem* problem, int nr_of_points, char* filename);
void free_problem(opt_problem* problem);
void create_population(opt_problem* problem);

#endif


