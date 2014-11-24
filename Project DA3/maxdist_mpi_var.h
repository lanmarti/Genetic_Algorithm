/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#ifndef _MAXDIST_MPI_HEADER_
#define _MAXDIST_MPI_HEADER_

#include "organism.h"

typedef struct{
	int size;
	double xcoords[5];
	double ycoords[5];
} mpi_organism;


int POP_SIZE;
int NR_OF_IT;
int NR_OF_PARENTS;
int MUTATION_CHANCE;
int MUTATION_MAX;

int main();

/* functions for setting up problem */
void init_problem(opt_problem* problem, int nr_of_points, char* filename);
void free_problem(opt_problem* problem);
void create_population(opt_problem* problem);
void update_population(opt_problem* problem, individual** new_pop, int transfers);

/* MPI functions */
void define_mpi_type();
individual** create_mpi_pop(opt_problem* problem, int number);
individual* receive_organism(int tag, MPI_Status* status);
void send_organism(individual* ind, int dest, int tag, MPI_Request* mpi_request);

#endif


