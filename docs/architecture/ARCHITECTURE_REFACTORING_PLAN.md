# Architecture Refactoring Plan

> Detaljerad åtgärdsplan för att lösa 18 identifierade arkitekturproblem
> Baserad på CODEBASE_ANALYSIS.md
> Skapad: 2026-01-08

---

## ⚠️ UTGÅNGSPUNKT (3 commits redan gjorda, ej pushade)

### Commit 1: `276da3f`
- ARCHITECTURE_AUDIT.md uppdaterad

### Commit 2: `bb8b006` 
- ✅ CameraActor.h/cpp (NY)
- ✅ LightActor.h/cpp (NY)
- CMakeLists.txt uppdaterad

### Commit 3: `321f4d6`
- ✅ Entity.h → Forwarding header till ActorObject
- PlayerCharacter.h/cpp ändrad
- RoomManager.h/cpp ändrad
- SceneLoader.h/cpp +216 rader
- GameDataLoader.h ändrad
- ViewportPanel.h ändrad
- RoomState.h ändrad

### REDAN KLART:
- [x] Entity.h är forwarding header
- [x] CameraActor och LightActor skapade
- [x] SceneLoader utökad

---

## 📋 ÖVERSIKT

### Prioritering

| Prioritet | Antal | Tidsestimat |
|-----------|-------|-------------|
| 🔴 KRITISK | 2 | 2-3 sessioner |
| 🟠 HÖG | 9 | 4-6 sessioner |
| 🟡 MEDIUM | 6 | 2-3 sessioner |
| 🟢 LÅG | 1 | 0.5 session |

**Total estimering:** 8-12 sessioner

### Strategi: Michael Feathers Legacy Code Approach

1. **Bryt beroenden först** - Skapa forwarding headers, inte ta bort
2. **Testa mellan varje steg** - Bygg och verifiera
3. **Ta bort legacy SIST** - Först när allt fungerar
4. **Ingen commit utan verifikation** - Varje fas ska bygga

---

# FAS 0: FÖRBEREDELSE


## 0.1 Fixa Data-problem Först (Problem #16)

- [ ] **0.2.1** Återställ rooms.json från backup
  ```powershell
  cd assets/data
  copy rooms.json.bak rooms.json
  ```

- [ ] **0.2.2** Verifiera att rooms.json finns och är valid JSON
  ```powershell
  Get-Content rooms.json | ConvertFrom-Json
  ```

- [ ] **0.2.3** Bygg och testa att spelet laddar

---

# FAS 1: KRITISKA PROBLEM (🔴)

## 1.1 Problem #1: Två Character-hierarkier

### Analys
```
LEGACY:
engine::Pawn (ActorObject)
└── Character (entities/Character.h) 
    └── PlayerCharacter (entities/PlayerCharacter.h)

NY:
engine::ActorObjectExtended
└── CharacterActor (actors/CharacterActor.h)
    ├── PlayerActor
    └── NPCActor
```

### Åtgärd: Behåll NY hierarki, deprecera LEGACY

#### Steg 1.1.1: Analysera användning av legacy Character

- [ ] **1.1.1.1** Sök efter alla #include "entities/Character.h"
  ```powershell
  rg '#include.*entities/Character' src/
  ```
  Resultat: _____

- [ ] **1.1.1.2** Sök efter alla användningar av ::Character
  ```powershell
  rg 'Character\s+\w+\s*[=;(]' src/ --type cpp
  ```
  Resultat: _____

- [ ] **1.1.1.3** Lista alla filer som använder Character
  ```
  Filer som använder Character:
  - [ ] _____
  - [ ] _____
  ```

#### Steg 1.1.2: Skapa forwarding header för Character

- [ ] **1.1.2.1** Ändra entities/Character.h till forwarding header
  ```cpp
  /**
   * @file Character.h
   * @brief DEPRECATED: Use engine::CharacterActor instead
   */
  #pragma once
  #include "engine/actors/CharacterActor.h"
  
  // DEPRECATED: Use engine::CharacterActor
  using Character = engine::CharacterActor;
  ```

- [ ] **1.1.2.2** Bygg och verifiera
  ```powershell
  cmake --build . --config Release 2>&1 | Select-Object -Last 30
  ```

