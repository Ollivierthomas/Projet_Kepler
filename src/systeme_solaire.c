/// @file systeme_solaire.c
/// @brief Simulation et affichage 3D d'un système solaire avec effet de supernova.

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "systeme_solaire.h"

// ------------------------------------------------------------
// Donnees des planetes
// ------------------------------------------------------------

struct Planete donnees_planetes[] = {
    { "mercure", 3.3011e23, 46.00e9, 69.82e9 },
    { "venus", 4.8675e24, 107.48e9, 108.94e9 },
    { "terre", 5.9720e24, 147.09e9, 152.10e9 },
    { "mars", 6.4171e23, 206.62e9, 249.23e9 },
    { "jupiter", 1.8982e27, 740.52e9, 816.62e9 },
    { "saturne", 5.6834e26, 1352.55e9, 1514.50e9 },
    { "uranus", 8.6810e25, 2741.30e9, 3003.62e9 },
    { "neptune", 1.02413e26, 4444.45e9, 4545.67e9 }
};

Color couleurs_planetes[] = {
    GRAY, ORANGE, BLUE, RED, BROWN, GOLD, SKYBLUE, DARKBLUE
};

float rayons_affichage_planetes[] = {
    3.60f, 3.36f, 3.84f, 3.00f, 9.00f, 7.80f, 6.60f, 6.60f
};

const char *fichiers_textures_planetes[] = {
    "assets/texture/mercure.png", "assets/texture/venus.png",
    "assets/texture/terre.png", "assets/texture/mars.png",
    "assets/texture/jupiter.png", "assets/texture/saturn.png",
    "assets/texture/uranus.png", "assets/texture/neptune.png"
};

// ------------------------------------------------------------
// Conversion des positions de simulation vers l'affichage 3D
// ------------------------------------------------------------


float convertir_distance_en_affichage(double d) {
    if (d <= 0.0) {
        return 0.0f;
    }
    return (float)(sqrt(d) * ECHELLE_DISTANCE_AFFICHAGE);
}


Vector3 convertir_position_simulation_en_3d(Vecteurs pos) {
    double d = sqrt(pos.x * pos.x + pos.y * pos.y);
    float dv = convertir_distance_en_affichage(d);
    
    float nx = 1.0f;
    float ny = 0.0f;
    
    if (d > 0.0) {
        nx = (float)(pos.x / d);
        ny = (float)(pos.y / d);
    }
    
    return (Vector3){ nx * dv, 0.0f, ny * dv };
}

// ------------------------------------------------------------
// Gestion des debris (explosion supernova)
// ------------------------------------------------------------

void exploser_planete(GroupeDebris *g, Vector3 centre, Color couleur, float rayon_planete) {
    g->actif = 1;
    for (int i = 0; i < DEBRIS_PAR_PLANETE; i++) {
        float theta = ((float)rand() / RAND_MAX) * 2.0f * PI;
        float phi = acosf(2.0f * ((float)rand() / RAND_MAX) - 1.0f);
        float speed = rayon_planete * (2.5f + 5.0f * ((float)rand() / RAND_MAX));

        g->items[i].position = centre;
        g->items[i].vitesse = (Vector3){
            sinf(phi) * cosf(theta) * speed,
            cosf(phi) * speed * 0.6f,
            sinf(phi) * sinf(theta) * speed
        };
        
        g->items[i].rayon = rayon_planete * (0.04f + 0.10f * ((float)rand() / RAND_MAX));
        g->items[i].couleur = couleur;
        g->items[i].vie = 1.0f;

        float ax_theta = ((float)rand() / RAND_MAX) * 2.0f * PI;
        float ax_phi = acosf(2.0f * ((float)rand() / RAND_MAX) - 1.0f);
        g->items[i].axe_rotation = (Vector3){
            sinf(ax_phi) * cosf(ax_theta),
            cosf(ax_phi),
            sinf(ax_phi) * sinf(ax_theta)
        };
        
        float direction_rot = (((float)rand() / RAND_MAX) < 0.5f) ? -1.0f : 1.0f;
        g->items[i].vitesse_rotation = (1.0f + 4.0f * ((float)rand() / RAND_MAX)) * direction_rot;
        g->items[i].angle_rotation = 0.0f;
    }
}

