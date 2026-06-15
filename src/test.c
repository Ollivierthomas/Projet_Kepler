#include "raylib.h"
#include "rlgl.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "tab_generic.h"
#include "trajectoire.h"
#include "vectors.h"

#define ECHELLE_DISTANCE_AFFICHAGE 3.5e-4
#define ECHELLE_DISTANCE_LUNE 1.5f
#define INDEX_TERRE 2
#define NOMBRE_PLANETES 8

typedef Array(Vecteurs) Trajectoire;


/// Convertit une distance réelle en mètres vers une distance visible dans raylib.
/// @param distance_metres Distance réelle en mètres
/// @return Distance convertie pour l'affichage dans raylib
float convertir_distance_en_affichage(double distance_metres) {
    if (distance_metres <= 0.0) {
        return 0.0f;
    }

    return (float)(sqrt(distance_metres) * ECHELLE_DISTANCE_AFFICHAGE);
}

/// Convertit une position de simulation 2D en position 3D pour raylib.
/// @param position_simulation Position de simulation 2D
/// @return Position 3D pour raylib
Vector3 convertir_position_simulation_en_3d(Vecteurs position_simulation) {
    double distance = sqrt(
        position_simulation.x * position_simulation.x +
        position_simulation.y * position_simulation.y
    );

    float distance_affichage = convertir_distance_en_affichage(distance);

    float direction_x;
    float direction_y;

    if (distance > 0.0) {
        direction_x = (float)(position_simulation.x / distance);
        direction_y = (float)(position_simulation.y / distance);
    } else {
        direction_x = 1.0f;
        direction_y = 0.0f;
    }

    return (Vector3){direction_x * distance_affichage, 0.0f, direction_y * distance_affichage};
}

struct Planete donnees_planetes[] = {
    { "mercure", 3.3011e23,  46.00e9,    69.82e9   },
    { "venus",   4.8675e24,  107.48e9,   108.94e9  },
    { "terre",   5.9720e24,  147.09e9,   152.10e9  },
    { "mars",    6.4171e23,  206.62e9,   249.23e9  },
    { "jupiter", 1.8982e27,  740.52e9,   816.62e9  },
    { "saturne", 5.6834e26,  1352.55e9,  1514.50e9 },
    { "uranus",  8.6810e25,  2741.30e9,  3003.62e9 },
    { "neptune", 1.02413e26, 4444.45e9,  4545.67e9 }
};

Color couleurs_planetes[] = {
    GRAY,
    ORANGE,
    BLUE,
    RED,
    BROWN,
    GOLD,
    SKYBLUE,
    DARKBLUE
};

float rayons_affichage_planetes[] = {
    3.60f, // Mercure
    3.36f, // Venus
    3.84f, // Terre
    3.00f, // Mars
    9.00f, // Jupiter
    7.80f, // Saturne
    6.60f, // Uranus
    6.60f  // Neptune
};

void dessiner_sphere_simple(Color couleur);

