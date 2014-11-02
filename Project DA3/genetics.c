/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "functions.h"
#include "genetics.h"

/* crossover function: overwrites the points of child1 and child2 */
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

/* mutate function: randomly adds or subtracts 0-0.1 to/from the x and y coords */
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

/* picks parents for the crossover function, mutates some children, 
	checks if children are valid individuals, returns the new population size */
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

/* uses selection algorithm for the procreate function,
	uses reversed selection algorithm to pick which individuals die */
void spawn_next_gen(opt_problem* problem){
	int i,j,new_pop_size;
	double* acc_fit_values;
	individual** temp = (individual**) malloc((NR_OF_PARENTS+POP_SIZE)*sizeof(individual*));

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

	// overwrite old population with new one
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

/* calculates the fitness of an individual  */
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

/* normalizes the fitness values for an entire population 
	and sorts the individuals by descending accumulated fitness.
	Each individual has an accumulated fitness value of it's own fitness
	value plus all fitnessvalues of its (more fit) predecessors
	i.e. the least fit individual has an accumulated fitness value of 1 */
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