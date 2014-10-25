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
	int pop_size,nr_of_points;
	double x_bound;
	double y_bound;
} opt_problem;

point** read_file(char* filename,			int *amount, double *xbound, double *ybound);

/* genetic functions */
void crossover(individual* indA, individual* indB, individual** children);
point* mutate(point* point);

/* general create and destroy functions */
void init_problem(opt_problem* problem, char* filename);
void free_problem(opt_problem* problem);
void create_population(opt_problem* problem);

individual* create_individual(point** points, int size);
void free_individual(individual* ind);
void ind_set_points(individual* ind, point** points, int size);

point* create_point(double x, double y);
point* copy_point(point* point);


int point_in_polygon(point* p, opt_problem* problem);

#endif


