#include "raylib.h" 
#include "rlgl.h" 
#include <math.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include "tab_generic.h" 
#include "trajectoire.h" 
#include "vectors.h"

// Distances visuelles (sqrt * SCALE_SQRT) :
//   Mercure  ~46e9  m -> sqrt(46e9)  * 3.5e-4 ≈  2.4 u
//   Venus   ~108e9  m -> sqrt(108e9) * 3.5e-4 ≈  3.6 u
//   Terre   ~150e9  m -> sqrt(150e9) * 3.5e-4 ≈  4.3 u
//   Mars    ~228e9  m -> sqrt(228e9) * 3.5e-4 ≈  5.3 u
//   Jupiter ~778e9  m -> sqrt(778e9) * 3.5e-4 ≈  9.8 u
//   Saturne ~1433e9 m -> sqrt(1433e9)* 3.5e-4 ≈ 13.3 u
//   Uranus  ~2872e9 m -> sqrt(2872e9)* 3.5e-4 ≈ 18.8 u
//   Neptune ~4495e9 m -> sqrt(4495e9)* 3.5e-4 ≈ 23.5 u
#define SCALE_SQRT     3.5e-4

// Rayon orbital de la lune (~384 000 km) -> offset visuel
// sqrt(384e6) * 3.5e-4 * LUNE_VIS_SCALE ≈ 0.55 u (visible, pas trop grand)
#define LUNE_VIS_SCALE 1.0f

#define INDEX_TERRE 2

typedef Array(Vecteurs) Array_Vecteurs;

static inline float dist_visuelle(double r_metres)
{
    if (r_metres <= 0.0) return 0.0f;
    return (float)(sqrt(r_metres) * SCALE_SQRT);
}

static inline Vector3 pos_vers_3d(Vecteurs pos_sim)
{
    double r  = sqrt(pos_sim.x * pos_sim.x + pos_sim.y * pos_sim.y);
    float r_vis = dist_visuelle(r);
    float nx  = (r > 0.0) ? (float)(pos_sim.x / r) : 1.0f;
    float ny  = (r > 0.0) ? (float)(pos_sim.y / r) : 0.0f;
    return (Vector3){ nx * r_vis, 0.0f, ny * r_vis };
}

struct Planete planete[] = {
    { "mercure", 3.3011e23,  46.00e9,    69.82e9   },
    { "venus",   4.8675e24,  107.48e9,  108.94e9   },
    { "terre",   5.9720e24,  147.09e9,  152.10e9   },
    { "mars",    6.4171e23,  206.62e9,  249.23e9   },
    { "jupiter", 1.8982e27,  740.52e9,  816.62e9   },
    { "saturne", 5.6834e26,  1352.55e9, 1514.50e9  },
    { "uranus",  8.6810e25,  2741.30e9, 3003.62e9  },
    { "neptune", 1.02413e26, 4444.45e9, 4545.67e9  }
};

int nb_planetes = sizeof(planete) / sizeof(planete[0]);

Color couleurs_planetes[] = {
    GRAY, ORANGE, BLUE, RED, BROWN, GOLD, SKYBLUE, DARKBLUE
};

// Rayons visuels calibrés pour être visibles sans se chevaucher :
// Les planètes intérieures (proches) sont petites, les géantes gazeuses plus grandes.
// Règle : rayon < (dist_suivante - dist_courante) / 2 en unités 3D
//   Mercure  à 2.4u, Venus  à 3.6u -> écart 1.2u -> rayon max ~0.5u
//   Venus    à 3.6u, Terre  à 4.3u -> écart 0.7u -> rayon max ~0.3u
//   Terre    à 4.3u, Mars   à 5.3u -> écart 1.0u -> rayon max ~0.4u
//   Mars     à 5.3u, Jupiter à 9.8u -> écart 4.5u -> rayon max ~0.5u
//   Jupiter  à 9.8u, Saturne à 13.3u -> écart 3.5u -> rayon max ~0.9u
//   Saturne  à 13.3u, Uranus à 18.8u -> écart 5.5u -> rayon max ~0.8u
//   Uranus   à 18.8u, Neptune à 23.5u -> écart 4.7u -> rayon max ~0.7u
//   Neptune  à 23.5u, bord   -> rayon max ~0.7u
float rayons_planetes[] = {
    3.60f, // mercure
    3.36f, // venus
    3.84f, // terre
    3.00f, // mars
    9.00f, // jupiter
    7.80f, // saturne
    6.60f, // uranus
    6.60f  // neptune  
};

