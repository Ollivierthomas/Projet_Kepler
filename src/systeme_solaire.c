#include "raylib.h"
#include "rlgl.h"
#include <math.h>           
#include <stdio.h>
#include <stdlib.h>

#define SCALE 1e-10f

struct Etat {
    double t;
    Vector3 position;
    double ke;
    double pe;
};


typedef struct Planete {
    char* nom;
    char* fichier;
    struct Etat* etats;
    int nb_etats;
    int index;
    int max_points;
    int parent;
    float distance_scale;

    float rayon;
    Color couleur;
} Planete;

Planete planetes[] = {
    {"Mercure", "./planetes/data_mercure.json", NULL, 0, 0, 0, -1, 1.0f, 0.20f, GRAY},
    {"Venus", "./planetes/data_venus.json", NULL, 0, 0, 0, -1, 1.0f, 0.45f, ORANGE},
    {"Terre", "./planetes/data_terre.json", NULL, 0, 0, 0, -1, 1.0f, 0.50f, BLUE},
    {"Lune", "./planetes/data_lune.json", NULL, 0, 0, 0, 2, 40.0f, 0.16f, LIGHTGRAY},
    {"Mars", "./planetes/data_mars.json", NULL, 0, 0, 0, -1, 1.0f, 0.30f, RED},
    {"Jupiter", "./planetes/data_jupiter.json", NULL, 0, 0, 0, -1, 1.0f, 1.20f, BROWN},
    {"Saturne", "./planetes/data_saturne.json", NULL, 0, 0, 0, -1, 1.0f, 1.00f, GOLD},
    {"Uranus", "./planetes/data_uranus.json", NULL, 0, 0, 0, -1, 1.0f, 0.75f, SKYBLUE},
    {"Neptune", "./planetes/data_neptune.json", NULL, 0, 0, 0, -1, 1.0f, 0.75f, DARKBLUE}
};

int nb_planetes = sizeof(planetes) / sizeof(planetes[0]);

int charger_json(char* filename, struct Etat etats[], int max_points) {
    FILE *f = fopen(filename, "r");

    if (f == NULL) {
        perror("Erreur ouverture fichier JSON\n");
        exit(EXIT_FAILURE);
    }

    char ligne[512];
    int count = 0;

    while (fgets(ligne, sizeof(ligne), f) != NULL && count < max_points) {
        double t, x, y, z, ke, pe;

        int result = sscanf(ligne, " { \"t\": %lf, \"x\": %lf, \"y\": %lf, \"z\": %lf, \"ke\": %lf, \"pe\": %lf", &t, &x, &y, &z, &ke, &pe);

        if (result == 6) {
            etats[count].t = t;
            
            etats[count].position = (Vector3){
                (float)(x * SCALE),
                (float)(z * SCALE),
                (float)(y * SCALE)
            };

            etats[count].ke = ke;
            etats[count].pe = pe;

            count++;
        }
    }

    fclose(f);
    return count;
}

int compter_points_json(char* filename) {
    FILE *f = fopen(filename, "r");

    if (f == NULL) {
        perror("Erreur ouverture fichier JSON\n");
        exit(EXIT_FAILURE);
    }

    char ligne[512];
    int count = 0;

    while (fgets(ligne, sizeof(ligne), f) != NULL) {
        double t, x, y, z, ke, pe;

        int result = sscanf(
            ligne,
            " { \"t\": %lf, \"x\": %lf, \"y\": %lf, \"z\": %lf, \"ke\": %lf, \"pe\": %lf",
            &t, &x, &y, &z, &ke, &pe
        );

        if (result == 6) {
            count++;
        }
    }

    fclose(f);
    return count;
}

