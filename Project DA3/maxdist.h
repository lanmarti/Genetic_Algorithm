/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#ifndef _MAXDIST_HEADER_
#define _MAXDIST_HEADER_

#include "organism.h"

int POP_SIZE = 100;
int NR_OF_IT = 50;
int NR_OF_PARENTS = 60;
int MUTATION_CHANCE = 60;
int MUTATION_MAX = 0.1;

int main();

/* functions for setting up problem */
void init_problem(opt_problem* problem, int nr_of_points, char* filename);
void free_problem(opt_problem* problem);
void create_population(opt_problem* problem);

#endif


