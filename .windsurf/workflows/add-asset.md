---
description: Laddar och hanterar game assets (texturer, ljud, data)
---

# Add Asset Workflow

## Texturer (PNG)

### 1. Placera filen
`powershell
# Sprites går i assets/sprites/
# Bakgrunder i assets/backgrounds/
Copy-Item "path\to\image.png" "assets\sprites\"
`

### 2. Ladda i kod (när TextureManager finns)
`cpp
auto texture = TextureManager::instance().load("sprites/player.png");
`

### Temporärt (utan TextureManager)
`cpp
SDL_Texture* tex = IMG_LoadTexture(renderer, "assets/sprites/player.png");
`

## Ljud (WAV/OGG)

### 1. Placera filen
`powershell
Copy-Item "path\to\sound.wav" "assets\sounds\"
Copy-Item "path\to\music.ogg" "assets\sounds\"
`

### 2. Ladda i kod
`cpp
// Sound effect
Mix_Chunk* sfx = Mix_LoadWAV("assets/sounds/click.wav");
Mix_PlayChannel(-1, sfx, 0);

// Music
Mix_Music* music = Mix_LoadMUS("assets/sounds/background.ogg");
Mix_PlayMusic(music, -1);
`

## JSON Data

### 1. Skapa datafil
`powershell
New-Item "assets/data/rooms.json" -Force
`

### 2. Format
`json
{
  "rooms": [
    {
      "id": "tavern",
      "name": "The Rusty Anchor",
      "background": "backgrounds/tavern.png"
    }
  ]
}
`

## Rebuild efter asset-ändringar
// turbo
`powershell
cd build; cmake --build . --config Release
`

Assets kopieras automatiskt till build/Release/assets/ via CMake.
