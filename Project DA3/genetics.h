/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#ifndef _GENETICS_HEADER_
#define _GENETICS_HEADER_

#include "maxdist.h"

/* genetic functions */
void crossover(individual* indA, individual* indB, individual *child1, individual *child2);
void mutate(individual* ind);
int procreate(individual** temp, double* acc_fit_values, opt_problem* problem); /* returns size of new population */
void spawn_next_gen(opt_problem* problem);
double fitness(individual* ind);
double* calculate_normalized_fitness(individual** temp, int size);

#endif // !_GENETICS_HEADER_