void mettre_a_jour_debris(GroupeDebris *g, float dt) {
    if (!g->actif) {
        return;
    }
    
    for (int d = 0; d < DEBRIS_PAR_PLANETE; d++) {
        Debris *db = &g->items[d];
        if (db->vie <= 0.0f) {
            continue;
        }
        
        db->position.x += db->vitesse.x * dt;
        db->position.y += db->vitesse.y * dt;
        db->position.z += db->vitesse.z * dt;
        
        // Décélération progressive
        db->vitesse.x *= 0.995f;
        db->vitesse.y *= 0.995f;
        db->vitesse.z *= 0.995f;
        
        db->vie -= dt * 0.12f;
        db->angle_rotation += db->vitesse_rotation * dt * RAD2DEG;
    }
}

void dessiner_debris(GroupeDebris *g, int avec_trainee) {
    if (!g->actif) {
        return;
    }
    
    for (int d = 0; d < DEBRIS_PAR_PLANETE; d++) {
        Debris *db = &g->items[d];
        if (db->vie <= 0.0f) {
            continue;
        }

        Color c = db->couleur;
        c.a = (unsigned char)(db->vie * 255.0f);
        
        rlPushMatrix();
            rlTranslatef(db->position.x, db->position.y, db->position.z);
            rlRotatef(db->angle_rotation, db->axe_rotation.x, db->axe_rotation.y, db->axe_rotation.z);
            DrawSphereEx((Vector3){0, 0, 0}, db->rayon, 5, 5, c);
        rlPopMatrix();

        Color halo;
        if (avec_trainee) {
            halo = (Color){ 255, 200, 80, (unsigned char)(db->vie * 100.0f) };
        } else {
            halo = (Color){ 200, 200, 200, (unsigned char)(db->vie * 90.0f) };
        }
        
        DrawSphereEx(db->position, db->rayon * 1.6f, 5, 5, halo);

        if (!avec_trainee) {
            continue;
        }

        Vector3 dir = Vector3Normalize(db->vitesse);
        float vitesse_norme = Vector3Length(db->vitesse);
        int nb_segments = 6;
        
        for (int t = 1; t <= nb_segments; t++) {
            float ft = (float)t * 0.4f;
            Vector3 trail = {
                db->position.x - dir.x * ft * (vitesse_norme * 0.02f + db->rayon * 2.0f),
                db->position.y - dir.y * ft * (vitesse_norme * 0.02f + db->rayon * 2.0f),
                db->position.z - dir.z * ft * (vitesse_norme * 0.02f + db->rayon * 2.0f)
            };
            float facteur = 1.0f - (float)t / (nb_segments + 1);
            Color ct = { 255, 170, 60, (unsigned char)(db->vie * 90.0f * facteur) };
            DrawSphereEx(trail, db->rayon * (0.8f * facteur + 0.1f), 4, 4, ct);
        }
    }
}

// ------------------------------------------------------------
// Dessin du soleil
// ------------------------------------------------------------

