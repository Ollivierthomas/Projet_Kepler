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

<<<<<<< HEAD
<<<<<<< HEAD:src/systeme.c
=======
>>>>>>> 59c9252 (textures des planetes et halo lumineux du soleil et fond etoilé)
Color couleurs_planetes[] = {
    GRAY,
    ORANGE,
    BLUE,
    RED,
    BROWN,
    GOLD,
    SKYBLUE,
    DARKBLUE
<<<<<<< HEAD
=======
typedef struct Planete {
    char* nom;
    char* fichier;
    char* texture_fichier;
    struct Etat* etats;
    int nb_etats;
    int index;
    int max_points;
    int parent;
    float distance_scale;

    float rayon;
    Color couleur;

    Texture2D texture;
    Model modele;
} Planete;

Planete planetes[] = {
    {"Mercure", "./planetes/data_mercure.json","texture/mercure.jpg", NULL, 0, 0, 0, -1, 1.0f, 0.20f, GRAY},
    {"Venus", "./planetes/data_venus.json", "texture/venus.jpg", NULL, 0, 0, 0, -1, 1.0f, 0.45f, ORANGE},
    {"Terre", "./planetes/data_terre.json", "texture/terre.jpg", NULL, 0, 0, 0, -1, 1.0f, 0.50f, BLUE},
    {"Lune", "./planetes/data_lune.json", "texture/lune.jpg", NULL, 0, 0, 0, 2, 40.0f, 0.16f, LIGHTGRAY},
    {"Mars", "./planetes/data_mars.json", "texture/mars.jpg", NULL, 0, 0, 0, -1, 1.0f, 0.30f, RED},
    {"Jupiter", "./planetes/data_jupiter.json", "texture/jupiter.jpg", NULL, 0, 0, 0, -1, 1.0f, 1.20f, BROWN},
    {"Saturne", "./planetes/data_saturne.json", "texture/saturne.jpg", NULL, 0, 0, 0, -1, 1.0f, 1.00f, GOLD},
    {"Uranus", "./planetes/data_uranus.json", "texture/uranus.jpg", NULL, 0, 0, 0, -1, 1.0f, 0.75f, SKYBLUE},
    {"Neptune", "./planetes/data_neptune.json", "texture/neptune.jpg", NULL, 0, 0, 0, -1, 1.0f, 0.75f, DARKBLUE}
>>>>>>> 97ef667 (ajout des textures):src/systeme_solaire.c
=======
>>>>>>> 59c9252 (textures des planetes et halo lumineux du soleil et fond etoilé)
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

<<<<<<< HEAD
=======



const char *textures_planetes[] = {
    "texture/mercure.png",
    "texture/venus.png",
    "texture/terre.png",
    "texture/mars.png",
    "texture/jupiter.png",
    "texture/saturn.png",
    "texture/uranus.png",
    "texture/neptune.png"
};


>>>>>>> 59c9252 (textures des planetes et halo lumineux du soleil et fond etoilé)
//------------------------------------------------------------------------------------
// Module Functions Declaration
//------------------------------------------------------------------------------------
void DrawSphereBasic(Color color);      // Draw sphere without any matrix transformation

<<<<<<< HEAD
void DrawSunShiny(Model sun_Model, float radius);
=======
>>>>>>> 59c9252 (textures des planetes et halo lumineux du soleil et fond etoilé)
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
<<<<<<< HEAD
    
    Model sunModel = LoadModelFromMesh(GenMeshSphere(1.0f, 32, 32));
    Texture2D sunTexture = LoadTexture("texture/soleil.jpg");

<<<<<<< HEAD:src/systeme.c
=======

>>>>>>> 59c9252 (textures des planetes et halo lumineux du soleil et fond etoilé)
    int monitor = GetCurrentMonitor();
    int monitorWidth = GetMonitorWidth(monitor);
    int monitorHeight = GetMonitorHeight(monitor);

    SetWindowPosition((monitorWidth - screenWidth) / 2, (monitorHeight - screenHeight) / 2);
    RestoreWindow();
<<<<<<< HEAD

=======
    if (IsTextureValid(sunTexture)) {
        SetMaterialTexture(&sunModel.materials[0], MATERIAL_MAP_DIFFUSE, sunTexture);
    }
    for (int i=0;i<nb_planetes;i++){
        planetes[i].modele = LoadModelFromMesh(GenMeshSphere(1.0f,32,32));
        planetes[i].texture=LoadTexture(planetes[i].texture_fichier);

        if (IsTextureValid(planetes[i].texture)) {
        SetMaterialTexture(
            &planetes[i].modele.materials[0],
            MATERIAL_MAP_DIFFUSE,
            planetes[i].texture
        );
        }
         else {
        printf("Texture non chargee pour %s : %s\n",planetes[i].nom,planetes[i].texture_fichier);
        }
    }


    }
>>>>>>> 97ef667 (ajout des textures):src/systeme_solaire.c
=======
    Model sphereModel = LoadModelFromMesh(GenMeshSphere(1.0f, 64, 64));
    sphereModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
    
    Texture2D textureSoleil = LoadTexture("texture/soleil.png");
    if (textureSoleil.id == 0) {
            printf("Erreur chargement texture soleil\n");
        }
    Texture2D texturesPlanetes[8];
    for (int i = 0; i < nb_planetes; i++) {
        texturesPlanetes[i] = LoadTexture(textures_planetes[i]);
        
    }

    // Halo lumineux du soleil
    Image haloImage = GenImageColor(512, 512, BLANK);
for (int y = 0; y < 512; y++) {
    for (int x = 0; x < 512; x++) {
        float dx = x - 256.0f;
        float dy = y - 256.0f;
        float dist = sqrtf(dx * dx + dy * dy) / 256.0f;
        if (dist <= 1.0f) {
            unsigned char alpha = (unsigned char)(255.0f * (1.0f - dist) * (1.0f - dist));
            ImageDrawPixel(&haloImage, x, y, (Color){ 255, 180, 40, alpha });
        }
    }
}
Texture2D haloTexture = LoadTextureFromImage(haloImage);
UnloadImage(haloImage);

//Fond étoilé
Texture2D textureSkybox = LoadTexture("texture/etoile.png");
Model skybox = LoadModelFromMesh(GenMeshSphere(200.0f, 32, 32)); // très grande sphère
skybox.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = textureSkybox;

#define NB_PARTICULES 80
typedef struct {
    Vector3 pos;
    float angle;
    float vitesse;
    float rayon;
    float taille;
    float phase;
} Particule;

Particule particules[NB_PARTICULES];
for (int i = 0; i < NB_PARTICULES; i++) {
    particules[i].angle   = ((float)rand() / RAND_MAX) * 2.0f * PI;
    particules[i].vitesse = 0.3f + ((float)rand() / RAND_MAX) * 0.5f;
    particules[i].rayon   = sunRadius + 0.2f + ((float)rand() / RAND_MAX) * 0.8f;
    particules[i].taille  = 0.3f + ((float)rand() / RAND_MAX) * 0.5f;
    particules[i].phase   = ((float)rand() / RAND_MAX) * 2.0f * PI;
}
>>>>>>> 59c9252 (textures des planetes et halo lumineux du soleil et fond etoilé)
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

            ClearBackground(BLACK);

            BeginMode3D(camera);

<<<<<<< HEAD
                DrawSunShiny(sunModel, sunRadius);

                rlPushMatrix();
=======
                // 1. Skybox en premier, faces intérieures visibles
                rlDisableBackfaceCulling();   // ← clé : voir l'intérieur de la sphère
                rlDisableDepthMask();
                DrawModel(skybox, (Vector3){0,0,0}, 1.0f, WHITE);
                rlEnableDepthMask();
                rlEnableBackfaceCulling();
                Vector3 soleilPos = { 0.0f, 0.0f, 0.0f };

                float t = (float)GetTime();
rlDisableDepthMask();
BeginBlendMode(BLEND_ADDITIVE);
for (int i = 0; i < NB_PARTICULES; i++) {
    particules[i].angle += particules[i].vitesse * GetFrameTime();

    // Rayon pulsant
    float r = particules[i].rayon + sinf(t * 2.0f + particules[i].phase) * 0.3f;

    Vector3 p = {
        cosf(particules[i].angle) * r,
        sinf(particules[i].phase + t) * 0.4f,  // légère variation verticale
        sinf(particules[i].angle) * r
    };

    // Couleur qui alterne jaune/orange
    float heat = (sinf(t * 3.0f + particules[i].phase) + 1.0f) / 2.0f;
    Color c = {
        255,
        (unsigned char)(100 + heat * 120),
        0,
        (unsigned char)(80 + heat * 80)
    };

    DrawBillboard(camera, haloTexture, p, particules[i].taille, c);
}
EndBlendMode();
rlEnableDepthMask();

                // // Lumière autour du soleil
                // BeginBlendMode(BLEND_ADDITIVE);
                // DrawBillboard(camera, haloTexture, soleilPos, sunRadius * 3.0f, Fade(YELLOW, 0.35f));
                // DrawBillboard(camera, haloTexture, soleilPos, sunRadius * 2.0f, Fade(ORANGE, 0.25f));
                // EndBlendMode();
                
                // Soleil texturé
                sphereModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = textureSoleil;

                DrawModelEx(sphereModel,soleilPos,(Vector3){ 0.0f, 1.0f, 0.0f },(float)GetTime() * 8.0f,(Vector3){ sunRadius, sunRadius, sunRadius },WHITE);

                
>>>>>>> 59c9252 (textures des planetes et halo lumineux du soleil et fond etoilé)
                    for (int i = 0; i < nb_planetes; i++) {
                        Array_Vecteurs trajectoire = planetes.values[i];
                        Vecteurs position = trajectoire.values[index % trajectoire.size];

                        Vector3 pos = {
                            (float)(position.x * SCALE),
                            (float)(position.z * SCALE),
                            (float)(position.y * SCALE)
                        };

<<<<<<< HEAD
<<<<<<< HEAD:src/systeme.c
                        rlPushMatrix();
                            rlTranslatef(pos.x, pos.y, pos.z);
                            rlScalef(rayons_planetes[i], rayons_planetes[i], rayons_planetes[i]);
                            DrawSphereBasic(couleurs_planetes[i]);
                        rlPopMatrix();
=======
                        float rotation = GetTime() * 20.0f;

                        DrawModelEx(planetes[i].modele,pos,(Vector3){ 0.0f, 1.0f, 0.0f },rotation,(Vector3){planetes[i].rayon,planetes[i].rayon,planetes[i].rayon},WHITE);
>>>>>>> 97ef667 (ajout des textures):src/systeme_solaire.c
                    }
                rlPopMatrix();
=======
                sphereModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texturesPlanetes[i];

                DrawModelEx(sphereModel,pos,(Vector3){ 0.0f, 1.0f, 0.0f },(float)GetTime() * (20.0f + i * 3.0f),(Vector3){rayons_planetes[i],rayons_planetes[i],rayons_planetes[i]},WHITE);
                }
                