void DrawSphereBasic(Color color);

int main(void) {
    double G        = 6.67430e-11;
    double m_soleil = 1.989e30;
    double m_terre  = 5.9720e24;

    // --- Simulation physique ---
    Array(Array_Vecteurs) planetes = {0};
    for (int i = 0; i < nb_planetes; i++) {
        array_push(planetes, simuler_planete(planete[i], G, m_soleil));
    }

    struct Planete lune_data = { "lune", 7.35e22, 356700e3, 406300e3 };
    Array_Vecteurs trajectoire_lune = simuler_planete(lune_data, G, m_terre);

    // --- Pré-calcul des trajectoires visuelles (une seule fois avant la boucle) ---
    Vector3 *traj_vis[8];
    for (int i = 0; i < nb_planetes; i++) {
        Array_Vecteurs traj = planetes.values[i];
        traj_vis[i] = (Vector3 *)malloc(traj.size * sizeof(Vector3));
        for (size_t k = 0; k < traj.size; k++) {
            traj_vis[i][k] = pos_vers_3d(traj.values[k]);
        }
    }

    // Lune : positions relatives centrées sur 0, mise à l'échelle visuelle
    Vector3 *lune_vis = (Vector3 *)malloc(trajectoire_lune.size * sizeof(Vector3));
    for (size_t k = 0; k < trajectoire_lune.size; k++) {
        Vecteurs pl = trajectoire_lune.values[k];
        double r_l  = sqrt(pl.x * pl.x + pl.y * pl.y);
        float r_l_vis = (float)(sqrt(r_l) * SCALE_SQRT * LUNE_VIS_SCALE);
        float nlx = (r_l > 0.0) ? (float)(pl.x / r_l) : 1.0f;
        float nly = (r_l > 0.0) ? (float)(pl.y / r_l) : 0.0f;
        lune_vis[k] = (Vector3){ nlx * r_l_vis, 0.0f, nly * r_l_vis };
    }

    // --- Fenêtre et caméra ---
    // Neptune est à ~23.5u -> caméra suffisamment haute et reculée pour tout voir
    const int screenWidth  = 1200;
    const int screenHeight = 700;
    size_t index = 0;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Système Solaire");

    Camera camera     = { 0 };
    camera.position   = (Vector3){ 0.0f, 550.0f, 550.0f };
    camera.target     = (Vector3){ 0.0f,  0.0f,  0.0f };
    camera.up         = (Vector3){ 0.0f,  1.0f,  0.0f };
    camera.fovy       = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    int vitesse_animation = 3;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        index += (size_t)vitesse_animation;

        // --- Contrôles caméra ---
        float speed      = 1.0f;
        float zoom_speed = 2.0f;

        if (IsKeyDown(KEY_LEFT))  { camera.position.x -= speed; camera.target.x -= speed; }
        if (IsKeyDown(KEY_RIGHT)) { camera.position.x += speed; camera.target.x += speed; }
        if (IsKeyDown(KEY_UP)) {
            camera.position.y -= zoom_speed;
            camera.position.z -= zoom_speed;
        }
        if (IsKeyDown(KEY_DOWN)) {
            camera.position.y += zoom_speed;
            camera.position.z += zoom_speed;
        }

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode3D(camera);

                // --- Soleil ---
                // Rayon solaire visuel ~1.5u (plus grand que les planètes mais pas écrasant)
                rlPushMatrix();
                    rlScalef(10.5f, 10.5f, 10.5f);
                    DrawSphereBasic(YELLOW);
                rlPopMatrix();

                // --- Trajectoires (précalculées, ~300 segments par orbite) ---
                for (int i = 0; i < nb_planetes; i++) {
                    size_t n   = planetes.values[i].size;
                    int    pas = (int)(n / 300);
                    if (pas < 1) pas = 1;
                    for (size_t k = 0; k + pas < n; k += pas) {
                        DrawLine3D(traj_vis[i][k], traj_vis[i][k + pas], couleurs_planetes[i]);
                    }
                    DrawLine3D(traj_vis[i][n - 1], traj_vis[i][0], couleurs_planetes[i]);
                }

                // --- Planètes ---
                for (int i = 0; i < nb_planetes; i++) {
                    size_t n   = planetes.values[i].size;
                    size_t idx = index % n;
                    Vector3 pos = traj_vis[i][idx];

                    rlPushMatrix();
                        rlTranslatef(pos.x, pos.y, pos.z);
                        rlScalef(rayons_planetes[i], rayons_planetes[i], rayons_planetes[i]);
                        DrawSphereBasic(couleurs_planetes[i]);
                    rlPopMatrix();
                }

                // --- Lune ---
                {
                    size_t n_t   = planetes.values[INDEX_TERRE].size;
                    size_t n_l   = trajectoire_lune.size;
                    size_t idx_t = index % n_t;
                    size_t idx_l = index % n_l;

                    Vector3 pt = traj_vis[INDEX_TERRE][idx_t];
                    Vector3 ol = lune_vis[idx_l];

                    Vector3 pos_lune = { pt.x + ol.x, 0.0f, pt.z + ol.z };

                    rlPushMatrix();
                        rlTranslatef(pos_lune.x, pos_lune.y, pos_lune.z);
                        rlScalef(1.05f, 1.05f, 1.05f);
                        DrawSphereBasic(LIGHTGRAY);
                    rlPopMatrix();
                }

            EndMode3D();

            // --- HUD ---
            DrawText("SYSTEME SOLAIRE", 10, 10, 20, RAYWHITE);
            DrawText("Fleches: deplacer  |  Haut/Bas: zoom", 10, 35, 14, GRAY);
            DrawFPS(10, screenHeight - 25);

        EndDrawing();
    }

    // --- Libération mémoire ---
    for (int i = 0; i < nb_planetes; i++) {
        array_free(planetes.values[i]);
        free(traj_vis[i]);
    }
    free(lune_vis);
    array_free(planetes);
    array_free(trajectoire_lune);

    CloseWindow();
    return 0;
}

