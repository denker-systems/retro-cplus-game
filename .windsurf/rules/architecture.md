# Arkitektur

<overview>
C++17 SDL2-spel med modern arkitektur.
</overview>

<folder_structure>
retro-cplus-game/
  CMakeLists.txt        # Build-konfiguration
  src/
    main.cpp            # Entry point
    Game.cpp/h          # Spelloop, init, koordinering
    Renderer.cpp/h      # Textur-laddning och ritning
    Input.cpp/h         # Tangentbordshantering
    Player.cpp/h        # Spelarlogik och animation
    Room.cpp/h          # Rum, hotspots, bakgrund
  assets/
    sprites/            # PNG sprite sheets
    backgrounds/        # PNG bakgrundsbilder
    sounds/             # WAV/OGG ljudfiler
  build/                # CMake build output
</folder_structure>

<class_responsibilities>
| Klass | Ansvar |
|-------|--------|
| Game | Spelloop, SDL init, koordinering |
| Renderer | Ladda/rita texturer |
| Input | Tangentbordstillstånd |
| Player | Position, animation, rörelse |
| Room | Bakgrund, hotspots, walk area |
</class_responsibilities>

<dependencies>
- SDL2 (fönster, rendering)
- SDL2_image (PNG-laddning)
- SDL2_mixer (ljud)
</dependencies>