>>>>>>> 59c9252 (textures des planetes et halo lumineux du soleil et fond etoilé)

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
<<<<<<< HEAD
=======

                rlDisableDepthMask();
            BeginBlendMode(BLEND_ADDITIVE);
                DrawBillboard(camera, haloTexture, soleilPos, sunRadius * 6.0f, WHITE);
            EndBlendMode();
            rlEnableDepthMask();
>>>>>>> 59c9252 (textures des planetes et halo lumineux du soleil et fond etoilé)
                DrawGrid(40, 2.0f);

            EndMode3D();

            DrawText("EARTH ORBITING AROUND THE SUN!", 400, 10, 20, MAROON);
            DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    for (int i = 0; i < nb_planetes; i++) {
<<<<<<< HEAD
<<<<<<< HEAD:src/systeme.c
        array_free(planetes.values[i]);
=======
        free(planetes[i].etats);
        UnloadTexture(planetes[i].texture);
        UnloadModel(planetes[i].modele);
>>>>>>> 97ef667 (ajout des textures):src/systeme_solaire.c
    }
    array_free(planetes);

    UnloadTexture(sunTexture);
    UnloadModel(sunModel);
=======
        array_free(planetes.values[i]);
    }
    array_free(planetes);

    for (int i = 0; i < nb_planetes; i++) {
    UnloadTexture(texturesPlanetes[i]);
    }

    UnloadTexture(textureSoleil);
    UnloadModel(sphereModel);

    UnloadTexture(textureSkybox);
    UnloadModel(skybox);