- [ ] **1.1.2.3** Fixa eventuella kompileringsfel
  ```
  Fel hittade:
  - [ ] _____
  - [ ] _____
  ```

#### Steg 1.1.3: Skapa forwarding header för PlayerCharacter

- [ ] **1.1.3.1** Ändra entities/PlayerCharacter.h till forwarding header
  ```cpp
  /**
   * @file PlayerCharacter.h
   * @brief DEPRECATED: Use engine::PlayerActor instead
   */
  #pragma once
  #include "engine/actors/CharacterActor.h"
  
  // DEPRECATED: Use engine::PlayerActor
  using PlayerCharacter = engine::PlayerActor;
  ```

- [ ] **1.1.3.2** Bygg och verifiera

- [ ] **1.1.3.3** Fixa eventuella kompileringsfel

#### Steg 1.1.4: Verifiera att CharacterActor har all funktionalitet

- [ ] **1.1.4.1** Jämför Character.h med CharacterActor.h
  
  | Funktion | Character | CharacterActor | Status |
  |----------|-----------|----------------|--------|
  | setTarget() | ✅ | ? | |
  | stop() | ✅ | ? | |
  | isMoving() | ✅ | ? | |
  | getHealth() | ✅ | ? | |
  | setHealth() | ✅ | ? | |
  | isAlive() | ✅ | ? | |
  | isFacingRight() | ✅ | ? | |
  | getSpeed() | ✅ | ? | |
  | setSpeed() | ✅ | ? | |
  | setWalkArea() | ✅ | ? | |
  | clampToWalkArea() | ✅ | ? | |
  | getWidth() | ✅ | ? | |
  | getHeight() | ✅ | ? | |

- [ ] **1.1.4.2** Lägg till saknad funktionalitet i CharacterActor
  ```
  Saknade funktioner att lägga till:
  - [ ] _____
  - [ ] _____
  ```

- [ ] **1.1.4.3** Bygg och verifiera

#### Steg 1.1.5: Testa gameplay

- [ ] **1.1.5.1** Starta RetroGame
- [ ] **1.1.5.2** Verifiera att spelaren kan röra sig
- [ ] **1.1.5.3** Verifiera att NPC:er fungerar
- [ ] **1.1.5.4** Verifiera att dialoger fungerar

---

## 1.2 Problem #2: nodes/ ärver från actors/

### Analys
```
engine::ActorObjectExtended
└── engine::VisualActor (actors/)
    └── Sprite (nodes/)       # LEGACY ärver NY!
        └── AnimatedSprite
    └── TileMapLayer (nodes/) # Fortfarande kompilerad!
```

### Åtgärd: Bryt beroendet genom att flytta till actors/

#### Steg 1.2.1: Analysera nodes/ användning

- [ ] **1.2.1.1** Lista alla nodes/-filer som kompileras
  ```powershell
  rg 'src/engine/nodes/' CMakeLists.txt
  ```
  Resultat:
  - TileMapLayer.cpp (aktiv)
  - Övriga (kommenterade ut)

- [ ] **1.2.1.2** Sök efter användning av TileMapLayer
  ```powershell
  rg 'TileMapLayer' src/ --type cpp --type h
  ```
  Resultat: _____

#### Steg 1.2.2: Beslut för varje nodes/-klass

| Klass | Åtgärd | Status |
|-------|--------|--------|
| Sprite | Ersatt av SpriteActor | [ ] Ta bort |
| AnimatedSprite | Ersatt av SpriteActor + AnimationComponent | [ ] Ta bort |
| TileMapLayer | Behövs av editor | [ ] Flytta till actors/ |
| ParallaxLayer | Ersätt med ParallaxActor | [ ] Ta bort |
| Label | Ersätt med TextActor eller UI | [ ] Ta bort |
| InteractiveArea | Ersatt av InteractiveActor | [ ] Ta bort |
| WalkArea | Ersatt av WalkArea i actors/ | [ ] Ta bort |
| Marker | Ersatt av Marker i actors/ | [ ] Ta bort |

#### Steg 1.2.3: Migrera TileMapLayer till actors/

