# Asset Generator

Python scripts to generate placeholder PNG assets for development and testing.

## Requirements

```bash
pip install Pillow
```

## Quick Start

**Generate all placeholder assets:**
```bash
cd generator
python generate_all.py
```

This creates placeholder PNG files in `../assets/` with proper dimensions and transparency.

## Individual Generators

### Sprites
```bash
python scripts/generate_sprites.py
```
Creates: `assets/sprites/player/idle.png`, `walk.png`, `jump.png`

### Backgrounds
```bash
python scripts/generate_backgrounds.py
```
Creates: `assets/backgrounds/sky.png`, `mountains.png`, `trees.png`

### Tilesets
```bash
python scripts/generate_tilesets.py
```
Creates: `assets/tilesets/dungeon.png`

### UI Elements
```bash
python scripts/generate_ui.py
```
Creates: `assets/ui/button.png`, `panel.png`

## Folder Structure

```
generator/
├── README.md              # This file
├── generate_all.py        # Main script - generates everything
├── requirements.txt       # Python dependencies
└── scripts/
    ├── generate_sprites.py      # Player sprite sheets
    ├── generate_backgrounds.py  # Parallax backgrounds
    ├── generate_tilesets.py     # Tilemap tilesets
    └── generate_ui.py           # UI elements
```

## What Gets Generated

### Sprites (32x32 frames)
- **idle.png** - 128x32 (4 frames)
- **walk.png** - 256x32 (8 frames)
- **jump.png** - 64x32 (2 frames)

### Backgrounds (640x480)
- **sky.png** - Blue gradient
- **mountains.png** - Mountain silhouettes
- **trees.png** - Tree silhouettes

### Tilesets (32x32 tiles)
- **dungeon.png** - 256x96 (8x3 grid = 24 tiles)

### UI Elements
- **button.png** - 128x32
- **panel.png** - 200x150

## Customization

Edit the scripts in `scripts/` to change:
- Colors
- Sizes
- Text labels
- Frame counts

## Replace with Real Assets

These are just placeholders! Download real assets from:
- **Kenney.nl**: https://kenney.nl/assets/platformer-art-deluxe
- **OpenGameArt.org**: https://opengameart.org/
- **itch.io**: https://itch.io/game-assets/free

Then replace the generated PNG files.
