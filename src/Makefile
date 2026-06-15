all: systeme_solaire systeme2
RAYLIB_FLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all: systeme2

energie.o: energie.c energie.h vectors.h
	gcc -pipe -c energie.c

euler.o: euler.c euler.h vectors.h
	gcc -pipe -c euler.c

trajectoire.o: trajectoire.c trajectoire.h energie.h euler.h vectors.h tab_generic.h
	gcc -pipe -c trajectoire.c -std=c23

systeme_solaire.o: systeme_solaire.c trajectoire.h vectors.h tab_generic.h
	gcc -pipe -c systeme_solaire.c -std=c23

systeme_solaire: systeme_solaire.o trajectoire.o energie.o euler.o
	gcc systeme_solaire.o trajectoire.o energie.o euler.o -o systeme_solaire -std=c23 $(RAYLIB_FLAGS)

	gcc -pipe -c trajectoire.c -lm -std=c23

systeme.o: systeme.c trajectoire.h vectors.h tab_generic.h
	gcc -pipe -c systeme.c -std=c23

systeme: systeme.o trajectoire.o energie.o euler.o
	gcc systeme.o trajectoire.o energie.o euler.o -o systeme -std=c23 $(RAYLIB_FLAGS)

systeme2.o: systeme2.c trajectoire.h vectors.h tab_generic.h
	gcc -pipe -c systeme2.c -std=c23

systeme2: systeme2.o trajectoire.o energie.o euler.o
	gcc systeme2.o trajectoire.o energie.o euler.o -o systeme2 -std=c23 $(RAYLIB_FLAGS)

systeme2.o: systeme2.c trajectoire.h vectors.h tab_generic.h
	gcc -pipe -c systeme2.c -std=c23

systeme2: systeme2.o trajectoire.o energie.o euler.o
	gcc systeme2.o trajectoire.o energie.o euler.o -o systeme2 -std=c23 $(RAYLIB_FLAGS)

kepler: energie.o euler.o trajectoire.o
	gcc -o kepler energie.o euler.o trajectoire.o -lm -std=c23

clean:
	rm -f *.o kepler systeme systeme_solaire
clean:
	rm -f *.o kepler systeme systeme2