- [ ] **1.2.3.1** Skapa actors/TileMapActor.h baserat på nodes/TileMapLayer.h
- [ ] **1.2.3.2** Uppdatera CMakeLists.txt
- [ ] **1.2.3.3** Skapa forwarding header i nodes/TileMapLayer.h
- [ ] **1.2.3.4** Bygg och verifiera

#### Steg 1.2.4: Kommentera ut övriga nodes/ från CMakeLists

- [ ] **1.2.4.1** Verifiera att alla nodes/ redan är kommenterade ut
- [ ] **1.2.4.2** Om inte, kommentera ut och bygg

---

# FAS 2: HÖGA PROBLEM (🟠)

## 2.1 Problem #3: Dubbla Label-klasser

### Åtgärd: Konsolidera till en Label

- [ ] **2.1.1** Analysera vilken Label som används var
  - nodes/Label.h → Används i _____
  - ui/Widget.h Label → Används i _____

- [ ] **2.1.2** Beslut: Behåll UI-Label, ta bort nodes/Label

- [ ] **2.1.3** Skapa forwarding header eller ta bort

- [ ] **2.1.4** Bygg och verifiera

---

## 2.2 Problem #4: PhysicsBody ärver ActorObject

### Analys
```cpp
class PhysicsBody : public ActorObject { }  // Saknar components!
class KinematicBody : public PhysicsBody { }
```

### Åtgärd: Ändra till ActorObjectExtended

- [ ] **2.2.1** Ändra PhysicsBody.h
  ```cpp
  // FÖRE
  class PhysicsBody : public ActorObject { }
  
  // EFTER
  class PhysicsBody : public ActorObjectExtended { }
  ```

- [ ] **2.2.2** Bygg och verifiera

- [ ] **2.2.3** Testa physics-funktionalitet

---

## 2.3 Problem #5: DataLoader vs GameDataLoader

### Åtgärd: Ta bort GameDataLoader, flytta logik till Systems

- [ ] **2.3.1** Analysera vad GameDataLoader gör
  - loadItems() → InventorySystem
  - loadQuests() → QuestSystem
  - loadDialogs() → DialogSystem
  - loadRooms() → RoomManager/SceneManager
  - loadNPCs() → AISystem

- [ ] **2.3.2** Verifiera att Systems har loadFromData()

- [ ] **2.3.3** Ändra anrop från GameDataLoader till Systems direkt

- [ ] **2.3.4** Ta bort GameDataLoader.h (eller lämna som tom forwarding)

- [ ] **2.3.5** Bygg och verifiera

---

## 2.4 Problem #8: NPC finns på två ställen

### Åtgärd: Behåll actors/NPC.h, ta bort entities/NPC.cpp

- [ ] **2.4.1** Verifiera att entities/NPC.cpp INTE kompileras (redan utkommenterad)

- [ ] **2.4.2** Sök efter användning av entities/NPC
  ```powershell
  rg '#include.*entities/NPC' src/
  ```

- [ ] **2.4.3** Om finns, ändra till actors/NPC.h

- [ ] **2.4.4** Bygg och verifiera

---

## 2.5 Problem #9: ViewportPanel dubbelt

### Åtgärd: Ta bort oanvänd ViewportPanel

- [ ] **2.5.1** Verifiera att viewport/ViewportPanel.h/cpp INTE inkluderas

- [ ] **2.5.2** Ta bort eller flytta till legacy/
  ```
  src/editor/viewport/ViewportPanel.h → DELETE eller legacy/
  src/editor/viewport/ViewportPanel.cpp → DELETE eller legacy/
  ```

- [ ] **2.5.3** Uppdatera CMakeLists.txt om nödvändigt

- [ ] **2.5.4** Bygg och verifiera

---

## 2.6 Problem #11: RoomState på fel plats

### Åtgärd: Flytta RoomPanel till editor/panels/

- [ ] **2.6.1** Byt namn RoomState.h → RoomPanel.h (redan korrekt klassnamn)

- [ ] **2.6.2** Flytta till editor/panels/editors/
  ```
  game/states/RoomState.h → editor/panels/editors/RoomPanel.h
  game/states/RoomState.cpp → editor/panels/editors/RoomPanel.cpp
  ```

