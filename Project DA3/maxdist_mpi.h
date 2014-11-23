/***********************************************************
* Naam student: Laurens Martin                             *
***********************************************************/

#ifndef _MAXDIST_MPI_HEADER_
#define _MAXDIST_MPI_HEADER_

#include "organism.h"

int POP_SIZE = 100;
int NR_OF_IT = 50;
int NR_OF_PARENTS = 60;
int MUTATION_CHANCE = 60;
int MUTATION_MAX = 0.1;
int nr_of_points;

int main();

/* functions for setting up problem */
void init_problem(opt_problem* problem, int nr_of_points, char* filename);
void free_problem(opt_problem* problem);
void create_population(opt_problem* problem);
void update_population(opt_problem* problem, individual** new_pop, int transfers);

/* MPI functions */
void define_mpi_type();
individual** create_mpi_pop(individual** population, int number);
individual* receive_organism(int tag, MPI_Status* status);
void send_organism(individual* ind, int dest, int tag, MPI_Request* mpi_request);

#endif