#include "energie.h"
#include "euler.h"
#include "vectors.h"
#include "tab_generic.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void export_json(char* filename, Array(Vecteurs)* array_positions, Array(double)* array_cinetique, Array(double)* array_potentielle, double h) {
    FILE *f = fopen(filename, "w");

    if (f == NULL) {
        printf("Erreur ouverture fichier JSON\n");
        return;
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
    double m_soleil = 1.9884e30;
    double m_terre = 5.9736e24;

    double r_per = 147098291e3; // périhélie
    double a = 149597887.5e3;
    double e = 0.01671022;

    double v_per = sqrt((G * m_soleil * (1.0 + e)) / (a * (1.0 - e)));

    Vecteurs r = {.x = r_per, .y = 0, .z = 0};
    Vecteurs v = {.x = 0, .y = v_per, .z = 0};

    double h = 3600.0;
    double T = 365.25 * 24.0 * 3600.0; // 1 an en secondes
    int n = (int)(T / h);

    Array(Vecteurs) array_positions = {0};
    Array(Vecteurs) array_vitesses = {0};

    Array(double) array_cinetique = {0};
    Array(double) array_potentielle = {0};

    array_push(array_positions, r);
    array_push(array_vitesses, v);

    array_push(array_cinetique, energie_cinetique(v, m_terre));
    array_push(array_potentielle, energie_potentielle(r, m_soleil, m_terre, G));


    for (int i = 1; i <= n; i++) {
        Vecteurs r_old = array_positions.values[array_positions.size - 1];
        Vecteurs v_old = array_vitesses.values[array_vitesses.size - 1];

        Vecteurs a_old = accelerations(G, m_soleil, r_old);

        Vecteurs r_next = positions(r_old, h, v_old);
        Vecteurs v_next = vitesses(v_old, h, a_old);

        array_push(array_positions, r_next);
        array_push(array_vitesses, v_next);

        array_push(array_cinetique, energie_cinetique(v_next, m_terre));
        array_push(array_potentielle, energie_potentielle(r_next, m_soleil, m_terre, G));
    }
    
    export_json("data.json", &array_positions, &array_cinetique, &array_potentielle, h);

    array_free(array_positions);
    array_free(array_vitesses);
    array_free(array_cinetique);
    array_free(array_potentielle);
}
