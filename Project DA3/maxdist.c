/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#include "maxdist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv []) {return 0;}

/* Geef de inhoud van een bestand terug */
char* read_file(char* filename) {
	FILE* file;
	int amount;
    unsigned long length;   /*lengte file*/
	char* content; /*inhoud file*/

	/*Openen bestand*/
	file= fopen(filename,"r");
	
	/*lengte van file*/
	length = get_file_length(file);
	
	content = (char *)malloc((length+1)*sizeof(char));
	if (content == NULL) {
		printf("ERROR: Allocation failed, insufficient memory?\n");
		exit(1);
	}
	fread(content,sizeof(char),length,file);
    
	content[length]='\0';  /* eindmarkering inhoud */
		
	/*Sluiten bestand*/
	fclose(file);
	
	return content;
}

/* Bepaald het aantal karakters in een bestand */
unsigned long get_file_length(FILE *ifp) {
	unsigned long length= 0;
	 
	/* zolang einde file niet bereikt, blijven tellen*/
	fseek(ifp, 0, SEEK_END);
	length = (unsigned long) ftell(ifp);
	length = length/sizeof(char);
	fseek(ifp, 0, SEEK_SET);
	return length;
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
	for(int i=0;i<problem->size;i++){
		free(problem->population[i]);
	}
	free(problem->population);
	free(problem);
}

void problem_set_population(opt_problem* problem, point** population, int size) {
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