void dessiner_soleil_texture(Model modele_soleil, float rayon, int texture_chargee, Color teinte) {
    float temps = (float)GetTime();
    float pulsation = 1.0f + 0.05f * sinf(temps * 2.5f);

    Color couleur_modele = texture_chargee ? teinte : YELLOW;

    DrawModelEx(
        modele_soleil,
        (Vector3){0, 0, 0},
        (Vector3){0, 1, 0},
        temps * 8.0f,
        (Vector3){rayon, rayon, rayon},
        couleur_modele
    );

    BeginBlendMode(BLEND_ADDITIVE);
        DrawSphereEx((Vector3){0, 0, 0}, rayon * 1.25f * pulsation, 32, 32, Fade((Color){255, 180, 30, 255}, 0.35f));
        DrawSphereEx((Vector3){0, 0, 0}, rayon * 1.80f * pulsation, 32, 32, Fade((Color){255, 120, 0, 255}, 0.12f));

        int nb = 12;
        for (int i = 0; i < nb; i++) {
            float angle = 2.0f * PI * i / nb;
            float phase = temps * 1.2f + i * 0.8f;
            float haut = rayon * (0.25f + 0.20f * sinf(phase));
            float larg = rayon * (0.30f + 0.15f * cosf(phase * 0.7f));
            float incl = (float)i / nb * PI;

            Vector3 bg = { cosf(angle) * rayon * 1.05f, sinf(incl) * rayon * 0.05f, sinf(angle) * rayon * 1.05f };
            Vector3 sm = { cosf(angle) * (rayon + larg), haut * cosf(incl), sinf(angle) * (rayon + larg) };
            Vector3 bd = { cosf(angle + 0.35f) * rayon * 1.05f, sinf(incl) * rayon * 0.05f, sinf(angle + 0.35f) * rayon * 1.05f };

            for (int s = 0; s < 8; s++) {
                float t0 = (float)s / 8;
                float t1 = (float)(s + 1) / 8;
                
                float u0 = (1 - t0) * (1 - t0);
                float v0 = 2 * (1 - t0) * t0;
                float w0 = t0 * t0;
                
                float u1 = (1 - t1) * (1 - t1);
                float v1 = 2 * (1 - t1) * t1;
                float w1 = t1 * t1;
                
                Vector3 p0 = { u0*bg.x + v0*sm.x + w0*bd.x, u0*bg.y + v0*sm.y + w0*bd.y, u0*bg.z + v0*sm.z + w0*bd.z };
                Vector3 p1 = { u1*bg.x + v1*sm.x + w1*bd.x, u1*bg.y + v1*sm.y + w1*bd.y, u1*bg.z + v1*sm.z + w1*bd.z };
                
                float alpha = 0.9f * (1.0f - fabsf(t0 + t1 - 1.0f));
                DrawLine3D(p0, p1, Fade((Color){255, 160, 40, 255}, alpha));
            }
        }
    EndBlendMode();
}

void dessiner_sphere_simple(Color couleur) {
    int a = 16;
    int t = 16;
    rlCheckRenderBatchLimit((a + 2) * t * 6);
    rlBegin(RL_TRIANGLES);
        rlColor4ub(couleur.r, couleur.g, couleur.b, couleur.a);
        for (int i = 0; i < a + 2; i++) {
            for (int j = 0; j < t; j++) {
                rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(a+1))*i)) *sinf(DEG2RAD*(j*360.0f/t)), sinf(DEG2RAD*(270+(180.0f/(a+1))*i)), cosf(DEG2RAD*(270+(180.0f/(a+1))*i)) *cosf(DEG2RAD*(j*360.0f/t)));
                rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(a+1))*(i+1)))*sinf(DEG2RAD*((j+1)*360.0f/t)),sinf(DEG2RAD*(270+(180.0f/(a+1))*(i+1))),cosf(DEG2RAD*(270+(180.0f/(a+1))*(i+1)))*cosf(DEG2RAD*((j+1)*360.0f/t)));
                rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(a+1))*(i+1)))*sinf(DEG2RAD*(j*360.0f/t)), sinf(DEG2RAD*(270+(180.0f/(a+1))*(i+1))),cosf(DEG2RAD*(270+(180.0f/(a+1))*(i+1)))*cosf(DEG2RAD*(j*360.0f/t)));
                rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(a+1))*i)) *sinf(DEG2RAD*(j*360.0f/t)), sinf(DEG2RAD*(270+(180.0f/(a+1))*i)), cosf(DEG2RAD*(270+(180.0f/(a+1))*i)) *cosf(DEG2RAD*(j*360.0f/t)));
                rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(a+1))*i)) *sinf(DEG2RAD*((j+1)*360.0f/t)),sinf(DEG2RAD*(270+(180.0f/(a+1))*i)), cosf(DEG2RAD*(270+(180.0f/(a+1))*i)) *cosf(DEG2RAD*((j+1)*360.0f/t)));
                rlVertex3f(cosf(DEG2RAD*(270+(180.0f/(a+1))*(i+1)))*sinf(DEG2RAD*((j+1)*360.0f/t)),sinf(DEG2RAD*(270+(180.0f/(a+1))*(i+1))),cosf(DEG2RAD*(270+(180.0f/(a+1))*(i+1)))*cosf(DEG2RAD*((j+1)*360.0f/t)));
            }
        }
    rlEnd();
}

