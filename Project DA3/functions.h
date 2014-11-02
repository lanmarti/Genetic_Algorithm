/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#ifndef _FUNCTIONS_HEADER_
#define _FUNCTIONS_HEADER_


#include "organism.h"
#include "maxdist.h"

/* file processor */
point** read_file(char* filename,			int *amount, double *xbound, double *ybound);

/* validness checkers */
int point_in_polygon(point* p, opt_problem* problem);
int valid_individual(individual* ind, opt_problem* problem);

#endif // !_FUNCTIONS_HEADER_
