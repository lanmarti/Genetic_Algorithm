/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "maxdist.h"


int main(int argc, char* argv []) {
	opt_problem problem;
	init_problem(&problem,"vierhoek.txt");
	free_problem(&problem);
	getchar();
	return 0;
}

/* Geef de inhoud van een bestand terug */
point** read_file(char* filename,			int amount) {
	FILE* file;
	int i;
	point** points;
	point* p;
	float x,y;

	/*Openen bestand*/
	file = fopen(filename,"r");
	if (file == NULL){
		perror("Error creating points");
		return NULL;
	}
	fscanf (file, "%d", &amount);
	printf("aantal lijnen: %i\n", amount);

	points = (point**) malloc(sizeof(point*)*amount);
	if (points == NULL){
		perror("Not enough memory available to create points");
		fclose(file);
		return NULL;
	}
	printf("Aanmaken van veelhoek:\n");
	for(i=0;i<amount;i++){
		fscanf(file,"%f",&x);
		fscanf(file,"%f",&y);
		p = create_point(x,y);
		points[i] = p;
		printf("punt %i: %f	%f\n",i+1,p->x, p->y);
	}

	/*Sluiten bestand*/
	fclose(file);

	printf("Inlezen bestand voltooid\n");
	return points;
}

void crossover(individual* indA, individual* indB, individual** children){
	individual* child1 = (individual*) calloc(1,sizeof(individual));
	individual* child2 = (individual*) calloc(1,sizeof(individual));
	int switchpoint,i=0;

	if (child1 == NULL || child2 == NULL){
		printf("ERROR: Allocation failed, insufficient memory?\n");
		exit(1);
	}

	switchpoint = rand()%indA->size;
	while(i<switchpoint){
		child1->points[i] = copy_point(indA->points[i]);
		child2->points[i] = copy_point(indB->points[i]);
		i++;
	}
	while(i<indA->size){
		child1->points[i] = copy_point(indB->points[i]);
		child2->points[i] = copy_point(indA->points[i]);
		i++;
	}

	// return values
}

point* mutate(point* point){
	int option;
	srand(time(NULL));
	option = rand()%4;
	switch (option) {
		case 0:
			point->x = point->x + ((double) rand()/(10*(double) RAND_MAX));
			point->y = point->y + ((double) rand()/(10*(double) RAND_MAX));
			break;
		case 1:
			point->x = point->x + ((double) rand()/(10*(double) RAND_MAX));
			point->y = point->y - ((double) rand()/(10*(double) RAND_MAX));
			break;
		case 2:
			point->x = point->x - ((double) rand()/(10*(double) RAND_MAX));
			point->y = point->y + ((double) rand()/(10*(double) RAND_MAX));
			break;
		default:
			point->x = point->x - ((double) rand()/(10*(double) RAND_MAX));
			point->y = point->y - ((double) rand()/(10*(double) RAND_MAX));
			break;
	}

	return point;
}

individual* create_individual(point** points, int size){
	individual* ind = (individual*) malloc(sizeof(individual));
	int i;
	ind->points = (point**) malloc(size*sizeof(point*));

	ind->size = size;
	for(i=0;i<size;i++){
		ind->points[i] = copy_point(points[i]);
	}

	return ind;
}

void free_individual(individual* ind){
	int i;
	for(i=0;i<ind->size;i++){
		free(ind->points[i]);
	}
	free(ind->points);
	free(ind);
}

void ind_set_points(individual* ind, point** points, int size) {
	int i;
	
	if ( ind->points != NULL) { 
		for(i=0;i<ind->size;i++){
			free(ind->points[i]);
		}
		free(ind->points);
	}
	ind->points=(point**)malloc(size*sizeof(point));
	// test op null

	for(i=0;i<size;i++){
		ind->points[i] = copy_point(points[i]);
	}
	ind->size=size;
}

point* create_point(double x, double y){
	point* p = (point*) malloc(sizeof(point));
	//test op null
	p->x = x;
	p->y = y;

	return p;
}

point* copy_point(point* original){
	return create_point(original->x, original->y);
}

double fitness(individual* ind){
	int i,j;
	double dist=0;

	for(i=0;i<ind->size;i++){
		for(j=0;j<ind->size;j++){
			dist += sqrt((pow(ind->points[i]->x,2)-pow(ind->points[j]->x,2))+(pow(ind->points[i]->y,2)-pow(ind->points[j]->y,2)));
		}
	}
	dist = sqrt(dist);
	return dist;
}

void spawn_next_gen(opt_problem* problem){
	// pick future parents
	// mutate some of the children

	// cull the population
}

individual** create_population(){
	individual** pop = (individual**) malloc(1*sizeof(individual*));
	return pop;
}

void init_problem(opt_problem* problem, char* file){
	int size=0;
	point** corners;

	corners = read_file(file,		size);
	problem->polygon = create_individual(corners,size);
	problem->population = create_population();
	free(corners);
}

void free_problem(opt_problem* problem){
	int i;
	for(i=0;i<problem->pop_size;i++){
		free_individual(problem->population[i]);
	}
	free(problem->population);
	free(problem->polygon);
}