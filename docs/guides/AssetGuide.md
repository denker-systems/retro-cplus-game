# Asset Guide - File Types and Structure

## Supported File Types

### Images (SDL2_image)
- **PNG** (recommended) - Transparency support, lossless
- **JPG/JPEG** - Photos, no transparency
- **BMP** - Simple format, large files
- **GIF** - Animations (we use sprite sheets instead)

**Best choice:** PNG for all game graphics

### Audio (SDL2_mixer)
- **WAV** - Sound effects (uncompressed, good quality)
- **OGG** - Music (compressed, smaller files)
- **MP3** - Music (compressed, widely supported)

**Best choice:** 
- WAV for short sound effects
- OGG for background music

### Fonts (SDL2_ttf)
- **TTF** - TrueType fonts
- **OTF** - OpenType fonts

---

## Asset Folder Structure

```
assets/
├── sprites/              # Character and object sprites
│   ├── player/
│   │   ├── idle.png      # Idle animation sprite sheet
│   │   ├── walk.png      # Walking animation sprite sheet
│   │   └── jump.png      # Jumping animation sprite sheet
│   ├── enemies/
│   │   ├── slime.png
│   │   └── bat.png
│   └── items/
│       ├── coin.png
│       └── key.png
│
├── backgrounds/          # Background images
│   ├── sky.png           # Parallax layer 1 (furthest)
│   ├── mountains.png     # Parallax layer 2
│   ├── trees.png         # Parallax layer 3 (closest)
│   └── ground.png        # Static background
│
├── tilesets/            # Tileset images for TileMap
│   ├── dungeon.png      # 32x32 tiles in grid
│   ├── grass.png        # Grass tiles
│   └── castle.png       # Castle tiles
│
├── ui/                  # UI elements
│   ├── button.png
│   ├── panel.png
│   ├── health_bar.png
│   └── icons/
│       ├── heart.png
│       └── star.png
│
├── sounds/              # Sound effects (.wav)
│   ├── jump.wav
│   ├── coin.wav
│   ├── hit.wav
│   └── explosion.wav
│
├── music/               # Background music (.ogg)
│   ├── menu.ogg
│   ├── level1.ogg
│   └── boss.ogg
│
└── fonts/               # Font files (.ttf)
    ├── main.ttf
    └── title.ttf
```

---

## Sprite Sheet Format

Sprite sheets contain multiple frames in a grid:

```
┌────┬────┬────┬────┐
│ 0  │ 1  │ 2  │ 3  │  Frame 0-3: Idle animation
├────┼────┼────┼────┤
│ 4  │ 5  │ 6  │ 7  │  Frame 4-7: Walk animation
├────┼────┼────┼────┤
│ 8  │ 9  │ 10 │ 11 │  Frame 8-11: Jump animation
└────┴────┴────┴────┘
```

**Example:** player_idle.png
- Image size: 128x32 (4 frames × 32x32 each)
- Frame size: 32x32
- Frames: 0, 1, 2, 3

---

## Tileset Format

Tilesets are grids of tiles for TileMap:

```
┌──┬──┬──┬──┬──┬──┬──┬──┐
│ 1│ 2│ 3│ 4│ 5│ 6│ 7│ 8│
├──┼──┼──┼──┼──┼──┼──┼──┤
│ 9│10│11│12│13│14│15│16│
├──┼──┼──┼──┼──┼──┼──┼──┤
│17│18│19│20│21│22│23│24│
└──┴──┴──┴──┴──┴──┴──┴──┘
```

**Example:** dungeon_tileset.png
- Image size: 256x96 (8×3 tiles × 32x32 each)
- Tile size: 32x32
- Total tiles: 24

---

## Loading Assets in Code

### Load Texture
```cpp
SDL_Texture* texture = TextureManager::instance().loadTexture(
    renderer, 
    "assets/sprites/player/idle.png"
);
```

