/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#ifndef _ORGANISM_HEADER_
#define _ORGANISM_HEADER_

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

individual* create_individual(point** points, int size);
individual* create_empty_individual();
void free_individual(individual* ind);
void ind_set_points(individual* ind, point** points, int size);

point* create_point(double x, double y);
point* copy_point(point* point);


#endif // !_ORGANISM_HEADER_
