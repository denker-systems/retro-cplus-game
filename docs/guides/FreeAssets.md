# Free 2D Game Assets - Download Links

## Best Sources for CC0/Free Assets

### 1. Kenney.nl ⭐ HIGHLY RECOMMENDED
**URL:** https://kenney.nl/assets

**Why it's best:**
- 100% CC0 (Public Domain) - Use anywhere, no attribution needed
- Consistent art style across all packs
- High quality, professional assets
- Huge collection (40,000+ assets)

**Download These First:**

#### Platformer Pack
- **URL:** https://kenney.nl/assets/platformer-art-deluxe
- **Contains:** Characters, tiles, backgrounds, items
- **Size:** ~10MB
- **Perfect for:** 2D platformer games

#### Pixel Platformer
- **URL:** https://kenney.nl/assets/pixel-platformer
- **Contains:** Retro pixel art characters and tiles
- **Size:** ~2MB
- **Perfect for:** Retro-style games

#### UI Pack
- **URL:** https://kenney.nl/assets/ui-pack
- **Contains:** Buttons, panels, icons
- **Size:** ~5MB
- **Perfect for:** Game menus and UI

---

### 2. OpenGameArt.org
**URL:** https://opengameart.org/

**Best Collections:**

#### Liberated Pixel Cup (LPC)
- **URL:** https://opengameart.org/content/liberated-pixel-cup-0
- **Contains:** Character sprites, tilesets, items
- **License:** CC-BY-SA 3.0 (requires attribution)
- **Style:** Top-down RPG style

#### Platformer Art
- **Search:** https://opengameart.org/art-search-advanced?keys=platformer
- **Filter by:** CC0 or CC-BY
- **Contains:** Various platformer assets

---

### 3. itch.io Free Assets
**URL:** https://itch.io/game-assets/free

**Top Picks:**

#### Pixel Adventure
- **URL:** https://pixelfrog-assets.itch.io/pixel-adventure-1
- **Contains:** Character animations, tiles, backgrounds
- **License:** Free for commercial use
- **Style:** Pixel art platformer

#### Brackeys Platformer Bundle
- **URL:** https://brackeysgames.itch.io/brackeys-platformer-bundle
- **Contains:** Complete platformer asset pack
- **License:** Free
- **Style:** Clean, modern pixel art

---

### 4. CraftPix Free Section
**URL:** https://craftpix.net/freebies/

**Free Packs:**
- 2D character sprites
- GUI elements
- Tilesets
- Backgrounds

**Note:** Check individual licenses - some require attribution

---

## Quick Start: Download & Setup

### Step 1: Download Kenney Platformer Pack

1. Go to: https://kenney.nl/assets/platformer-art-deluxe
2. Click "Download" (no signup needed!)
3. Extract the ZIP file

### Step 2: Organize Assets

```bash
# Create folders
mkdir -p assets/sprites/player
mkdir -p assets/backgrounds
mkdir -p assets/tilesets
mkdir -p assets/ui

# Copy from Kenney pack
cp kenney_platformer/PNG/Players/* assets/sprites/player/
cp kenney_platformer/PNG/Backgrounds/* assets/backgrounds/
cp kenney_platformer/PNG/Tiles/* assets/tilesets/
cp kenney_platformer/PNG/HUD/* assets/ui/
```

### Step 3: Test in Code

```cpp
// Load player sprite
auto player = std::make_unique<Sprite>("Player");
player->setTexture(TextureManager::instance().loadTexture(
    renderer, 
    "assets/sprites/player/p1_stand.png"
));
player->setPosition(100, 300);

// Load background
auto bg = std::make_unique<Sprite>("Background");
bg->setTexture(TextureManager::instance().loadTexture(
    renderer, 
    "assets/backgrounds/colored_grass.png"
));
bg->setZIndex(-100);
```

---

## Recommended Asset Packs by Type

### Characters (Animated)

| Pack | URL | License | Style |
|------|-----|---------|-------|
| Kenney Platformer Characters | https://kenney.nl/assets/platformer-characters-1 | CC0 | Modern pixel |
| Pixel Adventure | https://pixelfrog-assets.itch.io/pixel-adventure-1 | Free | Pixel art |
| Fantasy Knight | https://itch.io/game-assets/tag-knight | Free | Pixel RPG |

### Tilesets

