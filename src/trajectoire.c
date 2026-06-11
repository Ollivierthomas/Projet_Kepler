/// @file trajectoire.c
/// @brief Simulation de la trajectoire de la Terre autour du Soleil en utilisant la méthode d'Euler

#include "energie.h"
#include "euler.h"
#include "vectors.h"
#include "tab_generic.h"

#include <math.h>
#include <raylib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


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

void simuler_planete(Planete planete, double G, double m_soleil){
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
    // Génération du nom de fichier pour la planète
    char filename[128];
    snprintf(filename, sizeof(filename), "planetes/data_%s.json", planete.nom);
    // Exportation des données de la simulation dans un fichier JSON
    export_json_planete(filename, &array_positions, &array_cinetique, &array_potentielle, h);

    array_free(array_positions);
    array_free(array_vitesses);
    array_free(array_cinetique);
    array_free(array_potentielle);


}

/// Point d'entrée du programme
int main() {
    double G = 6.67430e-11;
    double m_soleil = 1.989e30;

    struct Planete planetes[] = {
    { "mercure", 3.3011e23, 46.00e9,   69.82e9 },
    { "venus",   4.8675e24, 107.48e9, 108.94e9 },
    { "terre",   5.9720e24, 147.09e9, 152.10e9 },
    { "mars",    6.4171e23, 206.62e9, 249.23e9 },
    { "jupiter", 1.8982e27, 740.52e9, 816.62e9 },
    { "saturne", 5.6834e26, 1352.55e9, 1514.50e9 },
    { "uranus",  8.6810e25, 2741.30e9, 3003.62e9 },
    { "neptune", 1.02413e26, 4444.45e9, 4545.67e9 }
    };

    size_t nb_planetes = sizeof(planetes)/sizeof(planetes[0]);

    for (size_t i = 0; i < nb_planetes; i++) {
        simuler_planete(planetes[i], G, m_soleil);
    }

    return 0;
    
}
