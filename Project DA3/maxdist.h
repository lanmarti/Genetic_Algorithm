/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#ifndef _MAXDIST_HEADER_
#define _MAXDIST_HEADER_

typedef struct {
	double x,y;
} point;

typedef struct {
	point** points;
	int size;
} individual;

typedef struct {
	individual** population;
	individual* polygon;
	int nr_of_points;
	double x_bound, y_bound;
	double poly_fit;
} opt_problem;

const int POP_SIZE = 300;
const int NR_OF_IT = 200;
const int NR_OF_PARENTS = 280;

point** read_file(char* filename,			int *amount, double *xbound, double *ybound);

/* genetic functions */
void crossover(individual* indA, individual* indB, individual *child1, individual *child2);
void mutate(individual* ind);
void spawn_next_gen(opt_problem* problem);
double fitness(individual* ind);

/* general create and destroy functions */
void init_problem(opt_problem* problem, int nr_of_points, char* filename);
void free_problem(opt_problem* problem);
void create_population(opt_problem* problem);

individual* create_individual(point** points, int size);
individual* create_empty_individual();
void free_individual(individual* ind);
void ind_set_points(individual* ind, point** points, int size);

point* create_point(double x, double y);
point* copy_point(point* point);


int point_in_polygon(point* p, opt_problem* problem);
int valid_individual(individual* ind, opt_problem* problem);

#endif


