#include "raylib.h"
#include "rlgl.h"
#include <math.h>           
#include <stdio.h>
#include <stdlib.h>
#include "tab_generic.h"
#include "trajectoire.h"
#include "vectors.h"

#define SCALE 1e-10f

typedef Array(Vecteurs) Array_Vecteurs;

struct Planete planete[] = {
    { "mercure", 3.3011e23, 46.00e9,   69.82e9 },
    { "venus",   4.8675e24, 107.48e9, 108.94e9 },
    { "terre",   5.9720e24, 147.09e9, 152.10e9 },
    { "mars",    6.4171e23, 206.62e9, 249.23e9 },
    { "jupiter", 1.8982e27, 740.52e9, 816.62e9 },
    { "saturne", 5.6834e26, 1352.55e9, 1514.50e9 },
    { "uranus",  8.6810e25, 2741.30e9, 3003.62e9 },
    { "neptune", 1.02413e26, 4444.45e9, 4545.67e9 }
};

int nb_planetes = sizeof(planete) / sizeof(planete[0]);

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

float rayons_planetes[] = {
    0.20f,
    0.45f,
    0.50f,
    0.30f,
    1.20f,
    1.00f,
    0.75f,
    0.75f
};

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
    double G = 6.67430e-11;
    double m_soleil = 1.989e30;

    Array(Array_Vecteurs) planetes = {0};

    for (int i = 0; i < nb_planetes; i++) {
        array_push(planetes, simuler_planete(planete[i], G, m_soleil));
    }

    const int screenWidth = 900;
    const int screenHeight = 550;

    const float sunRadius = 4.0f;

    size_t index = 0;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "raylib [models] example - rlgl solar system");

    int monitor = GetCurrentMonitor();
    int monitorWidth = GetMonitorWidth(monitor);
    int monitorHeight = GetMonitorHeight(monitor);

    SetWindowPosition((monitorWidth - screenWidth) / 2, (monitorHeight - screenHeight) / 2);
    RestoreWindow();

    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ 25.0f, 25.0f, 25.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    int vitesse_animation = 16;

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        index += (size_t)vitesse_animation;

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
                        Array_Vecteurs trajectoire = planetes.values[i];
                        Vecteurs position = trajectoire.values[index % trajectoire.size];

                        Vector3 pos = {
                            (float)(position.x * SCALE),
                            (float)(position.z * SCALE),
                            (float)(position.y * SCALE)
                        };

                        rlPushMatrix();
                            rlTranslatef(pos.x, pos.y, pos.z);
                            rlScalef(rayons_planetes[i], rayons_planetes[i], rayons_planetes[i]);
                            DrawSphereBasic(couleurs_planetes[i]);
                        rlPopMatrix();
                    }
                rlPopMatrix();

                // Some reference elements (not affected by previous matrix transformations)
                // for (int i = 0; i < nb_planetes; i++) {
                //     Array_Vecteurs trajectoire = planetes.values[i];

                //     for (size_t j = 1; j < trajectoire.size; j++) {
                //         Vecteurs previous = trajectoire.values[j - 1];
                //         Vecteurs current = trajectoire.values[j];

                //         Vector3 previous_pos = {
                //             (float)(previous.x * SCALE),
                //             (float)(previous.z * SCALE),
                //             (float)(previous.y * SCALE)
                //         };
                //         Vector3 current_pos = {
                //             (float)(current.x * SCALE),
                //             (float)(current.z * SCALE),
                //             (float)(current.y * SCALE)
                //         };

                //         DrawLine3D(previous_pos, current_pos, couleurs_planetes[i]);
                //     }
                // }
                DrawGrid(40, 2.0f);

            EndMode3D();

            DrawText("EARTH ORBITING AROUND THE SUN!", 400, 10, 20, MAROON);
            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    for (int i = 0; i < nb_planetes; i++) {
        array_free(planetes.values[i]);
    }
    array_free(planetes);

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