| Pack | URL | License | Style |
|------|-----|---------|-------|
| Kenney Abstract Platformer | https://kenney.nl/assets/abstract-platformer | CC0 | Abstract |
| Kenney Pixel Platformer | https://kenney.nl/assets/pixel-platformer | CC0 | Retro pixel |
| Mossy Cavern | https://adamatomic.itch.io/cavernas | Free | Cave theme |

### Backgrounds

| Pack | URL | License | Style |
|------|-----|---------|-------|
| Kenney Backgrounds | https://kenney.nl/assets/background-elements-redux | CC0 | Various |
| Parallax Backgrounds | https://opengameart.org/content/parallax-scrolling-backgrounds | CC0 | Layered |

### UI Elements

| Pack | URL | License | Style |
|------|-----|---------|-------|
| Kenney UI Pack | https://kenney.nl/assets/ui-pack | CC0 | Clean modern |
| Kenney UI Pack Space | https://kenney.nl/assets/ui-pack-space-expansion | CC0 | Sci-fi theme |

### Sound Effects

| Source | URL | License | Type |
|--------|-----|---------|------|
| Kenney Digital Audio | https://kenney.nl/assets/digital-audio | CC0 | Game SFX |
| Freesound.org | https://freesound.org/ | Various | All types |
| OpenGameArt Sounds | https://opengameart.org/art-search-advanced?keys=&field_art_type_tid%5B%5D=13 | Various | Game audio |

### Music

| Source | URL | License | Type |
|--------|-----|---------|------|
| Incompetech | https://incompetech.com/music/royalty-free/ | CC-BY | Various genres |
| OpenGameArt Music | https://opengameart.org/art-search-advanced?keys=&field_art_type_tid%5B%5D=12 | Various | Game music |

---

## Example: Complete Game with Free Assets

### Assets Needed

1. **Player sprite** - Kenney p1_walk.png (11 frames)
2. **Background** - Kenney colored_grass.png
3. **Tileset** - Kenney tiles_spritesheet.png
4. **UI** - Kenney button.png
5. **Sound** - Kenney jump.wav
6. **Music** - Incompetech track

### Download Links

```
Player: https://kenney.nl/assets/platformer-characters-1
Background: https://kenney.nl/assets/background-elements-redux  
Tileset: https://kenney.nl/assets/abstract-platformer
UI: https://kenney.nl/assets/ui-pack
Sounds: https://kenney.nl/assets/digital-audio
Music: https://incompetech.com/music/royalty-free/music.html
```

### Code Example

```cpp
// Scene setup with free assets
auto scene = std::make_unique<Scene>("Level1");

// Background (Kenney)
auto bg = std::make_unique<Sprite>("Sky");
bg->setTexture(loadTexture("assets/backgrounds/colored_grass.png"));
bg->setZIndex(-100);
scene->addChild(std::move(bg));

// Tilemap (Kenney tileset)
auto ground = std::make_unique<TileMapLayer>("Ground", 30, 20, 70);
ground->setTexture(loadTexture("assets/tilesets/tiles_spritesheet.png"));
ground->setZIndex(-50);
scene->addChild(std::move(ground));

// Player (Kenney character)
auto player = std::make_unique<AnimatedSprite>("Player");
player->setTexture(loadTexture("assets/sprites/player/p1_walk.png"));
player->setFrameSize(72, 97);
player->addAnimation("walk", {0,1,2,3,4,5,6,7,8,9,10}, 0.1f);
player->play("walk");
player->setZIndex(0);
scene->addChild(std::move(player));

// Sound (Kenney)
AudioManager::instance().loadSound("jump", "assets/sounds/jump.wav");
AudioManager::instance().playSound("jump");

// Music (Incompetech)
AudioManager::instance().loadMusic("level1", "assets/music/level1.ogg");
AudioManager::instance().playMusic("level1");
```

---

## License Quick Reference

| License | Can Use Commercially? | Need Attribution? | Can Modify? |
|---------|----------------------|-------------------|-------------|
| CC0 | ✅ Yes | ❌ No | ✅ Yes |
| CC-BY | ✅ Yes | ✅ Yes | ✅ Yes |
| CC-BY-SA | ✅ Yes | ✅ Yes | ✅ Yes (share-alike) |
| Public Domain | ✅ Yes | ❌ No | ✅ Yes |

**Safest for commercial games:** CC0 or Public Domain

---

## Next Steps

1. **Download Kenney Platformer Pack** (start here!)
2. **Extract to `assets/` folder**
3. **Test loading one sprite** in your game
4. **Add more assets** as needed
5. **Check licenses** before releasing your game

---

**You now have access to thousands of free, high-quality game assets!** 🎨