>>>>>>> 59c9252 (textures des planetes et halo lumineux du soleil et fond etoilé)
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
<<<<<<< HEAD

=======
}
>>>>>>> 59c9252 (textures des planetes et halo lumineux du soleil et fond etoilé)

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
<<<<<<< HEAD

void DrawSunShiny(Model sunModel, float radius)
{
    float time = GetTime();
    float pulse = 1.0f + 0.08f * sinf(time * 3.0f);

    // Halo lumineux autour du soleil
    BeginBlendMode(BLEND_ADDITIVE);

        DrawSphereEx(
            (Vector3){ 0.0f, 0.0f, 0.0f },
            radius * 1.45f * pulse,
            32,
            32,
            Fade(ORANGE, 0.20f)
        );

        DrawSphereEx(
            (Vector3){ 0.0f, 0.0f, 0.0f },
            radius * 2.10f * pulse,
            32,
            32,
            Fade(YELLOW, 0.10f)
        );

        // Rayons autour du soleil dans le plan XZ
        int rayCount = 32;

        for (int i = 0; i < rayCount; i++) {
            float angle = 2.0f * PI * i / rayCount;
            float rayLength = radius * (1.9f + 0.25f * sinf(time * 4.0f + i));

            Vector3 start = {
                cosf(angle) * radius * 1.15f,
                0.0f,
                sinf(angle) * radius * 1.15f
            };

            Vector3 end = {
                cosf(angle) * rayLength,
                0.0f,
                sinf(angle) * rayLength
            };

            DrawLine3D(start, end, Fade(GOLD, 0.85f));
        }

    EndBlendMode();

    // Corps du soleil texturé
    DrawModelEx(
        sunModel,
        (Vector3){ 0.0f, 0.0f, 0.0f },
        (Vector3){ 0.0f, 1.0f, 0.0f },
        time * 8.0f,
        (Vector3){ radius, radius, radius },
        WHITE
    );
}
=======
>>>>>>> 59c9252 (textures des planetes et halo lumineux du soleil et fond etoilé)
