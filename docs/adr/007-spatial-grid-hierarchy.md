# ADR 007: Spatial Grid Hierarchy (Super Mario World-style)

## Status
**Implemented** - 2026-01-06

## Context
Vi behöver en editor-vänlig arkitektur för att bygga levels och rooms visuellt, liknande Unreal Engine eller Super Mario World. Nuvarande system har World → Level → Scene hierarki men saknar:
- Spatial positionering på alla nivåer
- Varierande room-storlekar
- Visuell grid-baserad editing
- Kamera-konfiguration och parallax

## Decision
Utöka befintlig WorldContainer basklass med spatial grid-data som ärvs av alla nivåer.

### Hierarki (Inheritance-based)

```
WorldContainer (abstract base with GridPosition)
├── World (grid of Levels)
│   └── Level (grid of Scenes)
│       └── Scene (grid of Actors)
```

### Implementation

**WorldContainer.h** - Basklassen med all grid-funktionalitet:
```cpp
class WorldContainer {
protected:
    GridPosition m_gridPosition = {0, 0, 640, 400};
    
public:
    // Alla barn ärver dessa automatiskt
    const GridPosition& getGridPosition() const;
    void setGridPosition(const GridPosition& pos);
    void setGridPosition(int x, int y, int w, int h);
    int getGridX() const;
    int getGridY() const;
    int getWidth() const;
    int getHeight() const;
    void setSize(int w, int h);
};
```

**GridTypes.h** - Gemensamma strukturer:
```cpp
struct GridPosition {
    int gridX = 0;
    int gridY = 0;
    int pixelWidth = 640;
    int pixelHeight = 400;
};

struct CameraConfig {
    float zoom = 1.0f;
    bool followPlayer = true;
    float smoothing = 0.1f;
    // ...bounds, offset, shake
};

struct GridConnection {
    std::string targetId;
    std::string exitPoint;
    std::string entryPoint;
    std::string transitionType;
};
```

### Fördelar med Inheritance

1. **DRY** - Grid-kod på ETT ställe (WorldContainer)
2. **Konsistens** - Samma API för World, Level och Scene
3. **Polymorfism** - Kan behandla alla containers likadant
4. **Utbyggbar** - Lägg till funktioner i basklassen, alla ärver

### Editor Integration

ViewportPanel har tre view modes för Level:
- **Spatial View** - Grid-baserad vy med pan/zoom
- **Grid View** - Kort-layout
- **List View** - Enkel lista

## Consequences

### Positiva
- En kodbas för spatial grid (WorldContainer)
- Automatiskt ärvd av alla nivåer
- Enkel att utöka med nya funktioner
- Visuell level-building i editor

### Negativa  
- WorldContainer blir mer komplex
- Alla containers får grid-data även om de inte behöver det

## Files Changed
- `src/engine/world/WorldContainer.h` - Grid-funktionalitet
- `src/engine/world/GridTypes.h` - Structs
- `src/editor/panels/ViewportPanel.cpp` - Spatial View
