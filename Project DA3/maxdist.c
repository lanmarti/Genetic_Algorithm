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

point** read_file(char* filename,			int *amount, double *xbound, double *ybound) {
	FILE* file;
	int i;
	point** points;
	point* p;
	float x,y;

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
	for(i=0;i<*amount;i++){
		fscanf(file,"%f",&x);
		fscanf(file,"%f",&y);
		p = create_point(x,y);
		if (x>*xbound) { *xbound = x; }
		if (y>*ybound) { *ybound = y; }
		points[i] = p;
	}

	/*Sluiten bestand*/
	fclose(file);
	
	//printf("aantal hoekpunten: %i\n", *amount);
	//printf("Inlezen bestand voltooid\n");
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
	int i,j,new_pop_size;
	double* acc_fit_values;
	individual** temp = (individual**) malloc((NR_OF_PARENTS+POP_SIZE)*sizeof(individual*));
	double fit_sum=0;

	if (temp == NULL){
		perror("Not enough memory to spawn next generation");
		return;
	}

	for(i=0;i<POP_SIZE;i++){
		temp[i] = problem->population[i];
	}

	// calculate accumulated fitness value for every individual in population
	acc_fit_values = calculate_normalized_fitness(temp,POP_SIZE);

	// create new solutions based on fitness
	new_pop_size = procreate(temp, acc_fit_values, problem);
	free(acc_fit_values);

	// recalculate fitness for population
	acc_fit_values = calculate_normalized_fitness(temp,new_pop_size);

	// pick dead
	j = new_pop_size;
	while(j>POP_SIZE){
		double random = (double) rand() / (double)RAND_MAX;
		for(i=new_pop_size-1;i>0;i--){
			if((acc_fit_values[i]!=0) && (1 - random < acc_fit_values[i])){
				free_individual(temp[i]);
				acc_fit_values[i]=0;
				j--;
				break;
			}
		}
	}

	j=0;
	for(i=0;i<new_pop_size;i++){
		if(acc_fit_values[i]!=0){
			problem->population[j] = temp[i];
			j++;
		}
	}
	
	free(acc_fit_values);
	free(temp);
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

	//printf("x limit: %f\ty limit: %f\n", problem->x_bound, problem->y_bound);
	//printf("fitness: %f\n", problem->poly_fit);
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
	int i,amount;
	int check=0;
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

int valid_individual(individual* ind, opt_problem* problem){
	int i;
	for(i=0;i<ind->size;i++){
		if(!point_in_polygon(ind->points[i],problem)){
			return 0;
		}
	}
	return 1;
}

int procreate(individual** temp, double* acc_fit_values, opt_problem* problem){
	int i,l=0,j=0;	
	individual* child1, *child2;
	int* picked = (int*) calloc(POP_SIZE,sizeof(int));
	
	if (picked == NULL){
		perror("Not enough memory to spawn next generation");
		return 0;
	}

	while(j < (NR_OF_PARENTS/2)){
		individual *parent1 = NULL,*parent2 = NULL;
		child1 = create_empty_individual();
		child2 = create_empty_individual();
		while(parent1==NULL){
			double random = (double) rand() / (double)RAND_MAX;
			for(i=0;i<POP_SIZE;i++){
				if(acc_fit_values[i]>random && picked[i]!=1){
					parent1 = temp[i];
					picked[i]=1;
					break;
				}
			}
		}
		while(parent2==NULL){
			double random = (double) rand() / (double)RAND_MAX;
			for(i=0;i<POP_SIZE;i++){
				if(acc_fit_values[i]>random && picked[i]!=1){
					parent2 = temp[i];
					picked[i]=1;
					break;
				}
			}
		}
		crossover(parent1,parent2,child1,child2);
		if (rand() % 100 < MUTATION_CHANCE){
			mutate(child1);
		}
		if (rand() % 100 < MUTATION_CHANCE){
			mutate(child2);
		}

		// avoid invalid individuals
		if (valid_individual(child1,problem)){
			temp[POP_SIZE+l]=child1;
			l++;
		} else {
			free_individual(child1);
		}
		if (valid_individual(child2,problem)){
			temp[POP_SIZE+l]=child2;
			l++;
		} else {
			free_individual(child2);
		}
		j++;
	}

	free(picked);
	return (POP_SIZE + l);
}

double* calculate_normalized_fitness(individual** temp, int size){
	int i,j;
	double fit_sum=0;
	double* fit_values = (double*) calloc(size,sizeof(double));
	double* acc_fit_values = (double*) calloc(size,sizeof(double));

	if (fit_values == NULL || acc_fit_values == NULL){
		perror("Not enough memory to spawn next generation");
		return 0;
	}

	// calculate fitness for population
	for(i=0;i<size;i++){
		fit_values[i] = fitness(temp[i]);
		fit_sum += fit_values[i];
	}
	// normalize
	for(i=0;i<size;i++){
		fit_values[i] = fit_values[i]/fit_sum;
	}
	// sort descending
	for(i=1;i<size;i++){
		for(j=0;j<size-i;j++){
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
	for(i=1;i<size;i++){
		acc_fit_values[i] = acc_fit_values[i-1] + fit_values[i];
	}

	free(fit_values);
	return acc_fit_values;
}