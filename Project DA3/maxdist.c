/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crtdbg.h>
#include <math.h>
#include <time.h>
#include "maxdist.h"


int main(int argc, char* argv []) {
	read_file("vierhoek.txt", 0);
	getchar();
	return 0;
}

/* Geef de inhoud van een bestand terug */
point** read_file(char* filename,			int amount) {
	FILE* file;
	int i;
	point** population;
	point* p;
	float x,y;

	/*Openen bestand*/
	file = fopen(filename,"r");
	if (file == NULL){
		perror("Error creating population");
		return NULL;
	}
	fscanf (file, "%d", &amount);
	printf("aantal lijnen: %i\n", amount);

	population = (point**) malloc(sizeof(point*)*amount);
	if (population == NULL){
		perror("Not enough memory available to create population");
		fclose(file);
		return NULL;
	}

	for(i=0;i<amount;i++){
		fscanf(file,"%f",&x);
		fscanf(file,"%f",&y);
		p = create_point(x,y);
		population[i] = p;
		printf("punt %i: %f	%f\n",i,p->x, p->y);
	}

	/*Sluiten bestand*/
	fclose(file);

	printf("Inlezen bestand voltooid\n");
	return population;
}

void crossover(point* parentA, point* parentB, point** children){
	point* child1 = (point*) calloc(1,sizeof(point));
	point* child2 = (point*) calloc(1,sizeof(point));

	if (child1 == NULL || child2 == NULL){
		printf("ERROR: Allocation failed, insufficient memory?\n");
		exit(1);
	}

	child1->x = parentA->x;
	child1->y = parentB->y;
	child2->x = parentA->x;
	child2->y = parentB->y;
}

point* mutate(point* point){
	point->x = point->x + 50; /* random getal hier */ 
	point->y = point->y + 50; /* random getal hier */
	printf("Zou jij mutatie niet eens juist implementeren?");
	return point;
}

void free_opt_problem(opt_problem* problem){
	int i;
	for(i=0;i<problem->size;i++){
		free(problem->population[i]);
	}
	free(problem->population);
	free(problem);
}

void problem_set_population(opt_problem* problem, point* population, int size) {
	int i;
	
	if ( problem->population != NULL) { 
		for(i=0;i<problem->size;i++){
			free(problem->population[i]);
		}
		free(problem->population);
	}
	problem->population=(point**)malloc(size*sizeof(point));
	// test op null

	// maak deep copy

	problem->size=size;
}

point* create_point(float x, float y){
	point* p = (point*) malloc(sizeof(point));
	p->x = x;
	p->y = y;

	return p;
}

