/// @file systeme_solaire.h
/// @brief Déclaration des structures et fonctions pour la simulation d'un système solaire avec effet de supernova.

#ifndef SYSTEME_SOLAIRE_H
#define SYSTEME_SOLAIRE_H

#include "raylib.h"
#include "tab_generic.h"
#include "trajectoire.h"

#define ECHELLE_DISTANCE_AFFICHAGE 3.5e-4
#define ECHELLE_DISTANCE_LUNE 1.5f
#define INDEX_TERRE 2
#define NOMBRE_PLANETES 8
#define DEBRIS_PAR_PLANETE 60

// Durees des phases (secondes)
#define DUREE_GONFLEMENT 3.0f
#define DUREE_FLASH 0.5f
#define DUREE_EXPLOSION 6.0f
#define DUREE_FONDU 2.0f

// Parametres de la camera orbitale (souris)
#define CAMERA_DISTANCE_MIN 50.0f
#define CAMERA_DISTANCE_MAX 1200.0f
#define CAMERA_VITESSE_ROTATION 0.005f
#define CAMERA_VITESSE_ZOOM 20.0f
#define CAMERA_VITESSE_DEPLACEMENT 2.0f

/// @brief Tableau dynamique représentant une trajectoire.
typedef Array(Vecteurs) Trajectoire;

/// @brief Énumération des différentes phases de l'animation de la supernova.
typedef enum {
    PHASE_NORMAL = 0,
    PHASE_GONFLEMENT,
    PHASE_FLASH,
    PHASE_EXPLOSION,
    PHASE_FONDU
} PhaseSupernova;

/// @brief Structure représentant un fragment (débris) lors d'une explosion.
typedef struct {
    Vector3 position;
    Vector3 vitesse;
    Vector3 axe_rotation;   // axe de rotation propre du fragment
    float vitesse_rotation; // vitesse de rotation (rad/s)
    float angle_rotation;   // angle courant accumule
    float rayon;
    Color couleur;
    float vie;              // 1.0 -> 0.0
} Debris;

/// @brief Structure gérant un ensemble de débris pour une planète.
typedef struct {
    Debris items[DEBRIS_PAR_PLANETE];
    int actif;
} GroupeDebris;

/// @brief Structure de la caméra orbitale contrôlée à la souris.
typedef struct {
    Vector3 cible;
    float distance;
    float angle_horizontal; // rotation autour de l'axe Y
    float angle_vertical;   // inclinaison haut/bas
} CameraOrbitale;

/// @brief Convertit une distance simulée en distance d'affichage 3D.
/// @param d Distance au carré issue de la simulation.
/// @return Distance adaptée à l'échelle visuelle.
float convertir_distance_en_affichage(double d);

/// @brief Convertit un vecteur 2D de simulation en un vecteur 3D pour Raylib.
/// @param pos Position 2D (x, y) de la simulation.
/// @return Position Vector3 dans l'espace 3D.
Vector3 convertir_position_simulation_en_3d(Vecteurs pos);

/// @brief Initialise et déclenche l'explosion d'une planète en générant des débris.
/// @param g Pointeur vers le groupe de débris à initialiser.
/// @param centre Position 3D où l'explosion a lieu.
/// @param couleur Couleur principale des débris.
/// @param rayon_planete Rayon de la planète originelle pour calculer la taille des fragments.
void exploser_planete(GroupeDebris *g, Vector3 centre, Color couleur, float rayon_planete);

/// @brief Met à jour la physique (position, vitesse, durée de vie) d'un groupe de débris.
/// @param g Pointeur vers le groupe de débris.
/// @param dt Delta time depuis la dernière frame.
void mettre_a_jour_debris(GroupeDebris *g, float dt);

/// @brief Dessine les débris à l'écran, avec optionnellement une traînée.
/// @param g Pointeur vers le groupe de débris.
/// @param avec_trainee Booléen (1 pour afficher la traînée de comète, 0 sinon).
void dessiner_debris(GroupeDebris *g, int avec_trainee);

/// @brief Dessine le modèle du soleil avec son halo et ses protubérances.
/// @param modele_soleil Modèle 3D du soleil.
/// @param rayon Rayon actuel du soleil.
/// @param texture_chargee Booléen (1 si la texture est chargée).
/// @param teinte Teinte actuelle à appliquer sur le modèle.
void dessiner_soleil_texture(Model modele_soleil, float rayon, int texture_chargee, Color teinte);

/// @brief Dessine une sphère simple en utilisant directement rlgl.
/// @param couleur Couleur de la sphère.
void dessiner_sphere_simple(Color couleur);

/// @brief Met à jour la position de la caméra en fonction des inputs utilisateur.
/// @param camera Pointeur vers la caméra Raylib.
/// @param orbite Pointeur vers les données de la caméra orbitale.
void mettre_a_jour_camera(Camera *camera, CameraOrbitale *orbite);

#endif