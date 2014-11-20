/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#ifndef _FUNCTIONS_HEADER_
#define _FUNCTIONS_HEADER_

extern int POP_SIZE;
extern int NR_OF_IT;
extern int NR_OF_PARENTS;
extern int MUTATION_CHANCE;

#include "organism.h"

/* file processor */
point** read_file(char* filename,			int *amount, double *xbound, double *ybound);

/* validness checkers */
int point_in_polygon(point* p, opt_problem* problem);
int valid_individual(individual* ind, opt_problem* problem);

#endif // !_FUNCTIONS_HEADER_
