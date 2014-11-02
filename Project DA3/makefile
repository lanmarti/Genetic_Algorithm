all: maxdist

maxdist: maxdist.o genetics.o organism.o functions.o
	gcc  maxdist.c genetics.c organism.c functions.c -o maxdist -lm

genetics.o: genetics.c
	gcc -c genetics.c -lm

organism.o: organism.c
	gcc -c organism.c -lm

functions.o: functions.c
	gcc -c functions.c -lm

maxdist.o: maxdist.c
	gcc -c maxdist.c -lm

clean:
	rm -rf *.o maxdist