int main(void) {
    double constante_gravitationnelle = 6.67430e-11;
    double masse_soleil = 1.989e30;
    double masse_terre = 5.9720e24;

    // Music music;

    Array(Trajectoire) trajectoires_planetes = {0};

    for (int i = 0; i < NOMBRE_PLANETES; i++) {
        Trajectoire trajectoire = simuler_planete(
            donnees_planetes[i],
            constante_gravitationnelle,
            masse_soleil
        );

        array_push(trajectoires_planetes, trajectoire);
    }

    struct Planete donnees_lune = {"lune", 7.35e22, 356700e3, 406300e3};

    Trajectoire trajectoire_lune_relative = simuler_planete(donnees_lune, constante_gravitationnelle, masse_terre);

    Vector3 *trajectoires_affichage[NOMBRE_PLANETES];

    for (int i = 0; i < NOMBRE_PLANETES; i++) {
        Trajectoire trajectoire_actuelle = trajectoires_planetes.values[i];

        trajectoires_affichage[i] = malloc(trajectoire_actuelle.size * sizeof(Vector3));

        for (size_t j = 0; j < trajectoire_actuelle.size; j++) {
            trajectoires_affichage[i][j] = convertir_position_simulation_en_3d(trajectoire_actuelle.values[j]);
        }
    }

    Vector3 *trajectoire_lune_affichage = malloc(
        trajectoire_lune_relative.size * sizeof(Vector3)
    );

    for (size_t i = 0; i < trajectoire_lune_relative.size; i++) {
        Vecteurs position_lune_simulation = trajectoire_lune_relative.values[i];

        double distance_lune = sqrt(
            position_lune_simulation.x * position_lune_simulation.x +
            position_lune_simulation.y * position_lune_simulation.y
        );

        float distance_lune_affichage =
            (float)(sqrt(distance_lune) * ECHELLE_DISTANCE_AFFICHAGE * ECHELLE_DISTANCE_LUNE);

        float direction_lune_x;
        float direction_lune_y;

        if (distance_lune > 0.0) {
            direction_lune_x = (float)(position_lune_simulation.x / distance_lune);
            direction_lune_y = (float)(position_lune_simulation.y / distance_lune);
        } else {
            direction_lune_x = 1.0f;
            direction_lune_y = 0.0f;
        }

        trajectoire_lune_affichage[i] = (Vector3){direction_lune_x * distance_lune_affichage, 0.0f, direction_lune_y * distance_lune_affichage};
    }

    const int largeur_fenetre = 1200;
    const int hauteur_fenetre = 700;

    size_t index_animation = 0;
    int vitesse_animation = 3;

    float sunRadius = 10.5f;
    float sunDirection = -0.1f;  // 0.1 = grandit, -0.1 = rétréci

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(largeur_fenetre, hauteur_fenetre, "Système Solaire");
    InitAudioDevice();

    // music = LoadMusicStream("assets/audio/music.mp3");
    // if (music.frameCount == 0) {
    //     printf("Erreur : fichier audio non trouvé ou invalide\n");
    //     printf("Chemin cherche : ../assets/audio/music.mp3\n");
    // } else {
    //     printf("Musique chargee correctement\n");
    //     PlayMusicStream(music);
    // }

    Camera camera = {0};
    camera.position = (Vector3){0.0f, 550.0f, 550.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        index_animation += (size_t)vitesse_animation;

        float vitesse_deplacement_camera = 1.0f;
        float vitesse_zoom_camera = 2.0f;

        if (IsKeyDown(KEY_LEFT)) {
            camera.position.x -= vitesse_deplacement_camera;
            camera.target.x -= vitesse_deplacement_camera;
        }

        if (IsKeyDown(KEY_RIGHT)) {
            camera.position.x += vitesse_deplacement_camera;
            camera.target.x += vitesse_deplacement_camera;
        }

        if (IsKeyDown(KEY_UP)) {
            camera.position.y -= vitesse_zoom_camera;
            camera.position.z -= vitesse_zoom_camera;
        }

        if (IsKeyDown(KEY_DOWN)) {
            camera.position.y += vitesse_zoom_camera;
            camera.position.z += vitesse_zoom_camera;
        }

        BeginDrawing();
            ClearBackground(BLACK);

            // UpdateMusicStream(music);

            BeginMode3D(camera);

                // Animation : rétréci puis grossit en blanc
                sunRadius -= 0.01f;  // Rétréci toujours

                // Quand atteint le minimum, recommence à grossir
                if (sunRadius < 10.5f) {
                    sunRadius = 10.5f;
                    sunDirection = 0.2f;  // Change en mode grossissement
                }

                // Grossit jusqu'à énorme
                if (sunDirection > 0.0f) {
                    sunRadius += sunDirection;
                    if (sunRadius > 200.0f) {
                        sunRadius = 35.5f;
                        sunDirection = -0.1f;  // Retour au rétréci
                    }
                }

                // Choisir couleur
                Color sunColor = (sunDirection > 0.0f) ? WHITE : BLUE;

                rlPushMatrix();
                    rlScalef(sunRadius, sunRadius, sunRadius);
                    dessiner_sphere_simple(sunColor);
                rlPopMatrix();

                for (int i = 0; i < NOMBRE_PLANETES; i++) {
                    size_t nombre_points = trajectoires_planetes.values[i].size;

                    int pas_affichage = (int)(nombre_points / 300);
                    if (pas_affichage < 1) {
                        pas_affichage = 1;
                    }

                    size_t j = 0;

                    while (j + pas_affichage < nombre_points) {
                        DrawLine3D(
                            trajectoires_affichage[i][j],
                            trajectoires_affichage[i][j + pas_affichage],
                            couleurs_planetes[i]
                        );
                        j += pas_affichage;
                    }

                    DrawLine3D(
                        trajectoires_affichage[i][j],
                        trajectoires_affichage[i][0],
                        couleurs_planetes[i]
                    );
                }

                for (int i = 0; i < NOMBRE_PLANETES; i++) {
                    size_t nombre_points = trajectoires_planetes.values[i].size;
                    size_t index_planete = index_animation % nombre_points;

                    Vector3 position_planete = trajectoires_affichage[i][index_planete];

                    rlPushMatrix();
                        rlTranslatef(
                            position_planete.x,
                            position_planete.y,
                            position_planete.z
                        );

                        rlScalef(
                            rayons_affichage_planetes[i],
                            rayons_affichage_planetes[i],
                            rayons_affichage_planetes[i]
                        );

                        dessiner_sphere_simple(couleurs_planetes[i]);
                    rlPopMatrix();
                }

                size_t nombre_points_terre = trajectoires_planetes.values[INDEX_TERRE].size;
                size_t nombre_points_lune = trajectoire_lune_relative.size;

                size_t index_terre = index_animation % nombre_points_terre;
                size_t index_lune = index_animation % nombre_points_lune;

                Vector3 position_terre = trajectoires_affichage[INDEX_TERRE][index_terre];
                Vector3 position_lune_relative = trajectoire_lune_affichage[index_lune];

                Vector3 position_lune = {position_terre.x + position_lune_relative.x, 0.0f, position_terre.z + position_lune_relative.z};

                rlPushMatrix();
                    rlTranslatef(
                        position_lune.x,
                        position_lune.y,
                        position_lune.z
                    );

                    rlScalef(1.05f, 1.05f, 1.05f);

                    dessiner_sphere_simple(LIGHTGRAY);
                rlPopMatrix();

            EndMode3D();

            DrawText("SYSTEME SOLAIRE", 10, 10, 20, RAYWHITE);
            DrawText("Fleches : deplacer  |  Haut/Bas : zoom", 10, 35, 14, GRAY);
            DrawFPS(10, hauteur_fenetre - 25);

        EndDrawing();
    }

    // StopMusicStream(music);



    for (int i = 0; i < NOMBRE_PLANETES; i++) {
        array_free(trajectoires_planetes.values[i]);
        free(trajectoires_affichage[i]);
    }

    free(trajectoire_lune_affichage);

    array_free(trajectoires_planetes);
    array_free(trajectoire_lune_relative);

    CloseWindow();

    return 0;
}

