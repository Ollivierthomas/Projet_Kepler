#include "raylib.h"
#include "rlgl.h"
#include <math.h>           // Required for: cosf(), sinf()
#include <stdio.h>
#include <stdlib.h>

#define SCALE 1e-10f

struct Etat {
    double t;
    Vector3 position;
    double ke;
    double pe;
};

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
    const int screenWidth = 800;
    const int screenHeight = 450;

    const float sunRadius = 4.0f;
    const float earthRadius = 0.6f;
    const float moonRadius = 0.16f;
    const float moonOrbitRadius = 1.5f;

    struct Etat etats[52590];
    int nb_etats = charger_json("../website/data_terre.json", etats, 52590);

    if (nb_etats == 0) {
        perror("Aucun etats");
        exit(EXIT_FAILURE);
    }

    InitWindow(screenWidth, screenHeight, "raylib [models] example - rlgl solar system");

    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ 16.0f, 16.0f, 16.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    float rotationSpeed = 0.2f;         // General system rotation speed
    int index_eart = 0;
    int vitesse_animation = 16;
    float earthRotation = 0.0f;         // Rotation of earth around itself (days) in degrees
    float moonRotation = 0.0f;          // Rotation of moon around itself
    float moonOrbitRotation = 0.0f;     // Rotation of moon around earth in degrees

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        earthRotation += (5.0f*rotationSpeed);
        index_eart += vitesse_animation;
        moonRotation += (2.0f*rotationSpeed);
        moonOrbitRotation += (8.0f*rotationSpeed);

        if (index_eart >= nb_etats) {
            index_eart = 0;
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
                    Vector3 terre = etats[index_eart].position;

                    rlPushMatrix();
                        rlTranslatef(terre.x, terre.y, terre.z);
                        rlRotatef(earthRotation, 0.25f, 1.0f, 0.0f);
                        rlScalef(earthRadius, earthRadius, earthRadius);

                        DrawSphereBasic(BLUE);
                    rlPopMatrix();   

                    // rlRotatef(moonOrbitRotation, 0.0f, 1.0f, 0.0f);     // Rotation for Moon orbit around Earth
                    // rlTranslatef(moonOrbitRadius, 0.0f, 0.0f);          // Translation for Moon orbit
                    // rlRotatef(moonRotation, 0.0f, 1.0f, 0.0f);          // Rotation for Moon itself
                    // rlScalef(moonRadius, moonRadius, moonRadius);       // Scale Moon

                    // DrawSphereBasic(LIGHTGRAY);                         // Draw the Moon
                rlPopMatrix();

                // Some reference elements (not affected by previous matrix transformations)
                for (int i = 1; i < nb_etats; i++) {
                    DrawLine3D(etats[i - 1].position, etats[i].position, BLUE);
                }
                DrawGrid(20, 1.0f);

            EndMode3D();

            DrawText("EARTH ORBITING AROUND THE SUN!", 400, 10, 20, MAROON);
            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
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