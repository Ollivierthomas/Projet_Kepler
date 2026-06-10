#include "vectors.h"
#include "energie.h"
#include <math.h>

double energie_cinetique(struct Vecteurs v, double m_terre) {
    double n_v = sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
    double e_cinetique = 0.5 * m_terre * (n_v * n_v);
    return e_cinetique;
}

double energie_potentielle(struct Vecteurs r, double m_soleil, double m_terre, double g) {
    double n_r = sqrt((r.x * r.x) + (r.y * r.y) + (r.z * r.z));
    double e_potentielle = -g * (m_soleil * m_terre) / n_r;
    return e_potentielle;
}