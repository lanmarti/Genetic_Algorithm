/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

/* Takes a string as input and tries to read file with the same name.
	Overwrites amount with the number of points defined in the file and 
	xbound and ybound with the upper x and y limits. */
point** read_file(char* filename,			int *amount, double *xbound, double *ybound) {
	FILE* file;
	int i;
	point** points;
	point* p;
	float x,y;

	file = fopen(filename,"r");
	if (file == NULL){
		perror("Error opening file");
		exit(-3);
	}
	fscanf (file, "%d", amount);

	points = (point**) malloc(sizeof(point*)*(*amount));
	if (points == NULL){
		perror("Not enough memory available to create points");
		fclose(file);
		exit(-4);
	}
	for(i=0;i<*amount;i++){
		fscanf(file,"%f",&x);
		fscanf(file,"%f",&y);
		p = create_point(x,y);
		if (x>*xbound) { *xbound = x; }
		if (y>*ybound) { *ybound = y; }
		points[i] = p;
	}

	fclose(file);
	
	return points;
}

/* returns 1 if the given point lies within the polygon of the given problem, 0 otherwise 
	points on boundaries do NOT count as within the polygon */
int point_in_polygon(point* p, opt_problem* problem){
	int i,amount;
	double side, previous = 0;
	point** corners;

	if(p->x > problem->x_bound || p->y > problem->y_bound) { return 0; } // punt ligt buiten bounding box;
	corners = problem->polygon->points;
	amount = problem->polygon->size;

	for(i=0;i<amount;i++){
		side = ((p->y - corners[i]->y) * (corners[(i+1)%amount]->x - corners[i]->x)) - 
			((p->x - corners[i]->x) * (corners[(i+1)%amount]->y - corners[i]->y));
		if(((side<0)!=(previous<0) && i!=0 ) || side == 0){
			return 0;
		}
		previous = side;
	}

	return 1;
}

/* checks if every point of the individual lies within the polygon */
int valid_individual(individual* ind, opt_problem* problem){
	int i;
	for(i=0;i<ind->size;i++){
		if(!point_in_polygon(ind->points[i],problem)){
			return 0;
		}
	}
	return 1;
}