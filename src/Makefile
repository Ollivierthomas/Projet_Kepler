all: kepler systeme_solaire

energie.o: energie.c energie.h vectors.h
	gcc -pipe -c energie.c -lm

euler.o: euler.c euler.h vectors.h
	gcc -pipe -c euler.c -lm

trajectoire.o: trajectoire.c energie.h euler.h vectors.h tab_generic.h
	gcc -pipe -c trajectoire.c -lm -std=c23

systeme_solaire: systeme_solaire.c
	gcc systeme_solaire.c -o systeme_solaire -std=c23 -I/usr/local/include -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lXrandr -lXi -lXcursor -lXinerama

kepler: energie.o euler.o trajectoire.o
	gcc -o kepler energie.o euler.o trajectoire.o -lm -std=c23