- [ ] **2.6.3** Uppdatera CMakeLists.txt

- [ ] **2.6.4** Uppdatera #include-sökvägar

- [ ] **2.6.5** Bygg och verifiera

---

## 2.7 Problem #12: Dubbla IEditorTool

### Åtgärd: Konsolidera till en IEditorTool

- [ ] **2.7.1** Jämför editor/tools/IEditorTool.h med ai/core/IEditorTool.h

- [ ] **2.7.2** Beslut vilken som ska vara kanonisk

- [ ] **2.7.3** Skapa forwarding header eller byt namn på en

- [ ] **2.7.4** Bygg och verifiera

---

## 2.8 Problem #16: rooms.json saknas

### Åtgärd: Återställ från backup (FAS 0)

- [ ] **2.8.1** Redan åtgärdat i Fas 0.2

---

# FAS 3: MEDIUM PROBLEM (🟡)

## 3.1 Problem #6: Namespace-blandning

### Åtgärd: Dokumentera och gradvis migrera

- [ ] **3.1.1** Lista alla klasser utan namespace
  ```
  Klasser utan namespace:
  - Character (entities/)
  - PlayerCharacter (entities/)
  - Sprite (nodes/)
  - DataLoader (data/)
  - RoomManager (systems/)
  - SceneManager (systems/)
  ```

- [ ] **3.1.2** Skapa migreringsplan (framtida fas)

- [ ] **3.1.3** Dokumentera i ARCHITECTURE.md

---

## 3.2 Problem #7: 16+ singletons

### Åtgärd: Dokumentera, konsolidera framtida

- [ ] **3.2.1** Lista alla singletons
  ```
  Singletons i systems/:
  - AISystem
  - ConditionSystem
  - CutsceneSystem
  - DialogSystem
  - EventBus
  - GateSystem
  - HintSystem
  - InventorySystem
  - JournalSystem
  - QuestSystem
  - RecapSystem
  - RoomManager
  - SaveSystem
  - SceneManager
  - WorldQuery
  - WorldState
  
  Övriga singletons:
  - DataLoader
  - TextureManager
  - GLTextureManager
  ```

- [ ] **3.2.2** Identifiera kandidater för konsolidering
  ```
  Föreslagna konsolideringar:
  - ConditionSystem + DialogSystem → DialogSystem
  - RecapSystem + JournalSystem → JournalSystem
  - HintSystem + QuestSystem → QuestSystem
  - GateSystem + AISystem → AISystem
  ```

- [ ] **3.2.3** Dokumentera i ADR

---

## 3.3 Problem #10: RoomManager vs SceneManager

### Åtgärd: Skapa delegation, behåll båda temporärt

- [ ] **3.3.1** RoomManager ska delegera till SceneManager

- [ ] **3.3.2** Markera RoomManager som DEPRECATED

- [ ] **3.3.3** Uppdatera dokumentation

---

## 3.4 Problem #13: Game.cpp kompileras dubbelt

### Åtgärd: Länka mot library istället

- [ ] **3.4.1** Skapa RetroGameLib med Game.cpp och states
  ```cmake
  add_library(RetroGameLib STATIC
      src/game/Game.cpp
      src/game/states/StateManager.cpp
      ...
  )
  ```

- [ ] **3.4.2** Länka RetroGame och RetroEditor mot RetroGameLib
  ```cmake
  target_link_libraries(RetroGame PRIVATE RetroCore RetroGameLib)
  target_link_libraries(RetroEditor PRIVATE RetroCore RetroGameLib)
  ```

- [ ] **3.4.3** Ta bort duplicerade filer från EDITOR_SOURCES

- [ ] **3.4.4** Bygg och verifiera

---

## 3.5 Problem #14: AI-system odokumenterat

### Åtgärd: Dokumentera

- [ ] **3.5.1** Skapa docs/architecture/AI_SYSTEM.md

- [ ] **3.5.2** Dokumentera:
  - AIAgentSystem
  - ILLMProvider
  - EditorToolRegistry
  - Alla tools

---

## 3.6 Problem #15: TileMapLayer fortfarande aktiv

