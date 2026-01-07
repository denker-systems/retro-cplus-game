# Assets Folder - Example Structure

## ⚠️ Placeholder Assets

The files in this folder are **text-based placeholders** to demonstrate the asset structure.

**To use real assets:**

1. **Download free assets from:**
   - **Kenney.nl** (recommended): https://kenney.nl/assets/platformer-art-deluxe
   - **OpenGameArt.org**: https://opengameart.org/
   - **itch.io**: https://itch.io/game-assets/free

2. **Replace placeholder files** with actual PNG/WAV/OGG files

3. **Keep the same folder structure**

---

## Folder Structure

```
assets/
├── sprites/              # Character and object sprites
│   └── player/
│       ├── idle.png      # Player idle animation (4 frames, 128x32)
│       ├── walk.png      # Player walk animation (8 frames, 256x32)
│       └── jump.png      # Player jump animation (2 frames, 64x32)
│
├── backgrounds/          # Background images for parallax
│   ├── sky.png           # Sky layer (640x480)
│   ├── mountains.png     # Mountain layer (640x480)
│   └── trees.png         # Tree layer (640x480)
│
├── tilesets/            # Tileset images for TileMap
│   └── dungeon.png      # Dungeon tileset (256x96, 8x3 tiles of 32x32)
│
└── ui/                  # UI elements
    ├── button.png       # Button sprite (128x32)
    └── panel.png        # Panel background (200x150)
```

---

## File Specifications

### Sprites (Player)

**idle.png**
- Size: 128x32 pixels (4 frames × 32x32 each)
- Format: PNG with transparency
- Frames: 0, 1, 2, 3 (left to right)

**walk.png**
- Size: 256x32 pixels (8 frames × 32x32 each)
- Format: PNG with transparency
- Frames: 0-7 (left to right)

**jump.png**
- Size: 64x32 pixels (2 frames × 32x32 each)
- Format: PNG with transparency
- Frames: 0, 1 (left to right)

### Backgrounds

**sky.png, mountains.png, trees.png**
- Size: 640x480 pixels (match game resolution)
- Format: PNG or JPG
- Usage: Parallax scrolling layers

### Tilesets

**dungeon.png**
- Size: 256x96 pixels (8 columns × 3 rows)
- Tile size: 32x32 pixels
- Total tiles: 24
- Format: PNG with transparency

### UI

**button.png**
- Size: 128x32 pixels
- Format: PNG with transparency

**panel.png**
- Size: 200x150 pixels
- Format: PNG with transparency

---

## How to Use in Code

### Load Player Sprite

```cpp
auto player = std::make_unique<AnimatedSprite>("Player");
player->setTexture(TextureManager::instance().loadTexture(
    renderer, 
    "assets/sprites/player/idle.png"
));
player->setFrameSize(32, 32);  // Each frame is 32x32
player->addAnimation("idle", {0, 1, 2, 3}, 0.15f);
player->play("idle");
```

### Load Background Layers

```cpp
auto sky = std::make_unique<ParallaxLayer>("Sky");
sky->setTexture(TextureManager::instance().loadTexture(
    renderer, 
    "assets/backgrounds/sky.png"
));
sky->setParallaxFactor(0.1f, 0.1f);
sky->setZIndex(-300);

auto mountains = std::make_unique<ParallaxLayer>("Mountains");
mountains->setTexture(TextureManager::instance().loadTexture(
    renderer, 
    "assets/backgrounds/mountains.png"
));
mountains->setParallaxFactor(0.3f, 0.2f);
mountains->setZIndex(-200);
```

### Load Tileset

```cpp
auto tilemap = std::make_unique<TileMapLayer>("Ground", 20, 15, 32);
tilemap->setTexture(TextureManager::instance().loadTexture(
    renderer, 
    "assets/tilesets/dungeon.png"
));
tilemap->setTile(5, 10, 1);  // Place tile #1 at grid (5, 10)
```

---

## Download Real Assets

### Quick Start: Kenney Platformer Pack

1. **Download:** https://kenney.nl/assets/platformer-art-deluxe
2. **Extract** the ZIP file
3. **Copy files:**
   ```bash
   # Player sprites
   cp kenney_platformer/PNG/Players/p1_stand.png assets/sprites/player/idle.png
   cp kenney_platformer/PNG/Players/p1_walk/*.png assets/sprites/player/
   
   # Backgrounds
   cp kenney_platformer/PNG/Backgrounds/*.png assets/backgrounds/
   
   # Tiles
   cp kenney_platformer/PNG/Tiles/*.png assets/tilesets/
   ```

4. **Update code** to use actual file names

---

## License

**Placeholder files:** Public domain (text files)

**When using real assets:**
- Check the license of each asset pack
- CC0 assets require no attribution
- CC-BY assets require attribution
- Keep license info in this README

---

**See also:**
- [Asset Guide](../docs/guides/AssetGuide.md) - File types and structure
- [Free Assets](../docs/guides/FreeAssets.md) - Download links
