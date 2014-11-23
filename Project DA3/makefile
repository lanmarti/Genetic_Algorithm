all: maxdist

maxdist_mpi: maxdist_mpi.o genetics.o organism.o functions.o
	gcc maxdist_mpi.c genetics.c organism.c functions.c -o maxdist_mpi -lm

maxdist: maxdist.o genetics.o organism.o functions.o
	gcc  maxdist.c genetics.c organism.c functions.c -o maxdist -lm

maxdist_mpi_var: maxdist_mpi_var.o genetics.o organism.o functions.o
	gcc maxdist_mpi_var.c genetics.c organism.c functions.c -o maxdist_mpi_var -lm

genetics.o: genetics.c
	gcc -c genetics.c -lm

organism.o: organism.c
	gcc -c organism.c -lm

functions.o: functions.c
	gcc -c functions.c -lm

maxdist.o: maxdist.c
	gcc -c maxdist.c -lm

clean:
	rm -rf *.o maxdist maxdist_mpi maxdist_mpi_var
