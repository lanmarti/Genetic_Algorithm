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
	individual* polygon;
	int pop_size;
} opt_problem;

point** read_file(char* filename,			int amount);

/* genetic functions */
void crossover(individual* indA, individual* indB, individual** children);
point* mutate(point* point);

/* general create and destroy functions */
void init_problem(opt_problem* problem, char* filename);
void free_problem(opt_problem* problem);

individual* create_individual();
void free_individual(individual* ind);
void ind_set_points(individual* ind, point** points, int size);

point* create_point(double x, double y);
point* copy_point(point* point);

#endif


