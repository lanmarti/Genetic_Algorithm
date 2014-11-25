/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#include <stddef.h>

#include "genetics.h"
#include "organism.h"
#include "functions.h"
#include "maxdist_mpi_var.h"


//#define _crtdbg_map_alloc
//#include <stdlib.h>
//#include <crtdbg.h>

int mpi_id, points_nr;
MPI_Datatype mpi_o_type;

int main(int argc, char* argv []) {
	int i,j,intvar,mpi_res,mpi_size,transfers;
	opt_problem problem;
	MPI_Status mpi_status;
	MPI_Request mpi_request;

	individual** old_pop;
	individual** new_pop;

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
	points_nr = intvar;
	mpi_res = MPI_Init(&argc, &argv);
	if (mpi_res != MPI_SUCCESS) {
		printf ("Error: MPI failed to start\n");
		MPI_Abort(MPI_COMM_WORLD, -10);
		exit (-10);
	}

	define_mpi_type();

	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_id);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
	init_problem(&problem, intvar, argv[2]);

	transfers = POP_SIZE/2;

	new_pop = (individual**) malloc(transfers*sizeof(individual*));
	
	for(i=0;i<NR_OF_IT;i++){
		old_pop = create_mpi_pop(&problem,transfers);
		for(j=0;j<transfers;j++){
			send_organism(old_pop[j],(mpi_id+1)%mpi_size,j,&mpi_request);
		}
		for(j=0;j<transfers;j++){
			new_pop[j] = receive_organism(j,&mpi_status);
		}
		update_population(&problem, new_pop, transfers);
		spawn_next_gen(&problem);
		free(old_pop);
		printf(";%i;%i;%f\n", mpi_id, i, fitness(problem.population[0]));
	}
	free(new_pop);

	if(mpi_id != 0){
		send_organism(problem.population[0],0,1,&mpi_request);
	} else {
		/* thread 0 takes care of final output */
		int i=1;
		individual* current_sol;
		double fitness_value, best_fit;
		best_fit = fitness(problem.population[0]);
		while(i<mpi_size){
			current_sol = receive_organism(1,&mpi_status);
			fitness_value = fitness(current_sol);
			if(fitness_value>best_fit){
				best_fit = fitness_value;
				problem.population[0] = current_sol;
			} else {
				free_individual(current_sol);
			}
			i++;
		}
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

	//getchar();
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

/* turn an organism into a MPI sendable struct and send it */
void send_organism(individual* ind, int dest, int tag, MPI_Request* mpi_request){
	int i;
	typedef struct mpi_organism {
		int size;
		double xcoords[points_nr];
		double ycoords[points_nr];
	} mpi_organism;
	mpi_organism* mpiorg = (mpi_organism*) malloc(1 * sizeof(struct mpi_organism));

	mpiorg->size = ind->size;
	for(i=0;i<mpiorg->size;i++){
		mpiorg->xcoords[i]= ind->points[i]->x;
		mpiorg->ycoords[i]= ind->points[i]->y;
	}
	MPI_Send(mpiorg, 1, mpi_o_type, dest, tag, MPI_COMM_WORLD);
	free(mpiorg);
}

/* turn a MPI organism into an individual */
individual* receive_organism(int tag, MPI_Status* status){
	int i;
	individual* ind;
	point** points;
	struct mpi_organism {
		int size;
		double xcoords[points_nr];
		double ycoords[points_nr];
	} mpio;
	
	MPI_Recv(&mpio, 1, mpi_o_type, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, status);
	
	points = (point**) malloc(sizeof(point*)*mpio.size);
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

individual** create_mpi_pop(opt_problem* problem, int number){
	int i;
	individual** mpi_pop = (individual**) malloc(number*sizeof(individual*));
	for(i=1;i<=number;i++){
		mpi_pop[i-1] = problem->population[(int) rand()%POP_SIZE];
	}
	return mpi_pop;
}

void update_population(opt_problem* problem, individual** new_pop, int transfers){
	int i;
	for(i=1;i<=transfers;i++){
		free_individual(problem->population[POP_SIZE-i]);
		problem->population[POP_SIZE-i]=new_pop[i-1];
	}
}

/* define mpi_o_type for sending and receiving */
void define_mpi_type(){	
	MPI_Aint offsets[3];
	MPI_Datatype oldtypes[3] = { MPI_INT, MPI_DOUBLE, MPI_DOUBLE};
	int blockcounts[3];
	struct mpi_organism {
		int size;
		double xcoords[points_nr];
		double ycoords[points_nr];
	};
	blockcounts[0] = 1;
	blockcounts[1] = points_nr;
	blockcounts[2] = points_nr;
	offsets[0] = offsetof(struct mpi_organism,size);
	offsets[1] = offsetof(struct mpi_organism,xcoords);
	offsets[2] = offsetof(struct mpi_organism,ycoords);
	MPI_Type_create_struct(3,blockcounts,offsets,oldtypes,&mpi_o_type);
	MPI_Type_commit(&mpi_o_type);
}