void dessiner_sphere_simple(Color couleur) {
    int nombre_anneaux = 16;
    int nombre_tranches = 16;

    rlCheckRenderBatchLimit((nombre_anneaux + 2) * nombre_tranches * 6);

    rlBegin(RL_TRIANGLES);
        rlColor4ub(couleur.r, couleur.g, couleur.b, couleur.a);

        for (int i = 0; i < nombre_anneaux + 2; i++) {
            for (int j = 0; j < nombre_tranches; j++) {
                rlVertex3f(
                    cosf(DEG2RAD * (270 + (180.0f / (nombre_anneaux + 1)) * i)) *
                    sinf(DEG2RAD * (j * 360.0f / nombre_tranches)),

                    sinf(DEG2RAD * (270 + (180.0f / (nombre_anneaux + 1)) * i)),

                    cosf(DEG2RAD * (270 + (180.0f / (nombre_anneaux + 1)) * i)) *
                    cosf(DEG2RAD * (j * 360.0f / nombre_tranches))
                );

                rlVertex3f(
                    cosf(DEG2RAD * (270 + (180.0f / (nombre_anneaux + 1)) * (i + 1))) *
                    sinf(DEG2RAD * ((j + 1) * 360.0f / nombre_tranches)),

                    sinf(DEG2RAD * (270 + (180.0f / (nombre_anneaux + 1)) * (i + 1))),

                    cosf(DEG2RAD * (270 + (180.0f / (nombre_anneaux + 1)) * (i + 1))) *
                    cosf(DEG2RAD * ((j + 1) * 360.0f / nombre_tranches))
                );

                rlVertex3f(
                    cosf(DEG2RAD * (270 + (180.0f / (nombre_anneaux + 1)) * (i + 1))) *
                    sinf(DEG2RAD * (j * 360.0f / nombre_tranches)),

                    sinf(DEG2RAD * (270 + (180.0f / (nombre_anneaux + 1)) * (i + 1))),

                    cosf(DEG2RAD * (270 + (180.0f / (nombre_anneaux + 1)) * (i + 1))) *
                    cosf(DEG2RAD * (j * 360.0f / nombre_tranches))
                );

                rlVertex3f(
                    cosf(DEG2RAD * (270 + (180.0f / (nombre_anneaux + 1)) * i)) *
                    sinf(DEG2RAD * (j * 360.0f / nombre_tranches)),

                    sinf(DEG2RAD * (270 + (180.0f / (nombre_anneaux + 1)) * i)),

                    cosf(DEG2RAD * (270 + (180.0f / (nombre_anneaux + 1)) * i)) *
                    cosf(DEG2RAD * (j * 360.0f / nombre_tranches))
                );

                rlVertex3f(
                    cosf(DEG2RAD * (270 + (180.0f / (nombre_anneaux + 1)) * i)) *
                    sinf(DEG2RAD * ((j + 1) * 360.0f / nombre_tranches)),

                    sinf(DEG2RAD * (270 + (180.0f / (nombre_anneaux + 1)) * i)),

                    cosf(DEG2RAD * (270 + (180.0f / (nombre_anneaux + 1)) * i)) *
                    cosf(DEG2RAD * ((j + 1) * 360.0f / nombre_tranches))
                );

                rlVertex3f(
                    cosf(DEG2RAD * (270 + (180.0f / (nombre_anneaux + 1)) * (i + 1))) *
                    sinf(DEG2RAD * ((j + 1) * 360.0f / nombre_tranches)),

                    sinf(DEG2RAD * (270 + (180.0f / (nombre_anneaux + 1)) * (i + 1))),

                    cosf(DEG2RAD * (270 + (180.0f / (nombre_anneaux + 1)) * (i + 1))) *
                    cosf(DEG2RAD * ((j + 1) * 360.0f / nombre_tranches))
                );
            }
        }
    rlEnd();
}