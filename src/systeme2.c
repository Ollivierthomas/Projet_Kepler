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

const char *fichiers_textures_planetes[] = {
    "assets/texture/mercure.png",
    "assets/texture/venus.png",
    "assets/texture/terre.png",
    "assets/texture/mars.png",
    "assets/texture/jupiter.png",
    "assets/texture/saturn.png",
    "assets/texture/uranus.png",
    "assets/texture/neptune.png"
};

void dessiner_soleil_texture(Model modele_soleil, float rayon, int texture_chargee);
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

    const float rayon_affichage_soleil = 10.5f;

    Model modele_soleil = LoadModelFromMesh(GenMeshSphere(1.0f, 32, 32));
    Texture2D texture_soleil = LoadTexture("assets/texture/soleil.png");
    int texture_soleil_chargee = texture_soleil.id > 0;

    if (texture_soleil_chargee) {
        SetMaterialTexture(&modele_soleil.materials[0], MATERIAL_MAP_DIFFUSE, texture_soleil);
    } else {
        printf("Texture non chargee pour le Soleil : texture/soleil.png\n");
    }

    Model modeles_planetes[NOMBRE_PLANETES];
    Texture2D textures_planetes[NOMBRE_PLANETES];
    int textures_planetes_chargees[NOMBRE_PLANETES] = {0};

    for (int i = 0; i < NOMBRE_PLANETES; i++) {
        modeles_planetes[i] = LoadModelFromMesh(GenMeshSphere(1.0f, 32, 32));
        textures_planetes[i] = LoadTexture(fichiers_textures_planetes[i]);
        textures_planetes_chargees[i] = textures_planetes[i].id > 0;

        if (textures_planetes_chargees[i]) {
            SetMaterialTexture(
                &modeles_planetes[i].materials[0],
                MATERIAL_MAP_DIFFUSE,
                textures_planetes[i]
            );
        } else {
            printf(
                "Texture non chargee pour %s : %s\n",
                donnees_planetes[i].nom,
                fichiers_textures_planetes[i]
            );
        }
    }

    Model modele_lune = LoadModelFromMesh(GenMeshSphere(1.0f, 32, 32));
    Texture2D texture_lune = LoadTexture("assets/texture/lune.png");
    int texture_lune_chargee = texture_lune.id > 0;

    if (texture_lune_chargee) {
        SetMaterialTexture(&modele_lune.materials[0], MATERIAL_MAP_DIFFUSE, texture_lune);
    } else {
        printf("Texture non chargee pour la Lune : assets/texture/lune.png\n");
    }

    // Skybox avec texture etoile.png
    Texture2D textureSkybox = LoadTexture("assets/texture/etoile.png");
    Model skybox = LoadModelFromMesh(GenMeshSphere(800.0f, 64, 64));
    skybox.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = textureSkybox;

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

                // --- Dessin du système solaire ---
                //skybox
                rlDisableBackfaceCulling();
                rlDisableDepthMask();
                rlDisableDepthTest();
                DrawModel(skybox, (Vector3){ 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);
                rlEnableDepthTest();
                rlEnableDepthMask();
                rlEnableBackfaceCulling();

                dessiner_soleil_texture(
                    modele_soleil,
                    rayon_affichage_soleil,
                    texture_soleil_chargee
                );

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

                    float rotation_planete = (float)GetTime() * 20.0f;
                    float rayon_planete = rayons_affichage_planetes[i];

                    DrawModelEx(
                        modeles_planetes[i],
                        position_planete,
                        (Vector3){0.0f, 1.0f, 0.0f},
                        rotation_planete,
                        (Vector3){rayon_planete, rayon_planete, rayon_planete},
                        textures_planetes_chargees[i] ? WHITE : couleurs_planetes[i]
                    );
                }

                size_t nombre_points_terre = trajectoires_planetes.values[INDEX_TERRE].size;
                size_t nombre_points_lune = trajectoire_lune_relative.size;

                size_t index_terre = index_animation % nombre_points_terre;
                size_t index_lune = index_animation % nombre_points_lune;

                Vector3 position_terre = trajectoires_affichage[INDEX_TERRE][index_terre];
                Vector3 position_lune_relative = trajectoire_lune_affichage[index_lune];

                Vector3 position_lune = {position_terre.x + position_lune_relative.x, 0.0f, position_terre.z + position_lune_relative.z};

                float rotation_lune = (float)GetTime() * 20.0f;

                DrawModelEx(
                    modele_lune,
                    position_lune,
                    (Vector3){0.0f, 1.0f, 0.0f},
                    rotation_lune,
                    (Vector3){1.05f, 1.05f, 1.05f},
                    texture_lune_chargee ? WHITE : LIGHTGRAY
                );

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

        if (textures_planetes[i].id > 0) {
            UnloadTexture(textures_planetes[i]);
        }

        UnloadModel(modeles_planetes[i]);
    }

    if (texture_lune.id > 0) {
        UnloadTexture(texture_lune);
    }
    UnloadModel(modele_lune);

    if (texture_soleil.id > 0) {
        UnloadTexture(texture_soleil);
    }
    UnloadModel(modele_soleil);

    UnloadTexture(textureSkybox);
    UnloadModel(skybox);

    free(trajectoire_lune_affichage);

    array_free(trajectoires_planetes);
    array_free(trajectoire_lune_relative);

    CloseWindow();

    return 0;
}

