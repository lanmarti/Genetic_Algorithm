/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "maxdist.h"

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>


int main(int argc, char* argv []) {
	int i,j,intvar;
	opt_problem problem;
	double best_fit,old_best_fit;

	if (argc != 3) { // 2 argumenten nodig! 
		printf("Error: expected 2 arguments: number of points and name of file to read\n");
		exit(-1);
	}
	if (sscanf (argv[1], "%i", &intvar)!=1) {
		printf ("Error: first argument is not an integer\n");
		exit (-1);
	}
	init_problem(&problem, intvar, argv[2]);
	
		best_fit = 0;
		old_best_fit=0;

	for(i=0;i<NR_OF_IT;i++){
		spawn_next_gen(&problem);
		for(j=0;j<POP_SIZE;j++){
			if (fitness(problem.population[j])>best_fit){
				best_fit = fitness(problem.population[j]);
			}
		}
		if (best_fit != old_best_fit) {
			printf("Iteratie %i: beste fit: %f\n",i,best_fit);
			old_best_fit = best_fit;
		}
	}
	printf("#####\nBest solution after %i iterations:\n",NR_OF_IT);
	printf("fitness: %f\n", old_best_fit);
	for(i=0;i<intvar;i++){
		printf("\tpoint %i: x=%f\ty=%f\n",i+1,problem.population[0]->points[i]->x, problem.population[0]->points[i]->y);
	}
	printf("#####\n");

	free_problem(&problem);
	getchar();
	//_CrtDumpMemoryLeaks();
	return 0;
}

/* Geef de inhoud van een bestand terug */
point** read_file(char* filename,			int *amount, double *xbound, double *ybound) {
	FILE* file;
	int i;
	point** points;
	point* p;
	float x,y;

	/*Openen bestand*/
	file = fopen(filename,"r");
	if (file == NULL){
		perror("Error opening file");
		exit(-2);
	}
	fscanf (file, "%d", amount);

	points = (point**) malloc(sizeof(point*)*(*amount));
	if (points == NULL){
		perror("Not enough memory available to create points");
		fclose(file);
		exit(-2);
	}
	printf("Aanmaken van veelhoek:\n");
	for(i=0;i<*amount;i++){
		fscanf(file,"%f",&x);
		fscanf(file,"%f",&y);
		p = create_point(x,y);
		if (x>*xbound) { *xbound = x; }
		if (y>*ybound) { *ybound = y; }
		points[i] = p;
		printf("\tpunt %i: %f	%f\n",i+1,p->x, p->y);
	}

	/*Sluiten bestand*/
	fclose(file);
	
	printf("aantal hoekpunten: %i\n", *amount);
	printf("Inlezen bestand voltooid\n");
	return points;
}

void crossover(individual* indA, individual* indB, individual *child1, individual *child2){
	point** child1_points = (point**) calloc(indA->size,sizeof(point*));
	point** child2_points = (point**) calloc(indA->size,sizeof(point*));
	int switchpoint,i=0;

	if (child1_points == NULL || child2_points == NULL ){
		perror("Not enough memory available to create children");
		return;
	}

	switchpoint = rand()%indA->size;
	while(i<switchpoint){
		child1_points[i] = copy_point(indA->points[i]);
		child2_points[i] = copy_point(indB->points[i]);
		i++;
	}
	while(i<indA->size){
		child1_points[i] = copy_point(indB->points[i]);
		child2_points[i] = copy_point(indA->points[i]);
		i++;
	}
	ind_set_points(child1, child1_points,indA->size);
	ind_set_points(child2, child2_points,indA->size);

	for(i=0;i<indA->size;i++){
		free(child1_points[i]);
		free(child2_points[i]);
	}
	free(child1_points);
	free(child2_points);
}

void mutate(individual* ind){
	int option,random;
	point* p;
	random = (int) rand()%ind->size;
	p = ind->points[random];
	option = rand()%4;
	switch (option) {
		case 0:
			p->x = p->x + ((double) rand()/(10*(double) RAND_MAX));
			p->y = p->y + ((double) rand()/(10*(double) RAND_MAX));
			break;
		case 1:
			p->x = p->x + ((double) rand()/(10*(double) RAND_MAX));
			p->y = p->y - ((double) rand()/(10*(double) RAND_MAX));
			break;
		case 2:
			p->x = p->x - ((double) rand()/(10*(double) RAND_MAX));
			p->y = p->y + ((double) rand()/(10*(double) RAND_MAX));
			break;
		default:
			p->x = p->x - ((double) rand()/(10*(double) RAND_MAX));
			p->y = p->y - ((double) rand()/(10*(double) RAND_MAX));
			break;
	}
}

individual* create_individual(point** points, int size){
	individual* ind = (individual*) malloc(sizeof(individual));
	int i;
	ind->points = (point**) malloc(size*sizeof(point*));
	if (ind->points == NULL){
		perror("Error creating individual");
		return NULL;
	}

	ind->size = size;
	for(i=0;i<size;i++){
		ind->points[i] = copy_point(points[i]);
	}

	return ind;
}

