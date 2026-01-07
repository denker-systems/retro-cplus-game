---
description: Add and manage game assets (textures, sounds, data)
---

# Add Asset Workflow

> Hantera game assets

## Textures (PNG)

### 1. Place File
```powershell
# Sprites  assets/sprites/
# Backgrounds  assets/backgrounds/
Copy-Item "path\to\image.png" "assets\sprites\"
```

### 2. Load in Code
```cpp
// Via TextureManager (recommended)
auto* texture = TextureManager::instance().load("sprites/player.png");

// Direct SDL (temporary)
SDL_Texture* tex = IMG_LoadTexture(renderer, "assets/sprites/player.png");
```

---

## Audio (WAV/OGG)

### 1. Place File
```powershell
Copy-Item "path\to\sound.wav" "assets/sounds/"
Copy-Item "path\to\music.ogg" "assets/music/"
```

### 2. Load in Code
```cpp
// Sound effect
Mix_Chunk* sfx = Mix_LoadWAV("assets/sounds/click.wav");
Mix_PlayChannel(-1, sfx, 0);

// Music
Mix_Music* music = Mix_LoadMUS("assets/music/theme.ogg");
Mix_PlayMusic(music, -1);  // -1 = loop forever
```

---

## JSON Data

### 1. Create File
```powershell
New-Item "assets/data/rooms.json" -Force
```

### 2. Format
```json
{
  "rooms": [
    {
      "id": "tavern",
      "name": "The Rusty Anchor",
      "background": "backgrounds/tavern.png"
    }
  ]
}
```

---

## Rebuild After Changes
// turbo
```powershell
cd build; cmake --build . --config Release
```

Assets kopieras automatiskt till build/Release/assets/ via CMake.