### Åtgärd: Migrera till TileMapActor (Fas 1.2)

- [ ] **3.6.1** Redan åtgärdat i Fas 1.2

---

## 3.7 Problem #17: assets/ mappar tomma

### Åtgärd: Generera eller dokumentera

- [ ] **3.7.1** Kör asset generator
  ```powershell
  cd generator
  python generate_all.py
  ```

- [ ] **3.7.2** Eller dokumentera att de är placeholders

---

# FAS 4: VERIFIKATION

## 4.1 Build-verifikation

- [ ] **4.1.1** Bygg RetroCore
  ```powershell
  cmake --build . --config Release --target RetroCore
  ```

- [ ] **4.1.2** Bygg RetroGame
  ```powershell
  cmake --build . --config Release --target RetroGame
  ```

- [ ] **4.1.3** Bygg RetroEditor
  ```powershell
  cmake --build . --config Release --target RetroEditor
  ```

- [ ] **4.1.4** Dokumentera build-tid
  ```
  Build-tid efter: _____ sekunder
  Förändring: _____
  ```

## 4.2 Funktionstest

- [ ] **4.2.1** Starta RetroGame
  - [ ] Huvudmeny visas
  - [ ] Kan starta nytt spel
  - [ ] Spelare rör sig
  - [ ] NPC:er fungerar
  - [ ] Dialoger fungerar

- [ ] **4.2.2** Starta RetroEditor
  - [ ] Alla paneler öppnas
  - [ ] Viewport fungerar
  - [ ] 3D viewport fungerar
  - [ ] Kan spara/ladda

## 4.3 Kod-kvalitet

- [ ] **4.3.1** Inga compiler warnings
- [ ] **4.3.2** Inga deprecated warnings
- [ ] **4.3.3** Dokumentation uppdaterad

---

# FAS 5: CLEANUP

## 5.1 Ta bort deprecated filer

**ENDAST efter att allt fungerar!**

- [ ] **5.1.1** Ta bort entities/Entity.cpp (behåll .h som forwarding)
- [ ] **5.1.2** Ta bort entities/Character.cpp (behåll .h som forwarding)
- [ ] **5.1.3** Ta bort entities/PlayerCharacter.cpp (behåll .h som forwarding)
- [ ] **5.1.4** Ta bort viewport/ViewportPanel.h/cpp
- [ ] **5.1.5** Flytta nodes/ till legacy/ eller ta bort

## 5.2 Uppdatera dokumentation

- [ ] **5.2.1** Uppdatera ARCHITECTURE_AUDIT.md
- [ ] **5.2.2** Uppdatera README.md
- [ ] **5.2.3** Skapa ADR för arkitekturändringarna

## 5.3 Final Verifikation

- [ ] **5.3.1** Verifiera att alla ändringar fungerar
- [ ] **5.3.2** Kör RetroGame och RetroEditor en sista gång
- [ ] **5.3.3** Meddela användaren att refaktorering är klar

---

# APPENDIX: RISKANALYS

## Högrisk-ändringar

| Ändring | Risk | Mitigation |
|---------|------|------------|
| Character → CharacterActor | Gameplay kan brytas | Testa alla karaktärsfunktioner |
| PhysicsBody-arv | Physics kan brytas | Testa kollisioner |
| CMakeLists-ändringar | Build kan brytas | Inkrementella ändringar |

## Rollback-plan

Om något går fel: Återställ filer manuellt eller använd IDE:ns undo-funktion.

---

# CHECKLISTA SAMMANFATTNING

| Fas | Beskrivning | Steg | Status |
|-----|-------------|------|--------|
| 0 | Förberedelse | 3 | [ ] |
| 1 | Kritiska problem | 2 | [ ] |
| 2 | Höga problem | 8 | [ ] |
| 3 | Medium problem | 7 | [ ] |
| 4 | Verifikation | 3 | [ ] |
| 5 | Cleanup | 3 | [ ] |

**Totalt:** ~26 huvudsteg med ~100 delsteg

---

*Skapad: 2026-01-08*
*Baserad på: CODEBASE_ANALYSIS.md*
*Metod: Michael Feathers Legacy Code Change Algorithm*
