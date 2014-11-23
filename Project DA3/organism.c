/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "organism.h"

/* constructor: makes deep copy of the given points */
individual* create_individual(point** points, int size){
	individual* ind = (individual*) malloc(sizeof(individual));
	int i;
	ind->points = (point**) malloc(size*sizeof(point*));
	if ( ind == NULL || ind->points == NULL ){
		perror("Error creating individual");
		return NULL;
	}

	ind->size = size;
	for(i=0;i<size;i++){
		ind->points[i] = copy_point(points[i]);
	}

	return ind;
}

/* constructor: 0 values for all fields */
individual* create_empty_individual(){
	individual* ind = (individual*) malloc(sizeof(individual));
	ind->points = NULL;
	ind->size = 0;
	return ind;
}

/* frees the memory used by a individual and its points */
void free_individual(individual* ind){
	int i;
	for(i=0;i<ind->size;i++){
		free(ind->points[i]);
	}
	free(ind->points);
	free(ind);
}

/* overwrites an individual's points, makes deep copy of given points */
void ind_set_points(individual* ind, point** points, int size) {
	int i;
	
	if ( ind->points != NULL) { 
		for(i=0;i<ind->size;i++){
			free(ind->points[i]);
		}
		free(ind->points);
	}
	ind->points=(point**)malloc(size*sizeof(point*));
		if (ind->points == NULL){
		perror("Error changing points");
		return;
	}

	for(i=0;i<size;i++){
		ind->points[i] = copy_point(points[i]);
	}
	ind->size=size;
}

/* constructor for a point */
point* create_point(double x, double y){
	point* p = (point*) malloc(sizeof(point));
	if (p == NULL){
		perror("Error creating point");
		return NULL;
	}
	p->x = x;
	p->y = y;

	return p;
}

/* copy constructor: deep copy of given point */
point* copy_point(point* original){
	return create_point(original->x, original->y);
}