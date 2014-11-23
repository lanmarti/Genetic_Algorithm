/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#include <stddef.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "genetics.h"
#include "organism.h"
#include "functions.h"
#include "maxdist_mpi_var.h"


//#define _crtdbg_map_alloc
//#include <stdlib.h>
//#include <crtdbg.h>

int mpi_id;


int main(int argc, char* argv []) {
	int i,j,intvar,mpi_res,mpi_size,mpi_received,transfers;
	opt_problem problem;
	MPI_Datatype mpi_o_type;
	MPI_Status mpi_status;
	MPI_Request mpi_request;
	
	MPI_Aint offsets[3];
	MPI_Datatype oldtypes[3] = { MPI_INT, MPI_DOUBLE, MPI_DOUBLE};
	int blockcounts[3];

	mpi_organism* old_pop;
	mpi_organism* new_pop;

	mpi_organism old_ind;
	mpi_organism new_ind;

	//_CrtSetBreakAlloc(1394);

	if (argc != 8) { // 7 argumenten nodig! 
		printf("Error: expected 6 arguments in following order:\n"
			"\t1. number of points\n"
			"\t2. name of file\n"
			"\t3. population size\n"
			"\t4. number of iterations\n"
			"\t5. number of parents\n"
			"\t6. mutation chance\n"
			"\t7. maximum mutation difference\n");
		exit(-1);
	}
	if (sscanf (argv[1], "%i", &intvar)!=1) {
		printf ("Error: first argument is not an integer\n");
		exit (-2);
	}
	sscanf(argv[3], "%i", &POP_SIZE);
	sscanf(argv[4], "%i", &NR_OF_IT);
	sscanf(argv[5], "%i", &NR_OF_PARENTS);
	sscanf(argv[6], "%i", &MUTATION_CHANCE);
	sscanf(argv[7], "%i", &MUTATION_MAX);
	mpi_res = MPI_Init(&argc, &argv);
	if (mpi_res != MPI_SUCCESS) {
		printf ("Error: MPI failed to start\n");
		MPI_Abort(MPI_COMM_WORLD, -10);
		exit (-10);
	}


	/* define mpi_o_type for sending and receiving */
	blockcounts[0] = 1;
	blockcounts[1] = intvar;
	blockcounts[2] = intvar;
	offsets[0] = offsetof(mpi_organism,size);
	offsets[1] = offsetof(mpi_organism,xcoords);
	offsets[2] = offsetof(mpi_organism,ycoords);
	MPI_Type_create_struct(3,blockcounts,offsets,oldtypes,&mpi_o_type);
	MPI_Type_commit(&mpi_o_type);

	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_id);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
	init_problem(&problem, intvar, argv[2]);

	transfers = POP_SIZE/2;

	new_pop = (mpi_organism*) malloc(transfers*sizeof(mpi_organism));
	
	for(i=0;i<NR_OF_IT;i++){
		spawn_next_gen(&problem);
		printf("%i;%i;%f\n", mpi_id, i, fitness(problem.population[0]));
		old_pop = create_mpi_pop(problem.population,transfers);
		for(j=0;j<transfers;j++){
			old_ind = old_pop[j];
			MPI_Isend(&old_ind, 1, mpi_o_type, (mpi_id+1)%mpi_size, j, MPI_COMM_WORLD, &mpi_request);
		}
		for(j=0;j<transfers;j++){
			MPI_Recv(&new_pop[j], 1, mpi_o_type, MPI_ANY_SOURCE, j, MPI_COMM_WORLD, &mpi_status);
		}
		update_population(&problem, new_pop, transfers);

		free(old_pop);
	}
	free(new_pop);



	if(mpi_id != 0){
		old_ind = create_mpi_organism(problem.population[0]);
		MPI_Isend(&old_ind, 1, mpi_o_type, 0, 1, MPI_COMM_WORLD, &mpi_request);
	} else {
		/* thread 0 takes care of final output */
		int i=1;
		individual* current_sol;
		double fitness_value, best_fit;
		mpi_organism* best_mpi = (mpi_organism*) malloc((mpi_size-1)*sizeof(mpi_organism));
		best_fit = fitness(problem.population[0]);
		while(i<mpi_size){
			MPI_Recv(&best_mpi[i-1], 1, mpi_o_type, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &mpi_status);
			current_sol = create_natural_organism(best_mpi[i-1]);
			fitness_value = fitness(current_sol);
			if(fitness_value>best_fit){
				best_fit = fitness_value;
				problem.population[0] = current_sol;
			} else {
				free_individual(current_sol);
			}
			i++;
		}
		free(best_mpi);
		printf("%f\n",best_fit);
		for(i=0;i<intvar;i++){
			printf("%f %f\n",problem.population[0]->points[i]->x, problem.population[0]->points[i]->y);
		}
	}
	free_problem(&problem);

	mpi_res = MPI_Finalize();
	if (mpi_res != MPI_SUCCESS) {
		printf ("Error: MPI failure\n");
		exit (-11);
	}

	getchar();
	//_CrtDumpMemoryLeaks();
	return 0;
}

/* creates a starting population for the given problem */
void create_population(opt_problem* problem){
	int i,j;
	individual* new_ind;

	srand(time(NULL)*(mpi_id+1));
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

/* turn an individual into a MPI parseable struct */
mpi_organism create_mpi_organism(individual* ind){
	int i;
	mpi_organism mpio;
	mpio.size = ind->size;
	for(i=0;i<mpio.size;i++){
		mpio.xcoords[i]= ind->points[i]->x;
		mpio.ycoords[i]= ind->points[i]->y;
	}
	return mpio;
}

/* turn a MPI organism into an individual */
individual* create_natural_organism(mpi_organism mpio){
	int i;
	individual* ind;
	point** points = (point**) malloc(sizeof(point*)*mpio.size);
	for(i=0;i<mpio.size;i++){
		points[i] = create_point(mpio.xcoords[i],mpio.ycoords[i]);
	}
	ind = create_individual(points, mpio.size);
	
	for(i=0;i<mpio.size;i++){
		free(points[i]);
	}
	free(points);
	
	return ind;
}

mpi_organism* create_mpi_pop(individual** population, int number){
	// halve pop size
	int i;
	mpi_organism* mpi_pop = (mpi_organism*) malloc(number*sizeof(mpi_organism));
	for(i=0;i<number;i++){
		individual* ind = population[i];
		mpi_pop[i] = create_mpi_organism(ind);
	}
	return mpi_pop;
}

void update_population(opt_problem* problem, mpi_organism* new_pop, int transfers){
	int i;
	for(i=1;i<=transfers;i++){
		free_individual(problem->population[POP_SIZE-i]);
		problem->population[POP_SIZE-i]=create_natural_organism(new_pop[i-1]);
	}
}