### Load Sprite with Animation
```cpp
auto player = std::make_unique<AnimatedSprite>("Player");
player->setTexture(playerTexture);
player->setFrameSize(32, 32);  // Each frame is 32x32
player->addAnimation("idle", {0, 1, 2, 3}, 0.15f);  // Frames 0-3, 0.15s per frame
player->play("idle");
```

### Load Tileset
```cpp
auto tilemap = std::make_unique<TileMapLayer>("Ground", 20, 15, 32);
tilemap->setTexture(tilesetTexture);
tilemap->setTile(5, 10, 1);  // Place tile #1 at grid position (5, 10)
```

### Load Sound
```cpp
AudioManager::instance().loadSound("jump", "assets/sounds/jump.wav");
AudioManager::instance().playSound("jump");
```

### Load Music
```cpp
AudioManager::instance().loadMusic("level1", "assets/music/level1.ogg");
AudioManager::instance().playMusic("level1");
```

---

## Asset Requirements

### Sprites
- **Size:** Power of 2 recommended (32x32, 64x64, 128x128)
- **Format:** PNG with transparency
- **Color:** 32-bit RGBA

### Backgrounds
- **Size:** Match screen resolution or larger for parallax
- **Format:** PNG or JPG
- **Parallax:** Multiple layers at different sizes

### Tilesets
- **Tile size:** 16x16, 32x32, or 64x64 (consistent)
- **Grid:** Evenly spaced, no gaps
- **Format:** PNG with transparency

### Sounds
- **Format:** WAV, 16-bit, 44.1kHz
- **Length:** < 5 seconds for effects
- **Size:** < 1MB per file

### Music
- **Format:** OGG Vorbis
- **Quality:** 128-192 kbps
- **Loop:** Seamless loop points

---

## Free Asset Resources

### Sprite & Tileset Sites
- **OpenGameArt.org** - CC0 and CC-BY licensed
- **itch.io** - Many free asset packs
- **Kenney.nl** - Huge collection of CC0 assets
- **CraftPix.net** - Free game assets

### Sound Effects
- **Freesound.org** - CC licensed sounds
- **OpenGameArt.org** - Game sound effects
- **Kenney.nl** - UI sounds and effects

### Music
- **OpenGameArt.org** - Game music
- **Incompetech.com** - Royalty-free music
- **FreePD.com** - Public domain music

### Fonts
- **Google Fonts** - Free fonts
- **DaFont.com** - Pixel fonts for retro games
- **FontLibrary.org** - Open source fonts

---

## Quick Start Example

### 1. Download Assets

Create these folders:
```bash
mkdir -p assets/sprites/player
mkdir -p assets/backgrounds
mkdir -p assets/tilesets
mkdir -p assets/sounds
mkdir -p assets/music
```

### 2. Get Free Assets

**Kenney.nl Platformer Pack:**
- Download: https://kenney.nl/assets/platformer-art-deluxe
- Extract to `assets/`

### 3. Use in Code

```cpp
// Load player sprite
auto player = std::make_unique<AnimatedSprite>("Player");
player->setTexture(TextureManager::instance().loadTexture(
    renderer, "assets/sprites/player/p1_walk.png"
));
player->setFrameSize(72, 97);
player->addAnimation("walk", {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, 0.1f);
player->play("walk");

// Load background
auto bg = std::make_unique<Sprite>("Background");
bg->setTexture(TextureManager::instance().loadTexture(
    renderer, "assets/backgrounds/colored_grass.png"
));

// Load tileset
auto ground = std::make_unique<TileMapLayer>("Ground", 30, 20, 70);
ground->setTexture(TextureManager::instance().loadTexture(
    renderer, "assets/tilesets/tiles_spritesheet.png"
));
```

---

## Tips

1. **Keep it organized** - Use subfolders
2. **Name consistently** - `player_idle.png`, `player_walk.png`
3. **Optimize sizes** - Don't use 4K images for 32x32 sprites
4. **Test early** - Load one asset first, then add more
5. **Check licenses** - CC0 is safest for commercial use

---

**Ready to add assets to your game!** 🎨
