//// @file energie.h
/// @brief Déclaration des fonctions pour calculer l'énergie cinétique et potentielle

#ifndef ENERGIE_H
#define ENERGIE_H

#include "vectors.h"

double energie_cinetique(struct Vecteurs v, double m_terre);

double energie_potentielle(struct Vecteurs r, double m_soleil, double m_terre, double g);

#endif