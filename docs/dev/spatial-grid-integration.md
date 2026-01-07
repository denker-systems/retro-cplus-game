# Spatial Grid Integration Plan

## Princip: Utöka, inte ersätta

Vi bygger på befintlig kod genom att **lägga till** grid-data, inte ändra befintlig funktionalitet.

---

## Befintlig Kod att Bygga På

### 1. WorldContainer (behåll som är)
```cpp
// src/engine/world/WorldContainer.h - INGEN ÄNDRING
class WorldContainer {
    std::string m_name;
    std::vector<std::unique_ptr<ActorObjectExtended>> m_actors;
};
```

### 2. Level (utöka med grid)
```cpp
// src/engine/world/Level.h - LÄGG TILL
#include "GridTypes.h"

class Level : public WorldContainer {
    // BEFINTLIGT (behåll):
    std::vector<std::unique_ptr<Scene>> m_scenes;
    Scene* m_activeScene;
    
    // NYTT (lägg till):
    GridPosition m_gridPosition;                              // Position i World
    std::map<std::string, GridPosition> m_roomPositions;      // Room positions
    std::vector<GridConnection> m_connections;                // Level-to-level
};
```

### 3. Scene/Room (utöka med grid)
```cpp
// src/engine/Scene.h - LÄGG TILL
#include "engine/world/GridTypes.h"

class Scene {
    // BEFINTLIGT (behåll):
    std::string m_id;
    std::vector<RoomLayer> m_layers;        // Redan finns!
    WalkArea m_walkArea;                    // Redan finns!
    std::vector<Hotspot> m_hotspots;        // Redan finns!
    
    // NYTT (lägg till):
    GridPosition m_gridPosition;            // Position i Level
    std::vector<GridConnection> m_exits;    // Ersätter/kompletterar hotspot exits
    CameraConfig m_cameraConfig;            // Kamera-inställningar
};
```

### 4. RoomLayer (redan implementerat!)
```cpp
// src/engine/Hotspot.h - REDAN FINNS
struct RoomLayer {
    SDL_Texture* texture;
    int zIndex;
    float parallaxX, parallaxY;
    float opacity;
};
```

---

## Integration Steg-för-Steg

### Steg 1: Lägg till GridPosition i Scene (minimal ändring)

**Fil:** `src/engine/Scene.h`
```cpp
// Lägg till include
#include "engine/world/GridTypes.h"

// Lägg till members (i private:)
engine::GridPosition m_gridPosition;
engine::CameraConfig m_cameraConfig;

// Lägg till getters/setters
const engine::GridPosition& getGridPosition() const { return m_gridPosition; }
void setGridPosition(const engine::GridPosition& pos) { m_gridPosition = pos; }
```

### Steg 2: Mappa befintliga Hotspot-exits till GridConnection

**Befintlig kod:**
```cpp
// I Scene/Room - redan finns!
for (const auto& hs : m_hotspots) {
    if (hs.type == HotspotType::Exit) {
        // hs.targetRoom finns redan!
    }
}
```

**Integration:**
```cpp
// Helper för att konvertera
GridConnection hotspotToConnection(const Hotspot& hs) {
    GridConnection conn;
    conn.targetId = hs.targetRoom;
    conn.exitPoint = hs.id;
    conn.triggerArea = hs.rect;
    conn.transitionType = "fade";  // Default
    return conn;
}
```

### Steg 3: Uppdatera JSON-loading (bakåtkompatibelt)

**Befintlig rooms.json:**
```json
{
  "id": "tavern",
  "background": "assets/backgrounds/tavern.png",
  "hotspots": [...]
}
```

**Utökad (optional fields):**
```json
{
  "id": "tavern",
  "background": "assets/backgrounds/tavern.png",
  "hotspots": [...],
  "gridPosition": { "x": 0, "y": 0, "width": 640, "height": 400 },
  "camera": { "zoom": 1.0, "followPlayer": true }
}
```

**Loader-kod:**
```cpp
// I GameDataLoader - lägg till (optional)
if (data.contains("gridPosition")) {
    scene->setGridPosition({
        data["gridPosition"]["x"],
        data["gridPosition"]["y"],
        data["gridPosition"].value("width", 640),
        data["gridPosition"].value("height", 400)
    });
}
// Om saknas → default (0,0,640,400)
```

---

## Vad som INTE ändras

| Komponent | Status |
|-----------|--------|
| `WorldContainer` | Oförändrad |
| `ActorObjectExtended` | Oförändrad |
| `RoomLayer` | Oförändrad |
| `Hotspot` | Oförändrad |
| `WalkArea` | Oförändrad |
| `SceneManager` | Minimal ändring (grid-navigation) |
| `PlayState` | Oförändrad |
| Befintliga JSON-filer | Fungerar utan ändringar |

---

## Editor-integration

### WorldViewPanel (utöka)
- Visa levels som rektanglar i grid
- Drag-and-drop för att flytta levels
- Dubbelklick för att öppna level

### LevelViewPanel (utöka)
- Visa rooms som rektanglar med varierande storlek
- Visa connections som linjer
- Drag för att ändra storlek

### ViewportPanel (behåll + lägg till)
- Befintlig room-rendering fungerar
- Lägg till kamera-gizmo för bounds
- Visa grid overlay (optional)

---

## Testplan

1. ✅ Bygga med nya headers (ingen breaking change)
2. ✅ Ladda befintliga rooms.json (bakåtkompatibelt)
3. ✅ Spelet fungerar utan ändringar
4. ⏳ Editor visar grid-data
5. ⏳ Editera grid-positions
6. ⏳ Spara/ladda grid-data

---

## Nästa Steg

1. [ ] Lägg till `#include "engine/world/GridTypes.h"` i Scene.h
2. [ ] Lägg till GridPosition member i Scene
3. [ ] Uppdatera GameDataLoader med optional grid-loading
4. [ ] Testa att befintligt spel fortfarande fungerar
5. [ ] Utöka editor-paneler
