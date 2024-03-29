/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "genetics.h"
#include "organism.h"
#include "functions.h"
#include "maxdist.h"


//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>


int main(int argc, char* argv []) {
	int i,intvar;
	opt_problem problem;

	if (argc != 3) { // 2 argumenten nodig! 
		printf("Error: expected 2 arguments: number of points and name of file to read\n");
		exit(-1);
	}
	if (sscanf (argv[1], "%i", &intvar)!=1) {
		printf ("Error: first argument is not an integer\n");
		exit (-1);
	}
	init_problem(&problem, intvar, argv[2]);

	for(i=0;i<NR_OF_IT;i++){
		spawn_next_gen(&problem);
	}

	printf("%f\n", fitness(problem.population[0]));
	for(i=0;i<intvar;i++){
		printf("%f %f\n",problem.population[0]->points[i]->x, problem.population[0]->points[i]->y);
	}

	free_problem(&problem);
	//_CrtDumpMemoryLeaks();
	return 0;
}

/* creates a starting population for the given problem */
void create_population(opt_problem* problem){
	int i,j;
	individual* new_ind;

	srand(time(NULL));
	for(i=0;i<POP_SIZE;i++){
		point** points = (point**) calloc(problem->nr_of_points,sizeof(point*));
		if (points == NULL){
			perror("Error creating population");
			return;
		}
		j=0;
		while(j<problem->nr_of_points){
			double x=(double) (rand() * problem->x_bound) / RAND_MAX;
			double y=(double) (rand() * problem->y_bound) / RAND_MAX;
			points[j] = create_point(x,y);
			if (point_in_polygon(points[j],problem)){
				j++;
			} else {
				free(points[j]);
			}
		}
		new_ind = create_individual(points,problem->nr_of_points);
		for(j=0;j<problem->nr_of_points;j++){
			free(points[j]);
		}
		free(points);
		problem->population[i] = new_ind;
	}
}

/* initializes the problem: sets up the starting polygon, starting population
	and calculates the fitness of the polygon */
void init_problem(opt_problem* problem, int nr_of_points, char* file){
	int amount=0,i;
	double xbound = 0, ybound = 0;
	point** corners;

	problem->population = (individual**) malloc(POP_SIZE*sizeof(individual*));
	if (problem->population == NULL){
		perror("Error setting up starting population");
		return;
	}

	corners = read_file(file,		&amount,&xbound,&ybound);
	problem->polygon = create_individual(corners,amount);
	for(i=0;i<amount;i++){
		free(corners[i]);
	}
	free(corners);

	problem->nr_of_points = nr_of_points;	
	problem->x_bound = xbound;
	problem->y_bound = ybound;
	problem->poly_fit = fitness(problem->polygon);
	create_population(problem);
}

/* frees all memory used by the problem */
void free_problem(opt_problem* problem){
	int i;
	for(i=0;i<POP_SIZE;i++){
		free_individual(problem->population[i]);
	}
	free(problem->population);
	free_individual(problem->polygon);
}