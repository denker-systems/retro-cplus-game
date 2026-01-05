# Tiled Workflow för Rum-design

## Workflow Modes

Editorn stödjer två modes:

### Manual Mode (Default)
- Redigera rum direkt i editorn
- Lägg till hotspots, layers, walk areas via UI
- Enklare för små ändringar
- **Aktivera:** Tryck **T** i Rooms tab

### Tiled Mode (Optional)
- Använd externa Tiled-editorn
- Professionella verktyg för level design
- Bättre för komplexa rum med många layers
- **Aktivera:** Tryck **T** i Rooms tab

---

## Installation (för Tiled Mode)

1. Ladda ner Tiled från https://www.mapeditor.org/
2. Installera och starta
3. I spelet: Editor → Rooms → Tryck **T** för Tiled mode

---

## Skapa nytt rum

### 1. Ny karta
- **File → New → New Map**
- **Orientation:** Orthogonal
- **Map size:** 640 x 400 tiles
- **Tile size:** 1 x 1 px (vi använder pixlar, inte tiles)

### 2. Lägg till bakgrund
- **Layer → New → Image Layer**
- Namnge lagret "background"
- Klicka på lagret, välj **Properties**
- Sätt **Image** till din bakgrundsbild

### 3. Skapa hotspots
- **Layer → New → Object Layer**
- Namnge lagret "hotspots"
- Använd **Rectangle Tool** för att rita hotspots

### 4. Sätt hotspot-properties
Markera en hotspot och sätt **Custom Properties**:

| Property | Typ | Beskrivning |
|----------|-----|-------------|
| `id` | string | Unikt ID (default: objektnamn) |
| `type` | string | "exit", "npc", "item", "examine" |
| `targetRoom` | string | Rum att gå till (för exits) |
| `dialogId` | string | Dialog att starta (för NPCs) |
| `examineText` | string | Text när spelaren tittar |
| `funnyFails` | string | Rolig text vid fel interaktion |

### 5. Skapa walk area
- **Layer → New → Object Layer**
- Namnge lagret "walkarea"
- Rita en rektangel som täcker gångbar yta

---

## Exportera

1. **File → Export As**
2. Välj **JSON map files (*.json)**
3. Spara i `assets/tiled/` med rum-id som namn (t.ex. `tavern.json`)

---

## Importera i spelet

```cpp
#include "data/TiledImporter.h"

// Importera ett rum
RoomData room = TiledImporter::importRoom("assets/tiled/tavern.json");

// Importera alla rum från en mapp
auto rooms = TiledImporter::importAllRooms("assets/tiled/");

// Exportera befintligt rum till Tiled-format
TiledImporter::exportRoom(roomData, "assets/tiled/tavern.json");
```

---

## Tips

### Snap to Grid
- **View → Snap to Grid** för exakt placering

### Object Templates
- Skapa templates för vanliga hotspot-typer
- **File → New → New Template**

### Custom Properties
- Använd Tiled's **Object Types Editor** för att definiera standard-properties
- **View → Object Types Editor**

---

## Exempel: Tavern

```
Layers:
├── background (Image Layer)
│   └── tavern_bg.png
├── hotspots (Object Layer)
│   ├── bartender (npc)
│   │   ├── dialogId: "bartender_intro"
│   │   └── examineText: "En storvuxen man..."
│   ├── door_street (exit)
│   │   └── targetRoom: "street"
│   └── chest (item)
│       └── examineText: "En gammal träkista"
└── walkarea (Object Layer)
    └── walkarea (rectangle: 0,260 to 640,400)
```

---

## Filstruktur

```
assets/
├── backgrounds/     # Bakgrundsbilder
│   ├── tavern.png
│   └── street.png
├── tiled/           # Tiled JSON-filer
│   ├── tavern.json
│   └── street.json
└── data/
    └── rooms.json   # Kompilerad rum-data (auto-genererad)
```
