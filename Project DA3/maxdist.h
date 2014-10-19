/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#ifndef _MAXDIST_HEADER_
#define _MAXDIST_HEADER_

typedef struct {
	float x;
	float y;
	// fitness?
} point;

typedef struct {
	point** population;
	int size;
} opt_problem;

point** read_file(char* filename,			int amount);

/* genetic functions */
void crossover(point* parentA, point* parentB, point** children); 
point* mutate(point* point);

/* general functions */
opt_problem* create_opt_problem();
void free_opt_problem(opt_problem* problem);
void problem_set_population(opt_problem* problem, point* population, int size);

point* create_point(float x, float y);

#endif


