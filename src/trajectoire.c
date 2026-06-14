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

Array(Vecteurs) simuler_planete(Planete planete, double G, double m_soleil) {
    double r_per = pow(planete.r_per, 0.95);
    double r_aph = pow(planete.r_aph, 0.95);

    double a = (r_per + r_aph) / 2.0;
    double e = (r_aph - r_per) / (r_aph + r_per);
    double v_per = sqrt((G * m_soleil * (1.0 + e)) / (a * (1.0 - e)));
    double T = 2.0 * PI * sqrt((a * a * a) / (G * m_soleil));

    double h = 3600;
    size_t n = (size_t)round(T / h);

    Vecteurs r = {.x = r_per, .y = 0, .z = 0};
    Vecteurs v = {.x = 0, .y = v_per, .z = 0};

    Array(Vecteurs) array_positions  = {0};
    Array(Vecteurs) array_vitesses   = {0};
    Array(double)   array_cinetique  = {0};
    Array(double)   array_potentielle = {0};

    array_push(array_positions,  r);
    array_push(array_vitesses,   v);
    array_push(array_cinetique,  energie_cinetique(v, planete.masse));
    array_push(array_potentielle, energie_potentielle(r, m_soleil, planete.masse, G));

    for (size_t i = 1; i <= n; i++) {
        Vecteurs r_old = array_positions.values[array_positions.size - 1];
        Vecteurs v_old = array_vitesses.values[array_vitesses.size - 1];

        Vecteurs a_old = accelerations(G, m_soleil, r_old);

        // ✅ Euler symplectique : v_next d'abord, puis r_next avec v_next
        Vecteurs v_next = vitesses(v_old, h, a_old);
        Vecteurs r_next = positions(r_old, h, v_next);  // ← correction ici

        array_push(array_positions,   r_next);
        array_push(array_vitesses,    v_next);
        array_push(array_cinetique,   energie_cinetique(v_next, planete.masse));
        array_push(array_potentielle, energie_potentielle(r_next, m_soleil, planete.masse, G));
    }

    // ✅ Libération correcte : on garde positions, on libère le reste
    array_free(array_vitesses);
    array_free(array_cinetique);
    array_free(array_potentielle);

    return array_positions;  // ← return après les free
}