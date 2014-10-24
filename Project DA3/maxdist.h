/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#ifndef _MAXDIST_HEADER_
#define _MAXDIST_HEADER_

typedef struct {
	double x;
	double y;
} point;

typedef struct {
	point** points;
	int size;
} individual;

typedef struct {
	individual** population;
} opt_problem;

point** read_file(char* filename,			int amount);

/* genetic functions */
void crossover(point* parentA, point* parentB, point** children); 
point* mutate(point* point);

/* general functions */
individual* create_individual();
void free_individual(individual* ind);
void ind_set_points(individual* ind, point* points, int size);

point* create_point(double x, double y);

#endif