// ------------------------------------------------------------
// Camera Orbitale
// ------------------------------------------------------------

void mettre_a_jour_camera(Camera *camera, CameraOrbitale *orbite) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 delta = GetMouseDelta();
        orbite->angle_horizontal -= delta.x * CAMERA_VITESSE_ROTATION;
        orbite->angle_vertical -= delta.y * CAMERA_VITESSE_ROTATION;

        float limite = DEG2RAD * 89.0f;
        if (orbite->angle_vertical > limite) {
            orbite->angle_vertical = limite;
        }
        if (orbite->angle_vertical < -limite) {
            orbite->angle_vertical = -limite;
        }
    }

    float zoom = GetMouseWheelMove() * CAMERA_VITESSE_ZOOM * 5.0f;
    if (IsKeyDown(KEY_UP)) {
        zoom -= CAMERA_VITESSE_ZOOM;
    }
    if (IsKeyDown(KEY_DOWN)) {
        zoom += CAMERA_VITESSE_ZOOM;
    }
    
    orbite->distance += zoom;
    
    if (orbite->distance < CAMERA_DISTANCE_MIN) {
        orbite->distance = CAMERA_DISTANCE_MIN;
    }
    if (orbite->distance > CAMERA_DISTANCE_MAX) {
        orbite->distance = CAMERA_DISTANCE_MAX;
    }

    if (IsKeyDown(KEY_LEFT)) {
        orbite->cible.x -= CAMERA_VITESSE_DEPLACEMENT;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        orbite->cible.x += CAMERA_VITESSE_DEPLACEMENT;
    }

    float cv = cosf(orbite->angle_vertical);
    Vector3 decalage = {
        orbite->distance * cv * sinf(orbite->angle_horizontal),
        orbite->distance * sinf(orbite->angle_vertical),
        orbite->distance * cv * cosf(orbite->angle_horizontal)
    };

    camera->target = orbite->cible;
    camera->position = (Vector3){
        orbite->cible.x + decalage.x,
        orbite->cible.y + decalage.y,
        orbite->cible.z + decalage.z
    };
}

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------

