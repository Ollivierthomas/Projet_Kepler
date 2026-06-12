#ifndef TRAJECTOIRE_H
#define TRAJECTOIRE_H

#include "vectors.h"
#include "tab_generic.h"
#include <raylib.h>
#include <stddef.h>


// Structure pour stocker les paramètres d'une planète
typedef struct Planete {
    const char* nom;
    double masse;
    double r_per;
    double r_aph;
} Planete;


Array(Vecteurs) simuler_planete(Planete planete, double G, double m_soleil);

void export_json_planete(char* filename, Array(Vecteurs)* array_positions, Array(double)* array_cinetique, Array(double)* array_potentielle, double h);

#endif