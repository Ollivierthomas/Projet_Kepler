# Projet Kepler - Simulation du Système Solaire

Une simulation 3D interactive du système solaire en C avec physique gravitationnelle, visualisation en temps réel et effets visuels.

## Aperçu

- 🪐 Simulation des 8 planètes + la Lune
- 🎨 Rendu 3D avec Raylib
- 🔬 Calculs physiques précis (lois de Kepler, orbites elliptiques)
- 🎵 Bande sonore intégrée
- ⌨️ Commandes interactives

## Prérequis

- **Linux** (avec support X11)
- **Compilateur C** (gcc/clang avec C23)
- **Meson** (système de build)
- **Pkg-config**

## Installation des dépendances

### Ubuntu/Debian
```bash
sudo apt-get install -y \
  meson pkg-config \
  libraylib-dev \
  libgl1-mesa-dev \
  libx11-dev \
  libpthread-stubs0-dev
```

### Fedora/RHEL
```bash
sudo dnf install -y \
  meson pkg-config \
  raylib-devel \
  mesa-libGL-devel \
  libX11-devel
```

### Arch
```bash
sudo pacman -S \
  meson pkg-config \
  raylib \
  libx11
```

## Compilation et exécution

```bash
# Configuration du build
meson setup build

# Compilation
meson compile -C build

# Exécution depuis la racine du projet (important pour les assets)
./build/kepler
```

## Structure du projet

```
Projet_Kepler/
├── src/                    # Code source C
│   ├── systeme_solaire.c   # Programme principal (affichage 3D)
│   ├── trajectoire.c/h     # Calcul des orbites
│   ├── energie.c/h         # Calculs d'énergie
│   ├── euler.c/h           # Intégrateur numérique
│   ├── vectors.h           # Opérations vectorielles
│   └── tab_generic.h       # Conteneur générique (tableaux)
├── assets/
│   └── texture/            # Textures des planètes (.png)
├── build/                  # Dossier de build (généré)
├── meson.build             # Configuration de build
└── README.md               # Ce fichier
```

## Commandes

### Navigation
- **Flèches gauche/droite** : Déplacer la caméra
- **Flèches haut/bas** : Zoom avant/arrière

### Affichage
- **ESC** : Quitter

## Fichier multimédias requis

- ✅ `assets/texture/` - Textures planétaires (présentes)

⚠️ **Important** : Exécutez le programme depuis la racine du projet pour que les chemins relatifs fonctionnent.

## Notes techniques

- **Norme C** : C23
- **Intégration numérique** : Méthode d'Euler
- **Bibliothèque graphique** : Raylib
- **Compilation conditionnelle** : Les deux versions (systeme_solaire.c et systeme2.c) peuvent être compilées

## Auteur

Projet académique - Cours IT - Programmation en C
Ollivier Thomas & Coing-Roy Marine
