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


/// Exporte les données de la simulation dans un fichier JSON
/// @param filename Nom du fichier de sortie
/// @param array_positions Tableau dynamique contenant les positions de la Terre
/// @param array_cinetique Tableau dynamique contenant les énergies cinétiques de la Terre
/// @param array_potentielle Tableau dynamique contenant les énergies potentielles de la Terre
/// @param h Pas de temps utilisé dans la simulation   
void export_json(char* filename, Array(Vecteurs)* array_positions, Array(double)* array_cinetique, Array(double)* array_potentielle, double h) {
    FILE *f = fopen(filename, "w");

    if (f == NULL) {
        perror("Erreur ouverture fichier JSON\n");
        exit(EXIT_FAILURE);
    }

    fprintf(f, "[\n");

    for (size_t i = 0; i < array_positions->size; i++) {
        double t = i * h;

        fprintf(f, " { \"t\": %.6E, \"x\": %.6E, \"y\": %.6E, \"z\": %.6E, \"ke\": %.6E, \"pe\": %.6E}", t, array_positions->values[i].x, array_positions->values[i].y, array_positions->values[i].z, array_cinetique->values[i], array_potentielle->values[i]);

        if (i < array_positions->size - 1) {
            fprintf(f, ",\n");
        }
    }

    fprintf(f, "]\n");

    fclose(f);
}

int main() {
    double G = 6.67430e-11;
    double m_soleil = 1.989e30;
    double m_terre = 5.972e24;
    double m_lune = 7.35e22;

    double r_per = 356700e3; // périhélie
    double r_aph = 	406300e3; // aphélie

    double a = (r_per + r_aph) / 2.0;
    double e = (r_aph - r_per) / (r_aph + r_per);

    double v_per = sqrt((G * m_terre * (1.0 + e)) / (a * (1.0 - e)));

    double h = 600.0;

    double T = 2.0 * PI *sqrt((a * a * a) / (G * m_terre));

    size_t n = (size_t)round(T / h);

    Vecteurs r = {.x = r_per, .y = 0, .z = 0};
    Vecteurs v = {.x = 0, .y = v_per, .z = 0};

    Array(Vecteurs) array_positions = {0};
    Array(Vecteurs) array_vitesses = {0};

    Array(double) array_cinetique = {0};
    Array(double) array_potentielle = {0};

    array_push(array_positions, r);
    array_push(array_vitesses, v);

    array_push(array_cinetique, energie_cinetique(v, m_lune));
    array_push(array_potentielle, energie_potentielle(r, m_terre, m_lune, G));


    for (size_t i = 1; i <= n; i++) {
        Vecteurs r_old = array_positions.values[array_positions.size - 1];
        Vecteurs v_old = array_vitesses.values[array_vitesses.size - 1];

        Vecteurs a_old = accelerations(G, m_terre, r_old);

        Vecteurs v_next = vitesses(v_old, h, a_old);
        Vecteurs r_next = positions(r_old, h, v_old);

        array_push(array_positions, r_next);
        array_push(array_vitesses, v_next);

        array_push(array_cinetique, energie_cinetique(v_next, m_lune));
        array_push(array_potentielle, energie_potentielle(r_next, m_terre, m_lune, G));
    }
    
    array_push(array_positions, r);
    array_push(array_vitesses, v);

    export_json("./data_lune.json", &array_positions, &array_cinetique, &array_potentielle, h);

    array_free(array_positions);
    array_free(array_vitesses);
    array_free(array_cinetique);
    array_free(array_potentielle);
}
