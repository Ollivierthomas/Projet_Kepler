all: kepler

energie.o: energie.c energie.h vectors.h
	gcc -c energie.c -lm

euler.o: euler.c euler.h vectors.h
	gcc -c euler.c -lm

trajectoire.o: trajectoire.c energie.h euler.h vectors.h tab_generic.h
	gcc -c trajectoire.c -lm -std=c23

kepler: energie.o euler.o trajectoire.o
	gcc -o kepler energie.o euler.o trajectoire.o -lm -std=c23