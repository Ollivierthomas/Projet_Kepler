//// @file energie.h
/// @brief Déclaration des fonctions pour calculer l'énergie cinétique et potentielle

#ifndef ENERGIE_H
#define ENERGIE_H

#include "vectors.h"

/// Calcule l'énergie cinétique d'un objet
/// @param v Vecteur de vitesse de l'objet
/// @param m_terre Masse de la terre
/// @return L'énergie cinétique de l'objet
double energie_cinetique(struct Vecteurs v, double m_terre);

/// Calcule l'énergie potentielle d'un objet
/// @param r Vecteur de position de l'objet
/// @param m_soleil Masse du Soleil
/// @param m_terre Masse de la terre
/// @param g Constante gravitationnelle
/// @return L'énergie potentielle de l'objet
double energie_potentielle(struct Vecteurs r, double m_soleil, double m_terre, double g);

#endif