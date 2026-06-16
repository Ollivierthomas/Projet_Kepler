//// @file euler.h
/// @brief Déclaration des fonctions pour la méthode d'Euler

#ifndef EULER_H
#define EULER_H

/// Calcule la position suivante en utilisant la méthode d'Euler
/// @param r Vecteur de position actuelle
/// @param h Pas de temps
/// @param v Vecteur de vitesse actuelle
/// @return Vecteur de position suivante
struct Vecteurs positions(struct Vecteurs r, double h, struct Vecteurs v);

/// Calcule la vitesse suivante en utilisant la méthode d'Euler
/// @param v Vecteur de vitesse actuelle
/// @param h Pas de temps
/// @param a Vecteur d'accélération actuelle
/// @return Vecteur de vitesse suivante
struct Vecteurs vitesses(struct Vecteurs v, double h, struct Vecteurs a);

/// Calcule l'accélération actuelle en fonction de la position actuelle
/// @param g Constante gravitationnelle
/// @param m_soleil Masse du Soleil
/// @param r Vecteur de position actuelle
/// @return Vecteur d'accélération actuelle
struct Vecteurs accelerations(double g, double m_soleil, struct Vecteurs r);

#endif 