/// @file trajectoire.c
/// @brief Simulation de la trajectoire de la Terre autour du Soleil en utilisant la méthode d'Euler

#include "energie.h"
#include "euler.h"
#include "vectors.h"
#include "tab_generic.h"
#include "trajectoire.h"

#include <math.h>
#include <raylib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/// Exporte les données de la simulation pour une planète donnée dans un fichier JSON
/// @param filename Nom du fichier de sortie
/// @param array_positions Tableau dynamique contenant les positions de la planète
/// @param array_cinetique Tableau dynamique contenant les énergies cinétiques de la planète
/// @param array_potentielle Tableau dynamique contenant les énergies potentielles de la planète
/// @param h Pas de temps utilisé dans la simulation
void export_json_planete(char* filename, Array(Vecteurs)* array_positions, Array(double)* array_cinetique, Array(double)* array_potentielle, double h) {
    // Ouvre le fichier en mode écriture
    FILE *f = fopen(filename, "w");

    if (f == NULL) {
        perror("Erreur ouverture fichier JSON\n");
        exit(EXIT_FAILURE);
    }

    fprintf(f, "[\n");
    // Parcourt les tableaux dynamiques et écrit les données de chaque point dans le fichier JSON
    for (size_t i = 0; i < array_positions->size; i++) {
        double t = i * h;

        // Écrit les données de la position, de l'énergie cinétique et de l'énergie potentielle dans le format JSON
        fprintf(f," { \"t\": %.6E, \"x\": %.6E, \"y\": %.6E, \"z\": %.6E, \"ke\": %.6E, \"pe\": %.6E}",t,
            array_positions->values[i].x,array_positions->values[i].y,array_positions->values[i].z,array_cinetique->values[i],array_potentielle->values[i]);
        // Ajoute une virgule après chaque point sauf le dernier
        if (i < array_positions->size - 1) {
            fprintf(f, ",\n");
        }
    }

    fprintf(f, "]\n");

    fclose(f);
}
/// Simule la trajectoire d'une planète autour du Soleil en utilisant la méthode d'Euler
/// @param planete Structure contenant les paramètres de la planète (masse, périhélie, aphélie)
/// @param G Constante gravitationnelle
/// @param m_soleil Masse du Soleil

Array(Vecteurs) simuler_planete(Planete planete, double G, double m_soleil) {
    double r_per = planete.r_per; // périhélie
    double r_aph = planete.r_aph; // aphélie

    double a = (r_per + r_aph) / 2.0;
    double e = (r_aph - r_per) / (r_aph + r_per);

    double v_per = sqrt((G * m_soleil * (1.0 + e)) / (a * (1.0 - e)));

    double T = 2.0 * PI *sqrt((a * a * a) / (G * m_soleil));

    double h = 3600;
    size_t n = (size_t)round(T / h);

    Vecteurs r = {.x = r_per, .y = 0, .z = 0};
    Vecteurs v = {.x = 0, .y = v_per, .z = 0};

    Array(Vecteurs) array_positions = {0};
    Array(Vecteurs) array_vitesses = {0};
    Array(double) array_cinetique = {0};
    Array(double) array_potentielle = {0};

    array_push(array_positions, r);
    array_push(array_vitesses, v);

    array_push(array_cinetique, energie_cinetique(v, planete.masse));
    array_push(array_potentielle, energie_potentielle(r, m_soleil, planete.masse, G));

    // Simulation de la trajectoire de la planète
    for (size_t i = 1; i <= n; i++) {
        // Récupération de la dernière position et vitesse
        Vecteurs r_old = array_positions.values[array_positions.size - 1];
        Vecteurs v_old = array_vitesses.values[array_vitesses.size - 1];

        // Calcul de l'accélération à partir de la position actuelle
        Vecteurs a_old = accelerations(G, m_soleil, r_old);

        // Mise à jour de la position et de la vitesse en utilisant la méthode d'Euler
        Vecteurs v_next = vitesses(v_old, h, a_old);
        Vecteurs r_next = positions(r_old, h, v_old);

        // Stockage des nouvelles positions, vitesses, énergies cinétiques et potentielles dans les tableaux dynamiques
        array_push(array_positions, r_next);
        array_push(array_vitesses, v_next);

        // Calcul de l'énergie cinétique et potentielle à partir de la nouvelle position et vitesse
        array_push(array_cinetique, energie_cinetique(v_next, planete.masse));
        array_push(array_potentielle, energie_potentielle(r_next, m_soleil, planete.masse, G));
    }

    array_push(array_positions, r);
    array_push(array_vitesses, v);

    return array_positions;

    array_free(array_positions);
    array_free(array_vitesses);
    array_free(array_cinetique);
    array_free(array_potentielle);
}