individual* create_empty_individual(){
	individual* ind = (individual*) malloc(sizeof(individual));
	ind->points = NULL;
	ind->size = 0;
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

point* copy_point(point* original){
	return create_point(original->x, original->y);
}

double fitness(individual* ind){
	int i,j;
	double dist=0;

	for(i=0;i<ind->size;i++){
		for(j=0;j<ind->size;j++){
			dist += sqrt(sqrt(pow(ind->points[i]->x-ind->points[j]->x,2)+(pow(ind->points[i]->y-ind->points[j]->y,2))));
		}
	}
	return dist;
}

void spawn_next_gen(opt_problem* problem){
	int i,j,k,l=0,new_pop_size;
	individual** temp = (individual**) malloc((NR_OF_PARENTS+POP_SIZE)*sizeof(individual*));
	individual* child1p, *child2p;
	double fit_sum=0;
	double* fit_values = (double*) calloc(POP_SIZE+NR_OF_PARENTS,sizeof(double));
	double* acc_fit_values = (double*) calloc(POP_SIZE+NR_OF_PARENTS,sizeof(double));
	if (fit_values == NULL || acc_fit_values == NULL || temp == NULL){
		perror("Not enough memory to spawn next generation");
		return;
	}
	for(i=0;i<POP_SIZE;i++){
		temp[i] = problem->population[i];
	}
	// UPDATE THIS!!
	// procreate
	for(i=0;i<NR_OF_PARENTS;i=i+2){
		child1p = create_empty_individual();
		child2p = create_empty_individual();
		j = rand() % POP_SIZE;
		k = rand() % POP_SIZE;
		while(j==k){ // zorgen dat de ouders verschillen
			k = rand() % POP_SIZE;
		}
		crossover(problem->population[j],problem->population[k], child1p, child2p);
		// create mutants
		if (rand() % 100 > 80){
			mutate(child1p);
		}

		// avoid invalid individuals
		if (valid_individual(child1p,problem)){
			temp[POP_SIZE+l]=child1p;
			l++;
		} else {
			free_individual(child1p);
		}

		if (rand() % 100 > 80){
			mutate(child2p);
		}
		if (valid_individual(child2p,problem)){
			temp[POP_SIZE+l]=child2p;
			l++;
		} else {
			free_individual(child2p);
		}
	}
	new_pop_size = POP_SIZE+l;

	//printf("%i new children\n",l);
	// calculate fitness for population
	for(i=0;i<new_pop_size;i++){
		fit_values[i] = fitness(temp[i]);
		fit_sum += fit_values[i];
	}
	// normalize
	for(i=0;i<new_pop_size;i++){
		fit_values[i] = fit_values[i]/fit_sum;
	}
	// sort
	for(i=1;i<new_pop_size;i++){
		for(j=0;j<new_pop_size-i;j++){
			if(fit_values[j] < fit_values[j+1]){
				double temp_value = fit_values[j+1];
				individual* temp_individual = temp[j+1];
				fit_values[j+1] = fit_values[j];
				temp[j+1] = temp[j];
				fit_values[j] = temp_value;
				temp[j] = temp_individual;
			}
		}
	}
	// assign accumulated value
	acc_fit_values[0] = fit_values[0];
	for(i=1;i<new_pop_size;i++){
		acc_fit_values[i] = acc_fit_values[i-1] + fit_values[i];
	}
	free(fit_values);

	l=0;
	// pick new population
	while(l < POP_SIZE){
		double random = (double) rand() / (double)RAND_MAX;
		for(i=0;i<new_pop_size;i++){
			if(acc_fit_values[i]>random){
				problem->population[l]=temp[i];
				acc_fit_values[i] = 0;	
				l++;
				break;
			}
		}
	}
	for(i=0;i<new_pop_size;i++){
		if (acc_fit_values[i]!=0){
			free_individual(temp[i]);
		}
	}
	free(temp);
	free(acc_fit_values);
}

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

	printf("x limit: %f\ty limit: %f\n", problem->x_bound, problem->y_bound);
	printf("fitness: %f\n", problem->poly_fit);
}

void free_problem(opt_problem* problem){
	int i;
	for(i=0;i<POP_SIZE;i++){
		free_individual(problem->population[i]);
	}
	free(problem->population);
	free_individual(problem->polygon);
}

int point_in_polygon(point* p, opt_problem* problem){
	int i,j;
	int inside = 0;
	point** corners;
	
	if(p->x > problem->x_bound || p->y > problem->y_bound) { return 0; } // punt ligt buiten bounding box;
	corners = problem->polygon->points;
	for(i=0,j=problem->polygon->size-1;i<problem->polygon->size;j=i++){
		if(((corners[i]->y < p->y) != (corners[j]->y < p->y)) // y coord ligt tussen de y coords van twee hoekpunten
			&& ( p->x < ((corners[j]->x - corners[i]->x) * (p->y - corners[i]->y) / (corners[j]->y - corners[i]->y) + corners[i]->x ))){
			inside = !inside;
		}
	}
	return inside;
}

int valid_individual(individual* ind, opt_problem* problem){
	int i;
	for(i=0;i<ind->size;i++){
		if(!point_in_polygon(ind->points[i],problem)){
			return 0;
		}
	}
	return 1;
}