void dessiner_soleil_texture(Model modele_soleil, float rayon, int texture_chargee) {
    float temps = (float)GetTime();
    float pulsation = 1.0f + 0.05f * sinf(temps * 2.5f);

    // --- Noyau texturé en rotation ---
    DrawModelEx(
        modele_soleil,
        (Vector3){0.0f, 0.0f, 0.0f},
        (Vector3){0.0f, 1.0f, 0.0f},
        temps * 8.0f,
        (Vector3){rayon, rayon, rayon},
        texture_chargee ? WHITE : YELLOW
    );

    BeginBlendMode(BLEND_ADDITIVE);

        // --- Couronne interne chaude ---
        DrawSphereEx((Vector3){0}, rayon * 1.25f * pulsation, 32, 32, Fade((Color){255, 180, 30, 255}, 0.35f));

        // --- Couronne externe diffuse ---
        DrawSphereEx((Vector3){0}, rayon * 1.80f * pulsation, 32, 32, Fade((Color){255, 120, 0, 255}, 0.12f));

        

        // --- Protubérances solaires : arcs 3D dans des plans variés ---
        int nombre_protuberances = 12;
        for (int i = 0; i < nombre_protuberances; i++) {
            float angle_base   = 2.0f * PI * i / nombre_protuberances;
            float phase        = temps * 1.2f + i * 0.8f;
            float hauteur      = rayon * (0.25f + 0.20f * sinf(phase));
            float largeur_arc  = rayon * (0.30f + 0.15f * cosf(phase * 0.7f));

            // Chaque protubérance est dans un plan incliné différent
            float inclinaison = (float)i / nombre_protuberances * PI;

            Vector3 base_gauche = {
                cosf(angle_base) * rayon * 1.05f,
                sinf(inclinaison) * rayon * 0.05f,
                sinf(angle_base) * rayon * 1.05f
            };

            Vector3 sommet = {
                cosf(angle_base) * (rayon + largeur_arc),
                hauteur * cosf(inclinaison),
                sinf(angle_base) * (rayon + largeur_arc)
            };

            Vector3 base_droite = {
                cosf(angle_base + 0.35f) * rayon * 1.05f,
                sinf(inclinaison) * rayon * 0.05f,
                sinf(angle_base + 0.35f) * rayon * 1.05f
            };

            // Arc en plusieurs segments pour la courbe
            int segments = 8;
            for (int s = 0; s < segments; s++) {
                float t0 = (float)s / segments;
                float t1 = (float)(s + 1) / segments;

                // Interpolation quadratique de Bézier
                float u0 = (1-t0)*(1-t0), v0 = 2*(1-t0)*t0, w0 = t0*t0;
                float u1 = (1-t1)*(1-t1), v1 = 2*(1-t1)*t1, w1 = t1*t1;

                Vector3 p0 = {
                    u0*base_gauche.x + v0*sommet.x + w0*base_droite.x,
                    u0*base_gauche.y + v0*sommet.y + w0*base_droite.y,
                    u0*base_gauche.z + v0*sommet.z + w0*base_droite.z
                };
                Vector3 p1 = {
                    u1*base_gauche.x + v1*sommet.x + w1*base_droite.x,
                    u1*base_gauche.y + v1*sommet.y + w1*base_droite.y,
                    u1*base_gauche.z + v1*sommet.z + w1*base_droite.z
                };

                float alpha = 0.9f * (1.0f - fabsf(t0 + t1 - 1.0f)); // fade aux extrémités
                DrawLine3D(p0, p1, Fade((Color){255, 160, 40, 255}, alpha));
            }
        }


    EndBlendMode();
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