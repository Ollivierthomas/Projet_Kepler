/// @file trajectoire.h
/// @brief Déclaration des fonctions pour simuler la trajectoire d'une planète autour du Soleil

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

/// Exporte les données de la simulation pour une planète donnée dans un fichier JSON
/// @param filename Nom du fichier de sortie
/// @param array_positions Tableau dynamique contenant les positions de la planète
/// @param array_cinetique Tableau dynamique contenant les énergies cinétiques de la planète
/// @param array_potentielle Tableau dynamique contenant les énergies potentielles de la planète
/// @param h Pas de temps utilisé dans la simulation
void export_json_planete(char* filename, Array(Vecteurs)* array_positions, Array(double)* array_cinetique, Array(double)* array_potentielle, double h);

/// @brief Simule la trajectoire d'une planète autour du Soleil en utilisant la méthode d'Euler symplectique
/// @param planete Structure contenant les paramètres de la planète (nom, masse, rayon périhélie, rayon aphélie)
/// @param G Constante gravitationnelle
/// @param m_soleil Masse du Soleil
/// @return Tableau dynamique contenant les positions de la planète à chaque pas de temps  
Array(Vecteurs) simuler_planete(Planete planete, double G, double m_soleil);

#endif