///@file euler.c
///@brief Implémentation des fonctions pour la méthode d'Euler

#include "vectors.h"
#include "euler.h"
#include <math.h>

/// Calcule la position suivante en utilisant la méthode d'Euler
/// @param r Vecteur de position actuelle
/// @param h Pas de temps
/// @param v Vecteur de vitesse actuelle
/// @return Vecteur de position suivante
struct Vecteurs positions(struct Vecteurs r, double h, struct Vecteurs v) {
    struct Vecteurs r_next = {.x = 0, .y = 0, .z = 0};
    r_next.x = r.x + h * v.x;
    r_next.y = r.y + h * v.y;
    r_next.z = r.z + h * v.z;
    return r_next;
}


/// Calcule la vitesse suivante en utilisant la méthode d'Euler
/// @param v Vecteur de vitesse actuelle
/// @param h Pas de temps
/// @param a Vecteur d'accélération actuelle
/// @return Vecteur de vitesse suivante
struct Vecteurs vitesses(struct Vecteurs v, double h, struct Vecteurs a) {
    struct Vecteurs v_next = {.x = 0, .y = 0, .z = 0};
    v_next.x = v.x + h * a.x;
    v_next.y = v.y + h * a.y;
    v_next.z = v.z + h * a.z;
    return v_next;
}


/// Calcule l'accélération actuelle en fonction de la position actuelle
/// @param g Constante gravitationnelle
/// @param m_soleil Masse du Soleil
/// @param r Vecteur de position actuelle
/// @return Vecteur d'accélération actuelle
struct Vecteurs accelerations(double g, double m_soleil, struct Vecteurs r) {
    struct Vecteurs a = {.x = 0, .y = 0, .z = 0};
    double n_r = sqrt((r.x * r.x) + (r.y * r.y) + (r.z * r.z));
    a.x = -g * (m_soleil / (n_r * n_r * n_r)) * r.x;
    a.y = -g * (m_soleil / (n_r * n_r * n_r)) * r.y;
    a.z = -g * (m_soleil / (n_r * n_r * n_r)) * r.z;
    return a;
}