//------------------------------------------------------------------------------------
// Module Functions Declaration
//------------------------------------------------------------------------------------
void DrawSphereBasic(Color color);      // Draw sphere without any matrix transformation

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    for (int i = 0; i < nb_planetes; i++) {

        planetes[i].max_points = compter_points_json(planetes[i].fichier);

        planetes[i].etats = malloc(planetes[i].max_points * sizeof(struct Etat));

        if (planetes[i].etats == NULL) {
            perror("Erreur malloc");
            exit(EXIT_FAILURE);
        }

        planetes[i].nb_etats = charger_json(planetes[i].fichier, planetes[i].etats, planetes[i].max_points);

        if (planetes[i].nb_etats == 0) {
            printf("Aucune donnee pour %s\n", planetes[i].nom);
            exit(EXIT_FAILURE);
        }
    }

    const int screenWidth = 900;
    const int screenHeight = 550;

    const float sunRadius = 4.0f;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - rlgl solar system");

    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ 25.0f, 25.0f, 25.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    float rotationSpeed = 0.2f;         // General system rotation speed
    int vitesse_animation = 16;

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        for (int i = 0; i < nb_planetes; i++) {
            planetes[i].index = (planetes[i].index + vitesse_animation) % planetes[i].nb_etats;
        }

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                rlPushMatrix();
                    rlScalef(sunRadius, sunRadius, sunRadius);          // Scale Sun
                    DrawSphereBasic(GOLD);                              // Draw the Sun
                rlPopMatrix();

                rlPushMatrix();
                    for (int i = 0; i < nb_planetes; i++) {
                        Vector3 pos = planetes[i].etats[planetes[i].index].position;

                        pos.x *= planetes[i].distance_scale;
                        pos.y *= planetes[i].distance_scale;
                        pos.z *= planetes[i].distance_scale;

                        if (planetes[i].parent != -1) {
                            Planete parent = planetes[planetes[i].parent];
                            Vector3 parent_pos = parent.etats[parent.index].position;

                            pos.x += parent_pos.x;
                            pos.y += parent_pos.y;
                            pos.z += parent_pos.z;
                        }

                        rlPushMatrix();
                            rlTranslatef(pos.x, pos.y, pos.z);
                            rlScalef(planetes[i].rayon, planetes[i].rayon, planetes[i].rayon);
                            DrawSphereBasic(planetes[i].couleur);
                        rlPopMatrix();
                    }
                rlPopMatrix();

                // Some reference elements (not affected by previous matrix transformations)
                for (int i = 0; i < nb_planetes; i++) {
                    if (planetes[i].parent != -1) continue;

                    // for (int j = 1; j < planetes[i].nb_etats; j++) {
                    //     DrawLine3D(planetes[i].etats[j - 1].position,
                    //                planetes[i].etats[j].position,
                    //                planetes[i].couleur);
                    // }
                }
                DrawGrid(40, 2.0f);

            EndMode3D();

            DrawText("EARTH ORBITING AROUND THE SUN!", 400, 10, 20, MAROON);
            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    for (int i = 0; i < nb_planetes; i++) {
        free(planetes[i].etats);
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//--------------------------------------------------------------------------------------------
// Module Functions Definition
//--------------------------------------------------------------------------------------------
// Draw sphere without any matrix transformation
// NOTE: Sphere is drawn in world position ( 0, 0, 0 ) with radius 1.0f
void DrawSphereBasic(Color color)
{
    int rings = 16;
    int slices = 16;

    // Make sure there is enough space in the internal render batch
    // buffer to store all required vertex, batch is reseted if required
    rlCheckRenderBatchLimit((rings + 2)*slices*6);

    rlBegin(RL_TRIANGLES);
        rlColor4ub(color.r, color.g, color.b, color.a);

        for (int i = 0; i < (rings + 2); i++)
        {
            for (int j = 0; j < slices; j++)
            {
                rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(rings + 1))*i))*sinf(DEG2RAD*(j*360.0f/slices)),
                           sinf(DEG2RAD*(270+(180.0f/(rings + 1))*i)),
                           cosf(DEG2RAD*(270+(180.0f/(rings + 1))*i))*cosf(DEG2RAD*(j*360.0f/slices)));
                rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(rings + 1))*(i+1)))*sinf(DEG2RAD*((j+1)*360.0f/slices)),
                           sinf(DEG2RAD*(270+(180.0f/(rings + 1))*(i+1))),
                           cosf(DEG2RAD*(270+(180.0f/(rings + 1))*(i+1)))*cosf(DEG2RAD*((j+1)*360.0f/slices)));
                rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(rings + 1))*(i+1)))*sinf(DEG2RAD*(j*360.0f/slices)),
                           sinf(DEG2RAD*(270+(180.0f/(rings + 1))*(i+1))),
                           cosf(DEG2RAD*(270+(180.0f/(rings + 1))*(i+1)))*cosf(DEG2RAD*(j*360.0f/slices)));

                rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(rings + 1))*i))*sinf(DEG2RAD*(j*360.0f/slices)),
                           sinf(DEG2RAD*(270+(180.0f/(rings + 1))*i)),
                           cosf(DEG2RAD*(270+(180.0f/(rings + 1))*i))*cosf(DEG2RAD*(j*360.0f/slices)));
                rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(rings + 1))*(i)))*sinf(DEG2RAD*((j+1)*360.0f/slices)),
                           sinf(DEG2RAD*(270+(180.0f/(rings + 1))*(i))),
                           cosf(DEG2RAD*(270+(180.0f/(rings + 1))*(i)))*cosf(DEG2RAD*((j+1)*360.0f/slices)));
                rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(rings + 1))*(i+1)))*sinf(DEG2RAD*((j+1)*360.0f/slices)),
                           sinf(DEG2RAD*(270+(180.0f/(rings + 1))*(i+1))),
                           cosf(DEG2RAD*(270+(180.0f/(rings + 1))*(i+1)))*cosf(DEG2RAD*((j+1)*360.0f/slices)));
            }
        }
    rlEnd();
}
