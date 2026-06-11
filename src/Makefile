all: kepler test_raylib

energie.o: energie.c energie.h vectors.h
	gcc -c energie.c -lm

euler.o: euler.c euler.h vectors.h
	gcc -c euler.c -lm

trajectoire.o: trajectoire.c energie.h euler.h vectors.h tab_generic.h
	gcc -c trajectoire.c -lm -std=c23

test_raylib: test_raylib.c
	gcc test_raylib.c -o test_raylib -I/usr/local/include -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lXrandr -lXi -lXcursor -lXinerama

kepler: energie.o euler.o trajectoire.o
	gcc -o kepler energie.o euler.o trajectoire.o -lm -std=c23