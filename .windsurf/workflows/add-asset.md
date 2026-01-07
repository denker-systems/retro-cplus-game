---
description: Import and manage game assets
---

# Add Asset Workflow

> Asset pipeline for game development

## Asset Types

| Type | Format | Location |
|------|--------|----------|
| Sprites | PNG | assets/sprites/ |
| Backgrounds | PNG | assets/backgrounds/ |
| Sound Effects | WAV | assets/sounds/ |
| Music | OGG | assets/music/ |
| Data | JSON | assets/data/ |
| Fonts | TTF | assets/fonts/ |

---

## 1. Textures (PNG)

### Import
```powershell
Copy-Item "source/image.png" "assets/sprites/"
```

### Naming Convention
```
[category]_[name]_[variant].png
player_idle_01.png
enemy_goblin_walk.png
ui_button_normal.png
```

### Load in Code
```cpp
// Via TextureManager
auto* tex = TextureManager::instance().load("sprites/player.png");

// Direct SDL (not recommended)
SDL_Texture* tex = IMG_LoadTexture(renderer, "assets/sprites/player.png");
```

---

## 2. Audio

### Sound Effects (WAV)
```powershell
Copy-Item "source/sound.wav" "assets/sounds/"
```

### Music (OGG)
```powershell
Copy-Item "source/music.ogg" "assets/music/"
```

### Load in Code
```cpp
// Sound effect
auto* sfx = AudioManager::instance().loadSound("sounds/click.wav");
AudioManager::instance().playSound(sfx);

// Music
AudioManager::instance().loadMusic("music/theme.ogg");
AudioManager::instance().playMusic("theme", -1);  // Loop forever
```

---

## 3. JSON Data

### Create Data File
```powershell
New-Item "assets/data/filename.json" -Force
```

### Validate Schema
```powershell
# If using json-schema-validator
```

### Load in Code
```cpp
auto data = DataLoader::load<RoomData>("data/rooms.json");
```

---

## 4. Sprite Sheets

### Format
```json
{
  "texture": "sprites/player_sheet.png",
  "frameWidth": 32,
  "frameHeight": 32,
  "animations": {
    "idle": { "start": 0, "end": 3, "fps": 8 },
    "walk": { "start": 4, "end": 11, "fps": 12 }
  }
}
```

### Load
```cpp
auto sheet = SpriteSheet::load("sprites/player_sheet.json");
```

---

## 5. Build & Verify

// turbo
```powershell
cd build; cmake --build . --config Release
```

Assets are copied to `build/Release/assets/` automatically.

---

## Asset Guidelines

### Textures
- Power of 2 dimensions (32, 64, 128, 256)
- PNG format with transparency
- Consistent pixel art scale

### Audio
- WAV for short effects (< 2 sec)
- OGG for music and long sounds
- Normalize volume levels

### Data
- Valid JSON
- Follow schema conventions
- Use meaningful IDs

---

## Checklist

- [ ] Asset copied to correct folder
- [ ] Naming convention followed
- [ ] Format validated
- [ ] Loaded in code
- [ ] Build successful
- [ ] Asset displays/plays correctly
