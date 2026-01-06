# OOP Hierarki - Retro Adventure Game

## Komplett Klassöversikt

### 🎮 GAME WORLD HIERARCHY

```
World (hela spelet)
├─ Level (kapitel/akt)
│  └─ Scene (rum/location)
│     └─ GameObject (allt i scenen)
│        ├─ Visual Objects
│        ├─ Interactive Objects
│        └─ Logic Objects
```

---

## 📦 CORE CLASSES (Bas-hierarki)

### Node (Tekniskt namn - bör döpas om)
**Nuvarande:** `engine/core/Node.h`
**Förslag:** `GameObject` eller `SceneObject`

```cpp
class Node {
    // Bas för allt i scenen
    // Har: namn, visibility, children (träd-struktur)
};
```

**Ärver från Node:**
- `Node2D` - Objekt med position/rotation/scale

---

## 🎨 VISUAL OBJECTS (Synliga objekt)

### Sprite-baserade
```
Node2D
├─ Sprite              // Statisk bild
├─ AnimatedSprite      // Animerad sprite
├─ Label               // Text
├─ TileMapLayer        // Tile-baserad karta
└─ ParallaxLayer       // Parallax scrolling bakgrund
```

**Användning:**
- **Sprite** → Bakgrunder, statiska objekt
- **AnimatedSprite** → Karaktärer, animerade objekt
- **Label** → UI-text, dialogrutor
- **TileMapLayer** → Marktiles, väggar
- **ParallaxLayer** → Bakgrundslager med djup

---

## 🎯 INTERACTIVE OBJECTS (Interaktiva objekt)

### Hotspots & Areas
```
Node2D
├─ InteractiveArea     // Klickbara hotspots (dörrar, items, NPCs)
├─ WalkArea            // Där spelaren kan gå
└─ Marker              // Positionsmarkörer (spawn points, waypoints)
```

**Användning:**
- **InteractiveArea** → Dörrar, items att plocka upp, NPCs att prata med
- **WalkArea** → Definierar walkable polygon
- **Marker** → Player spawn, NPC spawn, checkpoints

---

## 👤 ENTITIES (Levande varelser)

```
Entity (abstrakt bas)
├─ Character (kan röra sig, animeras)
│  ├─ PlayerCharacter  // Spelaren
│  └─ NPC              // Icke-spelbar karaktär
```

**Skillnad mot Sprite:**
- **Entity** = Har logik, AI, state
- **Sprite** = Bara visuell representation

**Nuvarande problem:** Entity och Node-hierarkin är separata!
**Förslag:** Entity borde ärva från Node2D

---

## 🌍 WORLD STRUCTURE (Värld-struktur)

### Hierarki
```
World
├─ Level (Chapter/Act)
│  ├─ Scene (Room/Location)
│  │  └─ GameObject (allt i rummet)
│  └─ Scene
└─ Level
```

**Klasser:**
- **World** (`engine/world/World.h`) - Hanterar alla Levels, transitions
- **Level** (`engine/world/Level.h`) - Grupperar Scenes (t.ex. "Act 1: Tavern District")
- **Scene** (`engine/world/Scene.h`) - Ett rum/location, innehåller GameObjects
- **Camera2D** (`engine/world/Camera2D.h`) - Kamera för varje Scene
- **LayerManager** (`engine/world/LayerManager.h`) - Hanterar rendering-lager

---

## 🎭 GAME SYSTEMS (Spel-system)

### Data-system
```
DialogSystem       // Hanterar dialogträd
QuestSystem        // Hanterar quests och objectives
InventorySystem    // Hanterar items i inventory
AISystem           // NPC beteende och scheman
SaveSystem         // Spara/ladda speldata
```

### State-system
```
WorldState         // Global spelstatus (flags, variables)
ConditionSystem    // Evaluerar villkor (if-satser i spelet)
EventBus           // Event-meddelanden mellan system
WorldQuery         // Fråga om världens tillstånd
```

### Special-system
```
CutsceneSystem     // Cutscenes och scripted events
HintSystem         // Hint-system
RecapSystem        // Sammanfattningar
JournalSystem      // Dagbok/journal
GateSystem         // Låsta dörrar/gates
RoomManager        // Legacy room management
```

---

## 📊 DATA STRUCTURES (Data-strukturer)

### GameData.h - JSON-data
```
RoomData           // Rum-definition (legacy)
HotspotData        // Hotspot-definition
WalkAreaData       // Walk area bounds
DialogData         // Dialog-träd
QuestData          // Quest-definition
ItemData           // Item-definition
NPCData            // NPC-definition
```

**Problem:** Dessa är POD (Plain Old Data), inte klasser!
**Används för:** Ladda från JSON → Konvertera till GameObject-hierarki

---

## 🎨 GRAPHICS & RENDERING

```
TextureManager     // Singleton - Laddar/cachar texturer
FontManager        // Singleton - Laddar/cachar fonts
SpriteSheet        // Sprite sheet med frames
Animation          // Animation från sprite sheet
Transition         // Scene transitions (fade, slide)
Renderer           // Rendering utilities
```

---

## 🎵 AUDIO

```
AudioManager       // Singleton - Musik och ljudeffekter
```

---

## 🎮 INPUT & PHYSICS

```
Input              // Singleton - Keyboard/mouse input
PhysicsBody        // Fysik-kropp
KinematicBody      // Kinematisk kropp (för karaktärer)
CollisionShape     // Kollisionsform
SpatialGrid        // Spatial hashing för kollision
```

---

## 🖼️ UI SYSTEM

```
Widget (abstrakt)  // Bas för UI-element
```

---

## 🛠️ UTILITIES

```
Logger             // Logging
FileWatcher        // Fil-övervakning
DataLoader         // Laddar JSON-data
TiledImporter      // Importerar Tiled maps
```

---

## ⚠️ IDENTIFIERADE PROBLEM

### 1. Förvirrande terminologi
- **"Node"** → Borde heta `GameObject` eller `SceneObject`
- **"Scene"** vs **"Room"** → Samma sak, olika namn
- **"Level"** → Oklart om det är Chapter eller Room

### 2. Dubbla hierarkier
```
Node-hierarki:     Node → Node2D → Sprite
Entity-hierarki:   Entity → Character → PlayerCharacter
```
**Problem:** Entity använder inte Node-systemet!

### 3. Data vs Objekt
- **RoomData** (JSON) vs **Scene** (GameObject-container)
- **NPCData** (JSON) vs **NPC** (Entity)
- Ingen tydlig konvertering

### 4. Legacy kod
- **Room.h** - Gammal room-klass (bör tas bort)
- **RoomManager** - Legacy manager (ersätts av World/Level/Scene)

---

## 💡 FÖRESLAGEN FÖRENKLING

### Ny hierarki
```
GameObject (bas för allt)
├─ GameObject2D (position, rotation, scale)
│  ├─ Sprite
│  ├─ AnimatedSprite
│  ├─ InteractiveArea
│  ├─ WalkArea
│  ├─ Marker
│  └─ Character (flyttas hit!)
│     ├─ Player
│     └─ NPC
```

### Tydligare namn
```
Node → GameObject
Scene → Room (eller Location)
Level → Chapter (eller Act)
```

### En hierarki
- Ta bort separat Entity-hierarki
- Allt ärver från GameObject
- Character blir en GameObject2D med AI/state

---

## 📋 NÄSTA STEG

1. **Diskutera:** Vilka namn föredrar du?
2. **Refactoring:** Byt Node → GameObject?
3. **Merge:** Slå ihop Entity och Node-hierarkin?
4. **Cleanup:** Ta bort legacy Room.h och RoomManager?

