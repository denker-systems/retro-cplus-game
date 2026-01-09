# Retro Engine Icon Setup Guide

## 📋 Steg-för-steg Guide

### Steg 1: Exportera PNG-filer från din design

Exportera följande storlekar från din Retro Engine-ikon:

| Storlek | Filnamn | Användning |
|---------|---------|------------|
| 256x256 | `retro_engine_256.png` | Vista+, High DPI |
| 128x128 | `retro_engine_128.png` | Large icons |
| 64x64 | `retro_engine_64.png` | Medium icons |
| 48x48 | `retro_engine_48.png` | Standard desktop |
| 32x32 | `retro_engine_32.png` | Taskbar |
| 24x24 | `retro_engine_24.png` | Small icons |
| 16x16 | `retro_engine_16.png` | Tiny icons |

**Tips:**
- Använd transparent bakgrund (PNG med alpha channel)
- Behåll pixel art-stilen konsekvent
- För 16x16 och 24x24 kan du behöva justera pixlar manuellt för bättre klarhet

### Steg 2: Placera PNG-filer

Kopiera alla exporterade PNG-filer till:
```
assets/icons/
```

### Steg 3: Skapa .ico-fil

Du har tre alternativ:

#### Alternativ A: Online Tool (Enklast)
1. Gå till https://convertio.co/png-ico/ eller https://www.icoconverter.com/
2. Ladda upp alla 7 PNG-filer
3. Välj "Combine into one icon" eller "Multi-size icon"
4. Ladda ner som `retro_engine.ico`
5. Placera i `assets/icons/retro_engine.ico`

#### Alternativ B: ImageMagick (Command Line)
```bash
magick convert retro_engine_256.png retro_engine_128.png retro_engine_64.png retro_engine_48.png retro_engine_32.png retro_engine_24.png retro_engine_16.png retro_engine.ico
```

#### Alternativ C: GIMP
1. Öppna `retro_engine_256.png` i GIMP
2. Lägg till andra storlekar som layers (File → Open as Layers)
3. Exportera som .ico (File → Export As → välj .ico)
4. I export-dialogen:
   - Kryssa i "Compressed (PNG)" för storlekar ≥ 48x48
   - Välj alla layers
5. Spara som `retro_engine.ico`

### Steg 4: Bygg projektet

```powershell
cd build
cmake --build . --config Release
```

CMake kommer automatiskt att:
- Inkludera `RetroEditor.rc` och `RetroGame.rc` (Windows resource files)
- Bädda in `retro_engine.ico` i .exe-filerna
- Sätta ikonen för både .exe-filen och window

### Steg 5: Verifiera

Efter build:
1. **Kontrollera .exe-fil ikon:**
   - Högerklicka på `build/Release/RetroEditor.exe`
   - Ikonen ska synas i File Explorer

2. **Kontrollera window ikon:**
   - Kör `RetroEditor.exe`
   - Ikonen ska synas i:
     - Fönstrets title bar
     - Taskbar
     - Alt+Tab switcher

## 🔧 Teknisk Implementation

### Windows Resource Files

**RetroEditor.rc:**
```rc
IDI_ICON1 ICON "..\\..\\assets\\icons\\retro_engine.ico"
```

**RetroGame.rc:**
```rc
IDI_ICON1 ICON "..\\..\\assets\\icons\\retro_engine.ico"
```

### CMake Integration

```cmake
# Windows icon resources
if(WIN32)
    list(APPEND EDITOR_SOURCES src/editor/RetroEditor.rc)
    list(APPEND GAME_SOURCES src/game/RetroGame.rc)
endif()
```

### Runtime Icon Setting (SDL Window)

För att sätta window icon runtime (om .rc inte fungerar):

```cpp
#include "engine/utils/IconLoader.h"

// I din window creation kod:
SDL_Window* window = SDL_CreateWindow("Retro Engine", ...);
engine::IconLoader::SetWindowIconAuto(window);
```

## 📁 Filstruktur

```
retro-cplus-game/
├── assets/
│   └── icons/
│       ├── retro_engine.ico          # Multi-size .ico (REQUIRED)
│       ├── retro_engine_256.png      # 256x256 PNG
│       ├── retro_engine_128.png      # 128x128 PNG
│       ├── retro_engine_64.png       # 64x64 PNG
│       ├── retro_engine_48.png       # 48x48 PNG
│       ├── retro_engine_32.png       # 32x32 PNG
│       ├── retro_engine_24.png       # 24x24 PNG
│       ├── retro_engine_16.png       # 16x16 PNG
│       └── README.md
├── src/
│   ├── editor/
│   │   ├── RetroEditor.rc            # Windows resource file
│   │   └── resource.h
│   ├── game/
│   │   ├── RetroGame.rc              # Windows resource file
│   │   └── resource.h
│   └── engine/
│       └── utils/
│           ├── IconLoader.h          # Runtime icon loader
│           └── IconLoader.cpp
└── CMakeLists.txt
```

## ✅ Checklist

- [ ] Exportera alla 7 PNG-storlekar från din design
- [ ] Placera PNG-filer i `assets/icons/`
- [ ] Skapa `retro_engine.ico` med alla storlekar
- [ ] Placera .ico-fil i `assets/icons/`
- [ ] Bygg projektet (`cmake --build . --config Release`)
- [ ] Verifiera ikon i File Explorer
- [ ] Verifiera ikon i window när programmet körs

## 🐛 Troubleshooting

### Problem: Ikonen syns inte i .exe-filen
**Lösning:** 
- Kontrollera att `retro_engine.ico` finns i `assets/icons/`
- Kontrollera att sökvägen i .rc-filen är korrekt
- Rebuild projektet helt: `cmake --build . --config Release --clean-first`

### Problem: Ikonen syns inte i window
**Lösning:**
- Använd `IconLoader::SetWindowIconAuto()` för runtime icon setting
- Kontrollera att PNG-filerna finns i `assets/icons/`

### Problem: .ico-filen är för stor
**Lösning:**
- Använd PNG-komprimering för storlekar ≥ 48x48
- För 16x16 och 24x24, använd BMP (okomprimerat) för bättre kompatibilitet

## 📚 Referenser

- [Microsoft Icon Guidelines](https://docs.microsoft.com/en-us/windows/win32/uxguide/vis-icons)
- [SDL_SetWindowIcon Documentation](https://wiki.libsdl.org/SDL_SetWindowIcon)
- [ICO File Format](https://en.wikipedia.org/wiki/ICO_(file_format))