void DrawSphereBasic(Color color) {
    int rings  = 16;
    int slices = 16;

    rlCheckRenderBatchLimit((rings + 2) * slices * 6);

    rlBegin(RL_TRIANGLES);
        rlColor4ub(color.r, color.g, color.b, color.a);

        for (int i = 0; i < (rings + 2); i++) {
            for (int j = 0; j < slices; j++) {
                rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(rings+1))*i))    * sinf(DEG2RAD*(j*360.0f/slices)),
                           sinf(DEG2RAD*(270+(180.0f/(rings+1))*i)),
                           cosf(DEG2RAD*(270+(180.0f/(rings+1))*i))    * cosf(DEG2RAD*(j*360.0f/slices)));

                rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(rings+1))*(i+1)))* sinf(DEG2RAD*((j+1)*360.0f/slices)),
                           sinf(DEG2RAD*(270+(180.0f/(rings+1))*(i+1))),
                           cosf(DEG2RAD*(270+(180.0f/(rings+1))*(i+1)))* cosf(DEG2RAD*((j+1)*360.0f/slices)));

                rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(rings+1))*(i+1)))* sinf(DEG2RAD*(j*360.0f/slices)),
                           sinf(DEG2RAD*(270+(180.0f/(rings+1))*(i+1))),
                           cosf(DEG2RAD*(270+(180.0f/(rings+1))*(i+1)))* cosf(DEG2RAD*(j*360.0f/slices)));

                rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(rings+1))*i))    * sinf(DEG2RAD*(j*360.0f/slices)),
                           sinf(DEG2RAD*(270+(180.0f/(rings+1))*i)),
                           cosf(DEG2RAD*(270+(180.0f/(rings+1))*i))    * cosf(DEG2RAD*(j*360.0f/slices)));

                rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(rings+1))*i))    * sinf(DEG2RAD*((j+1)*360.0f/slices)),
                           sinf(DEG2RAD*(270+(180.0f/(rings+1))*i)),
                           cosf(DEG2RAD*(270+(180.0f/(rings+1))*i))    * cosf(DEG2RAD*((j+1)*360.0f/slices)));

                rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(rings+1))*(i+1)))* sinf(DEG2RAD*((j+1)*360.0f/slices)),
                           sinf(DEG2RAD*(270+(180.0f/(rings+1))*(i+1))),
                           cosf(DEG2RAD*(270+(180.0f/(rings+1))*(i+1)))* cosf(DEG2RAD*((j+1)*360.0f/slices)));
            }
        }
    rlEnd();
}