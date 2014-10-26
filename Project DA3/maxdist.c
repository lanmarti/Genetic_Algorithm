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
	opt_problem problem;
	init_problem(&problem,"vierhoek.txt");
	
	printf("x limit: %lf		y limit: %lf\n", problem.x_bound, problem.y_bound);
	printf("problem->polygon points:\n\tpunt 2: %lf %lf\n", problem.polygon->points[1]->x,problem.polygon->points[1]->y);

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
		return NULL;
	}
	fscanf (file, "%d", amount);
	printf("aantal lijnen: %i\n", *amount);

	points = (point**) malloc(sizeof(point*)*(*amount));
	if (points == NULL){
		perror("Not enough memory available to create points");
		fclose(file);
		return NULL;
	}
	printf("Aanmaken van veelhoek:\n");
	for(i=0;i<*amount;i++){
		fscanf(file,"%f",&x);
		fscanf(file,"%f",&y);
		p = create_point(x,y);
		if (x>*xbound) { *xbound = x; }
		if (y>*ybound) { *ybound = y; }
		points[i] = p;
		printf("punt %i: %f	%f\n",i+1,p->x, p->y);
	}

	/*Sluiten bestand*/
	fclose(file);

	printf("Inlezen bestand voltooid\n");
	return points;
}

void crossover(individual* indA, individual* indB, individual* child1, individual* child2){
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
	child1 = create_individual(child1_points,indA->size);
	child2 = create_individual(child2_points,indA->size);
	for(i=0;i<indA->size;i++){
		free(child1_points[i]);
		free(child2_points[i]);
	}
	free(child1_points);
	free(child2_points);
}

void mutate(individual* ind){
	int option;
	point* p;
	srand(time(NULL));
	p = ind->points[rand()%ind->size];
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
			dist += sqrt(sqrt((pow(ind->points[i]->x,2)-pow(ind->points[j]->x,2))+(pow(ind->points[i]->y,2)-pow(ind->points[j]->y,2))));
		}
	}
	return dist;
}

void spawn_next_gen(opt_problem* problem){
	int i,j,k,l=0;
	individual** children = (individual**) malloc(NR_OF_PARENTS*sizeof(individual*));
	
	// UPDATE THIS!!
	// procreate
	srand(time(NULL));
	for(i=0;i<NR_OF_PARENTS;i=i+2){
		individual child1,child2;
		j = rand() % POP_SIZE;
		k = rand() % POP_SIZE;
		while(j==k){ // zorgen dat de ouders verschillen
			k = rand() % POP_SIZE;
		}
		crossover(problem->population[j],problem->population[k], &child1, &child2);
		// create mutants
		if (rand() % 100 > 80){
			mutate(&child1);
		}

		// avoid jean grey babies
		if (valid_individual(&child1,problem)){
			problem->population[l]=&child1;
			l++;
		} else {
			free(&child1);
		}

		if (rand() % 100 > 80){
			mutate(&child2);
		}
		if (valid_individual(&child2,problem)){
			problem->population[l]=&child2;
			l++;
		} else {
			free(&child2);
		}
	}

	// culling the population
	while(l > 0){
		int random = rand()%POP_SIZE;
		// formule hier UPDATE
		if( /* iets met fitness */ rand()%10 > 9){
			free(problem->population[random]);
			problem->population[random] = children[l-1];
			free(children[l-1]);
			l--;
		}
	}
	free(children);
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
		free(points);
		problem->population[i] = new_ind;
	}
}

void init_problem(opt_problem* problem, char* file){
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

	problem->nr_of_points = amount;
	problem->x_bound = xbound;
	problem->y_bound = ybound;
	create_population(problem);
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
	for(i=0,j=problem->nr_of_points-1;i<problem->nr_of_points;j=i++){
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