int main(void) {
    double G = 6.67430e-11;
    double m_sol = 1.989e30;
    double m_terre = 5.9720e24;

    Array(Trajectoire) trajectoires_planetes = {0};
    for (int i = 0; i < NOMBRE_PLANETES; i++) {
        array_push(trajectoires_planetes, simuler_planete(donnees_planetes[i], G, m_sol));
    }

    struct Planete donnees_lune = { "lune", 7.35e22, 356700e3, 406300e3 };
    Trajectoire traj_lune = simuler_planete(donnees_lune, G, m_terre);

    Vector3 *traj_vis[NOMBRE_PLANETES];
    for (int i = 0; i < NOMBRE_PLANETES; i++) {
        Trajectoire t = trajectoires_planetes.values[i];
        traj_vis[i] = malloc(t.size * sizeof(Vector3));
        for (size_t j = 0; j < t.size; j++) {
            traj_vis[i][j] = convertir_position_simulation_en_3d(t.values[j]);
        }
    }

    Vector3 *traj_lune_vis = malloc(traj_lune.size * sizeof(Vector3));
    for (size_t i = 0; i < traj_lune.size; i++) {
        Vecteurs pl = traj_lune.values[i];
        double dl = sqrt(pl.x * pl.x + pl.y * pl.y);
        float dlv = (float)(sqrt(dl) * ECHELLE_DISTANCE_AFFICHAGE * ECHELLE_DISTANCE_LUNE);
        
        float dx = 1.0f;
        float dy = 0.0f;
        if (dl > 0.0) {
            dx = (float)(pl.x / dl);
            dy = (float)(pl.y / dl);
        }
        
        traj_lune_vis[i] = (Vector3){ dx * dlv, 0.0f, dy * dlv };
    }

    GroupeDebris debris_planetes[NOMBRE_PLANETES] = {0};
    GroupeDebris debris_lune = {0};

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1200, 700, "Système Solaire");
    InitAudioDevice();

    Music musique_ambiance = LoadMusicStream("assets/audio/music.mp3");
    Music musique_explosion = LoadMusicStream("assets/audio/music_explosion.mp3");
    musique_ambiance.looping = true;
    musique_explosion.looping = false;

    SetMusicVolume(musique_ambiance, 0.45f);
    SetMusicVolume(musique_explosion, 0.85f);
    PlayMusicStream(musique_ambiance);

    const float rayon_sol = 10.5f;

    Model mod_sol = LoadModelFromMesh(GenMeshSphere(1.0f, 32, 32));
    Texture2D tex_sol = LoadTexture("assets/texture/soleil.png");
    int tex_sol_ok = tex_sol.id > 0;
    if (tex_sol_ok) {
        SetMaterialTexture(&mod_sol.materials[0], MATERIAL_MAP_DIFFUSE, tex_sol);
    }

    Model mod_planetes[NOMBRE_PLANETES];
    Texture2D tex_planetes[NOMBRE_PLANETES];
    int tex_ok[NOMBRE_PLANETES] = {0};
    
    for (int i = 0; i < NOMBRE_PLANETES; i++) {
        mod_planetes[i] = LoadModelFromMesh(GenMeshSphere(1.0f, 32, 32));
        tex_planetes[i] = LoadTexture(fichiers_textures_planetes[i]);
        tex_ok[i] = tex_planetes[i].id > 0;
        if (tex_ok[i]) {
            SetMaterialTexture(&mod_planetes[i].materials[0], MATERIAL_MAP_DIFFUSE, tex_planetes[i]);
        }
    }

    Model mod_lune = LoadModelFromMesh(GenMeshSphere(1.0f, 32, 32));
    Texture2D tex_lune = LoadTexture("assets/texture/lune.png");
    int tex_lune_ok = tex_lune.id > 0;
    if (tex_lune_ok) {
        SetMaterialTexture(&mod_lune.materials[0], MATERIAL_MAP_DIFFUSE, tex_lune);
    }

    Texture2D tex_sky = LoadTexture("assets/texture/etoile.png");
    Model skybox = LoadModelFromMesh(GenMeshSphere(800.0f, 64, 64));
    skybox.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tex_sky;

    CameraOrbitale orbite = {0};
    orbite.cible = (Vector3){ 0.0f, 0.0f, 0.0f };
    orbite.distance = 778.0f;
    orbite.angle_horizontal = 0.0f;
    orbite.angle_vertical = DEG2RAD * 45.0f;

    Camera camera = {0};
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    mettre_a_jour_camera(&camera, &orbite);

    size_t index_anim = 0;
    int vitesse = 3;

    PhaseSupernova phase = PHASE_NORMAL;
    float timer = 0.0f;
    float rayon_soleil = rayon_sol;
    float ecran_alpha = 0.0f;
    int planete_visible[NOMBRE_PLANETES];
    int lune_visible = 1;
    
    for (int i = 0; i < NOMBRE_PLANETES; i++) {
        planete_visible[i] = 1;
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        index_anim += (size_t)vitesse;
        float dt = GetFrameTime();

        UpdateMusicStream(musique_ambiance);
        UpdateMusicStream(musique_explosion);

        mettre_a_jour_camera(&camera, &orbite);

        if (IsKeyPressed(KEY_SPACE) && phase == PHASE_NORMAL) {
            phase = PHASE_GONFLEMENT;
            timer = 0.0f;
            StopMusicStream(musique_ambiance);
            PlayMusicStream(musique_explosion);
        }
        

        if (phase == PHASE_GONFLEMENT) {
            timer += dt;
            float t = timer / DUREE_GONFLEMENT;
            if (t > 1.0f) {
                t = 1.0f;
            }
            rayon_soleil = rayon_sol * (1.0f + t * 7.0f);
            
            if (timer >= DUREE_GONFLEMENT) {
                phase = PHASE_FLASH;
                timer = 0.0f;
                ecran_alpha = 0.0f;


            }
        } else if (phase == PHASE_FLASH) {
            timer += dt;
            float t = timer / DUREE_FLASH;
            
            if (t < 0.5f) {
                ecran_alpha = t * 2.0f;
            } else {
                ecran_alpha = (1.0f - t) * 2.0f;
            }
            
            if (ecran_alpha < 0.0f) {
                ecran_alpha = 0.0f;
            }

            if (timer >= DUREE_FLASH * 0.5f) {
                for (int i = 0; i < NOMBRE_PLANETES; i++) {
                    if (planete_visible[i] && !debris_planetes[i].actif) {
                        size_t n = trajectoires_planetes.values[i].size;
                        Vector3 pos = traj_vis[i][index_anim % n];
                        exploser_planete(&debris_planetes[i], pos, couleurs_planetes[i], rayons_affichage_planetes[i]);
                        planete_visible[i] = 0;
                    }
                }
                
                if (lune_visible && !debris_lune.actif) {
                    size_t nt = trajectoires_planetes.values[INDEX_TERRE].size;
                    size_t nl = traj_lune.size;
                    Vector3 pt = traj_vis[INDEX_TERRE][index_anim % nt];
                    Vector3 ol = traj_lune_vis[index_anim % nl];
                    Vector3 pl = { pt.x + ol.x, 0.0f, pt.z + ol.z };
                    exploser_planete(&debris_lune, pl, LIGHTGRAY, 1.05f);
                    lune_visible = 0;
                }
                rayon_soleil = 0.0f;
            }

            if (timer >= DUREE_FLASH) {
                phase = PHASE_EXPLOSION;
                timer = 0.0f;
                ecran_alpha = 0.0f;
            }
        } else if (phase == PHASE_EXPLOSION) {
            timer += dt;
            if (timer >= DUREE_EXPLOSION) {
                phase = PHASE_FONDU;
                timer = 0.0f;
            }
        } else if (phase == PHASE_FONDU) {
            timer += dt;
            ecran_alpha = timer / DUREE_FONDU;
            if (ecran_alpha > 1.0f) {
                ecran_alpha = 1.0f;
            }
        }

        for (int i = 0; i < NOMBRE_PLANETES; i++) {
            mettre_a_jour_debris(&debris_planetes[i], dt);
        }
        mettre_a_jour_debris(&debris_lune, dt);

        Color teinte_soleil = WHITE;
        if (phase == PHASE_GONFLEMENT) {
            float t = timer / DUREE_GONFLEMENT;
            if (t > 1.0f) {
                t = 1.0f;
            }
            unsigned char g_col = (unsigned char)(220.0f * (1.0f - t));
            unsigned char b_col = (unsigned char)(50.0f * (1.0f - t));
            teinte_soleil = (Color){ 255, g_col, b_col, 255 };
        }

        // --- Gestion de l'intensité et du clignotement des trajectoires ---
        float alpha_trajectoire = 1.0f;
        if (phase == PHASE_EXPLOSION) {
            float clignotement = 0.6f + 0.4f * sinf((float)GetTime() * 25.0f);
            float progression_extinction = 1.0f - (timer / DUREE_EXPLOSION);
            alpha_trajectoire = clignotement * progression_extinction;
        } else if (phase == PHASE_FONDU) {
            alpha_trajectoire = 0.0f;
        }
        
        if (alpha_trajectoire < 0.0f) {
            alpha_trajectoire = 0.0f;
        }

        // ============================
        //          DESSIN
        // ============================
        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode3D(camera);

                rlDisableBackfaceCulling();
                rlDisableDepthMask();
                rlDisableDepthTest();
                DrawModel(skybox, (Vector3){0, 0, 0}, 1.0f, WHITE);
                rlEnableDepthTest();
                rlEnableDepthMask();
                rlEnableBackfaceCulling();

                if (rayon_soleil > 0.0f) {
                    dessiner_soleil_texture(mod_sol, rayon_soleil, tex_sol_ok, teinte_soleil);
                }

                // Trajectoires des planètes
                if (alpha_trajectoire > 0.0f) {
                    for (int i = 0; i < NOMBRE_PLANETES; i++) {
                        Color couleur_effet = Fade(couleurs_planetes[i], alpha_trajectoire);
                        size_t n = trajectoires_planetes.values[i].size;
                        int pas = (int)(n / 300);
                        
                        if (pas < 1) {
                            pas = 1;
                        }
                        
                        size_t j = 0;
                        while (j + pas < n) {
                            DrawLine3D(traj_vis[i][j], traj_vis[i][j + pas], couleur_effet);
                            j += pas;
                        }
                        DrawLine3D(traj_vis[i][j], traj_vis[i][0], couleur_effet);
                    }
                }

                // Rendu des planètes
                for (int i = 0; i < NOMBRE_PLANETES; i++) {
                    if (!planete_visible[i]) {
                        continue;
                    }
                    
                    size_t n = trajectoires_planetes.values[i].size;
                    Vector3 pos = traj_vis[i][index_anim % n];
                    float rot = (float)GetTime() * 20.0f;
                    float r = rayons_affichage_planetes[i];
                    
                    mod_planetes[i].transform = MatrixRotateX(PI / 2.0f);
                    
                    Color couleur_planete = tex_ok[i] ? WHITE : couleurs_planetes[i];
                    
                    DrawModelEx(
                        mod_planetes[i], 
                        pos, 
                        (Vector3){0, 1, 0}, 
                        rot, 
                        (Vector3){r, r, r}, 
                        couleur_planete
                    );
                }

                // Rendu de la lune
                if (lune_visible) {
                    size_t nt = trajectoires_planetes.values[INDEX_TERRE].size;
                    size_t nl = traj_lune.size;
                    
                    Vector3 pt = traj_vis[INDEX_TERRE][index_anim % nt];
                    Vector3 ol = traj_lune_vis[index_anim % nl];
                    Vector3 pl = { pt.x + ol.x, 0.0f, pt.z + ol.z };
                    
                    float rot_l = 90.0f + (float)GetTime() * 20.0f;
                    Color couleur_lune = tex_lune_ok ? WHITE : LIGHTGRAY;
                    
                    DrawModelEx(
                        mod_lune, 
                        pl, 
                        (Vector3){0, 1, 0}, 
                        rot_l, 
                        (Vector3){1.05f, 1.05f, 1.05f}, 
                        couleur_lune
                    );
                }

                BeginBlendMode(BLEND_ADDITIVE);
                for (int i = 0; i < NOMBRE_PLANETES; i++) {
                    dessiner_debris(&debris_planetes[i], 1);
                }
                dessiner_debris(&debris_lune, 0);
                EndBlendMode();

            EndMode3D();

            if (ecran_alpha > 0.0f) {
                Color voile;
                if (phase == PHASE_FLASH || (phase == PHASE_EXPLOSION && timer < 0.3f)) {
                    voile = (Color){ 255, 255, 255, (unsigned char)(ecran_alpha * 255) };
                } else {
                    voile = (Color){ 0, 0, 0, (unsigned char)(ecran_alpha * 255) };
                }
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), voile);
            }

            DrawText("SYSTEME SOLAIRE", 10, 10, 20, RAYWHITE);
            DrawText("Souris (clic gauche) : orienter  |  Molette/Haut/Bas : zoom  |  Gauche/Droite : deplacer  |  ESPACE : supernova",
                     10, 35, 14, GRAY);
            DrawFPS(10, GetScreenHeight() - 25);

        EndDrawing();
    }

    StopMusicStream(musique_ambiance);
    StopMusicStream(musique_explosion);
    UnloadMusicStream(musique_ambiance);
    UnloadMusicStream(musique_explosion);

    // Libération de la mémoire
    for (int i = 0; i < NOMBRE_PLANETES; i++) {
        array_free(trajectoires_planetes.values[i]);
        free(traj_vis[i]);
        if (tex_planetes[i].id > 0) {
            UnloadTexture(tex_planetes[i]);
        }
        UnloadModel(mod_planetes[i]);
    }
    
    if (tex_lune.id > 0) {
        UnloadTexture(tex_lune);
    }
    if (tex_sol.id > 0) {
        UnloadTexture(tex_sol);
    }
    
    UnloadModel(mod_lune);
    UnloadModel(mod_sol);
    UnloadTexture(tex_sky);
    UnloadModel(skybox);
    
    free(traj_lune_vis);
    array_free(trajectoires_planetes);
    array_free(traj_lune);
    
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}