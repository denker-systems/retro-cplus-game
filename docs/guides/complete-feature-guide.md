# Komplett Feature Guide

Detaljerad guide för alla features i Retro Adventure.

---

## State Management

### Tillgängliga States

| State | Beskrivning | Aktivering |
|-------|-------------|------------|
| MenuState | Huvudmeny | Start av spel |
| PlayState | Gameplay | New Game / Load |
| PauseState | Pausmeny (overlay) | ESC under spel |
| DialogState | Dialog med NPC (overlay) | Klicka på NPC |
| InventoryState | Inventory (overlay) | I-tangent |
| QuestLogState | Quest log (overlay) | J-tangent |
| OptionsState | Inställningar | Options från meny |
| EditorState | In-game editor | F1-tangent |
| SaveLoadState | Save/Load meny | Från pause |

### State Stack

States använder en stack för overlays:

```
┌─────────────────┐
│ DialogState     │ ← Överst (aktiv)
├─────────────────┤
│ PlayState       │ ← Pausad men synlig
└─────────────────┘
```

**Exempel:**
```cpp
// Öppna inventory (overlay)
game->pushState(std::make_unique<InventoryState>());

// Stäng inventory
game->popState();

// Byt state helt
game->changeState(std::make_unique<MenuState>());
```

---

## Room System

### Walk Areas

Walk areas definierar var spelaren kan gå:

```
┌─────────────────────────────────────┐
│                                     │
│  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░   │ ← Ej walkable
│  ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░   │
│  ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓   │ ← Walk area
│  ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓   │
│  ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓   │
└─────────────────────────────────────┘
```

**Parametrar:**
- `minX, maxX` - Horisontella gränser
- `minY, maxY` - Vertikala gränser
- `scaleTop` - Karaktärsstorlek vid minY (0.1-2.0)
- `scaleBottom` - Karaktärsstorlek vid maxY (0.1-2.0)

### Depth Scaling

Karaktärer skalas automatiskt baserat på Y-position:

```
Y=220 (minY) → scale=0.6 → 🧍 (60% storlek)
Y=300        → scale=0.8 → 🧍 (80% storlek)
Y=380 (maxY) → scale=1.0 → 🧍🧍 (100% storlek)
```

**Formel:**
```cpp
float t = (playerY - minY) / (maxY - minY);
float scale = scaleTop + t * (scaleBottom - scaleTop);
```

### Hotspots

Hotspots är interaktiva områden i rum:

**Typer:**
- **NPC** - Karaktärer att prata med (grön)
- **Item** - Objekt att plocka upp (gul)
- **Exit** - Dörrar/övergångar (blå)
- **Examine** - Objekt att undersöka (grå)

**Interaktion:**
1. Vänsterklick på hotspot
2. Spelaren går dit
3. Vid ankomst: trigga action (dialog, pickup, etc.)

### Multi-Layer Backgrounds

Rum kan ha flera lager med parallax:

```json
"layers": [
  {
    "image": "sky.png",
    "zIndex": -2,
    "parallaxX": 0.5,  // Rör sig långsammare
    "parallaxY": 0.5,
    "opacity": 1.0
  },
  {
    "image": "background.png",
    "zIndex": -1,
    "parallaxX": 1.0,  // Normal hastighet
    "parallaxY": 1.0,
    "opacity": 1.0
  }
]
```

---

## Dialog System

### Dialog Trees

Dialoger är träd med noder och val:

```
┌─────────────────────────────────┐
│ Node 0: "Welcome!"              │
├─────────────────────────────────┤
│ [1] "Tell me about this place"  │ → Node 1
│ [2] "Goodbye"                   │ → End (-1)
└─────────────────────────────────┘
```

### Villkor och Actions

Val kan ha villkor och actions:

```cpp
DialogChoice choice;
choice.text = "Here's 10 gold";
choice.nextNodeId = 5;

// Villkor: Visas bara om spelaren har guld
choice.condition = []() {
    return InventorySystem::instance().getGold() >= 10;
};

// Action: Körs när valet väljs
choice.action = []() {
    InventorySystem::instance().removeGold(10);
    QuestSystem::instance().updateObjective("quest1", "paid_bartender");
};
```

### Dynamic Text

Dialog-text kan vara dynamisk:

```cpp
std::string getText() {
    std::string name = WorldQuery::getPlayerName();
    int gold = InventorySystem::instance().getGold();
    return "Hello " + name + "! You have " + std::to_string(gold) + " gold.";
}
```

---

## Inventory System

### Item Management

**Lägga till item:**
```cpp
InventorySystem::instance().addItem("rusty_key");
```

**Ta bort item:**
```cpp
InventorySystem::instance().removeItem("health_potion");
```

**Kolla om item finns:**
```cpp
if (InventorySystem::instance().hasItem("cellar_key")) {
    // Lås upp dörr
}
```

### Item Combination

Kombinera items för att skapa nya:

```cpp
std::string result = InventorySystem::instance().combineItems("rope", "hook");
if (result == "grappling_hook") {
    LOG_INFO("Created grappling hook!");
}
```

**Registrera kombination:**
```cpp
InventorySystem::instance().registerCombination("rope", "hook", "grappling_hook");
```

### Item Usage

Använd items på targets:

```cpp
// Använd nyckel på dörr
if (InventorySystem::instance().useItem("rusty_key", "cellar_door")) {
    // Dörr öppnad
    RoomManager::instance().changeRoom("cellar");
}
```

**Registrera användning:**
```cpp
InventorySystem::instance().registerUseCallback("health_potion", [](const std::string& target) {
    PlayerCharacter::instance().heal(50);
    return true;
});
```

---

## Quest System

### Quest Structure

Quests består av objectives:

```
Quest: "The Lost Treasure"
├─ [✓] Talk to bartender
├─ [✓] Find cellar key
├─ [ ] Enter cellar
└─ [ ] Open treasure chest
```

### Objective Types

| Type | Beskrivning | Auto-Complete |
|------|-------------|---------------|
| Talk | Prata med NPC | Nej (manuell) |
| Collect | Samla item | Ja (när item i inventory) |
| Deliver | Leverera item | Nej (manuell) |
| GoTo | Gå till rum | Ja (när i rum) |
| Examine | Undersök objekt | Nej (manuell) |
| Custom | Anpassad condition | Ja (om condition true) |

### Quest Progression

**Starta quest:**
```cpp
QuestSystem::instance().startQuest("find_treasure");
```

**Uppdatera objective:**
```cpp
QuestSystem::instance().updateObjective("find_treasure", "talk_to_bartender");
```

**Completion callback:**
```cpp
quest.onComplete = []() {
    InventorySystem::instance().addGold(100);
    showMessage("Quest completed! +100 gold");
};
```

---

## AI System

### NPC Behaviors

| Behavior | Beskrivning |
|----------|-------------|
| Idle | Står still |
| Patrol | Patrullerar waypoints |
| Wander | Vandrar slumpmässigt |
| Follow | Följer target |
| GoTo | Går till specifik punkt |
| Schedule | Följer tidschema |

### Patrol

NPC går mellan waypoints:

```cpp
std::vector<Waypoint> waypoints = {
    {100, 200, 2.0f},  // x, y, wait time
    {300, 200, 1.0f},
    {300, 300, 0.0f},
    {100, 300, 1.5f}
};
AISystem::instance().setWaypoints("Guard", waypoints);
AISystem::instance().setBehavior("Guard", BehaviorType::Patrol);
```

### Schedule

NPC följer tidschema:

```cpp
std::vector<ScheduleEntry> schedule = {
    {8, 18, {400, 270}, AIState::Working, "shop"},    // 08:00-18:00
    {18, 22, {200, 300}, AIState::Idle, "tavern"},   // 18:00-22:00
    {22, 8, {150, 250}, AIState::Sleeping, "home"}   // 22:00-08:00
};
AISystem::instance().setSchedule("Shopkeeper", schedule);
```

### Follow

NPC följer target (t.ex. spelaren):

```cpp
AISystem::instance().setBehavior("Companion", BehaviorType::Follow);
AISystem::instance().setFollowTarget("Companion", player);
```

---

## Save/Load System

### Save Slots

Spelet stödjer flera save slots:

```
assets/saves/
├── slot1.json      # Manual save
├── slot2.json      # Manual save
├── slot3.json      # Manual save
├── quicksave.json  # F5
└── autosave.json   # Auto
```

### Quick Save/Load

**Quick Save (F5):**
```cpp
SaveSystem::instance().quickSave();
```

**Quick Load (F9):**
```cpp
SaveSystem::instance().quickLoad();
```

### Auto-Save

Auto-save triggas vid:
- Room transitions
- Quest completion
- Efter 5 minuters spelande

**Manuell auto-save:**
```cpp
SaveSystem::instance().autoSave();
```

### Save Data

Sparad data inkluderar:
- Nuvarande rum och position
- Inventory items
- Quest status
- Flags (bool)
- Counters (int)
- Speltid

---

## Audio System

### Music

**Spela musik:**
```cpp
AudioManager::instance().playMusic("tavern_theme", true); // Loop
```

**Stoppa musik:**
```cpp
AudioManager::instance().stopMusic();
```

**Fade out:**
```cpp
AudioManager::instance().fadeOutMusic(2000); // 2 sekunder
```

### Sound Effects

**Spela ljud:**
```cpp
AudioManager::instance().playSound("door_open");
```

**Ladda ljud:**
```cpp
AudioManager::instance().loadSound("pickup", "assets/sounds/pickup.wav");
```

### Volume Control

**Sätt volym:**
```cpp
AudioManager::instance().setMusicVolume(0.7f);  // 70%
AudioManager::instance().setSFXVolume(0.8f);    // 80%
```

### Global Mute

**Toggle mute (M-tangent):**
```cpp
AudioManager::instance().toggleMute();
```

**Kolla mute status:**
```cpp
if (AudioManager::instance().isMuted()) {
    // Visa mute-ikon
}
```

---

## Cutscene System

### Cutscene Actions

| Action | Beskrivning |
|--------|-------------|
| Text | Visa text overlay |
| FadeIn | Fade in från svart |
| FadeOut | Fade out till svart |
| Wait | Vänta X sekunder |
| MoveNPC | Flytta NPC |
| PlaySound | Spela ljudeffekt |
| Custom | Anpassad lambda |

### Skapa Cutscene

```cpp
auto cutscene = CutsceneSystem::createSimpleCutscene({
    "Welcome to the tavern!",
    "The bartender looks at you suspiciously.",
    "What will you do?"
});

CutsceneSystem::instance().playCutscene(std::move(cutscene));
```

### Avancerad Cutscene

```cpp
Cutscene cutscene;

// Fade in
cutscene.addAction(std::make_unique<FadeInAction>(1.0f));

// Text med speaker
cutscene.addAction(std::make_unique<TextAction>(
    "Bartender", 
    "Welcome, stranger!", 
    3.0f
));

// Flytta NPC
cutscene.addAction(std::make_unique<MoveNPCAction>(
    "bartender",
    320.0f, 300.0f,
    2.0f
));

// Custom action
cutscene.addAction(std::make_unique<CustomAction>([]() {
    QuestSystem::instance().startQuest("tavern_mystery");
}));

// Fade out
cutscene.addAction(std::make_unique<FadeOutAction>(1.0f));

CutsceneSystem::instance().playCutscene(std::move(cutscene));
```

---

## Editor System

### Öppna Editor

**F1-tangent** öppnar in-game editor.

### Hotspot Editing

**Drag:**
- Klicka och dra hotspot för att flytta

**Resize:**
- Dra i hörnen för att ändra storlek

**Delete:**
- Välj hotspot och tryck DEL

**Add:**
- H-tangent → Klicka i rummet

### Walk Area Editing

**Handles:**
- **Top/Bottom** - Dra för att ändra minY/maxY
- **Left/Right** - Dra för att ändra minX/maxX

**Depth Scaling:**
- **+** - Öka bottom scale
- **-** - Minska bottom scale
- **Shift++** - Öka top scale
- **Shift+-** - Minska top scale

### Spara Ändringar

**Ctrl+S** eller klicka "Save" sparar till `rooms.json`.

---

## World Query System

### Flags

Flags är boolean värden för story progression:

**Sätt flag:**
```cpp
WorldQuery::setFlag("talked_to_bartender", true);
```

**Kolla flag:**
```cpp
if (WorldQuery::getFlag("cellar_unlocked")) {
    // Dörr är upplåst
}
```

### Counters

Counters är integer värden:

**Öka counter:**
```cpp
WorldQuery::incrementCounter("enemies_defeated");
```

**Sätt counter:**
```cpp
WorldQuery::setCounter("gold_collected", 250);
```

**Hämta counter:**
```cpp
int kills = WorldQuery::getCounter("enemies_defeated");
```

---

## Hint System

### Funny Fails

När spelaren försöker göra något dumt:

```json
"funnyFails": [
  "Jag tror inte han vill bli puttad på.",
  "Att stjäla hans mustasch vore nog olämpligt.",
  "Han ser ut som han kan krossa mig med blotta blicken."
]
```

**Visa random fail:**
```cpp
HintSystem::instance().showFunnyFail(hotspot->funnyFails);
```

### Context Hints

Hints baserat på situation:

```cpp
if (QuestSystem::instance().isQuestActive("find_key")) {
    if (WorldQuery::getFlag("searched_bartender")) {
        HintSystem::instance().showHint("Maybe check the cellar?");
    }
}
```

---

## Keyboard Shortcuts

### Gameplay

| Tangent | Funktion |
|---------|----------|
| WASD / Arrows | Rörelse |
| Left Click | Interagera / Gå |
| ESC | Pausmeny |
| I | Inventory |
| J | Quest Log |
| M | Mute/Unmute |
| F1 | Editor |
| F5 | Quick Save |
| F9 | Quick Load |

### Editor

| Tangent | Funktion |
|---------|----------|
| Ctrl+Z | Undo |
| Ctrl+Y | Redo |
| Ctrl+S | Save |
| DEL | Delete selected |
| H | Add hotspot |
| +/- | Adjust depth scale |
| Shift++/- | Adjust top scale |

---

## Performance Tips

### Texture Management

Texturer cachas automatiskt:

```cpp
// Första gången: Laddar från disk
auto* tex = TextureManager::instance().loadTexture(renderer, "bg.png");

// Andra gången: Hämtar från cache
auto* tex2 = TextureManager::instance().loadTexture(renderer, "bg.png");
```

### Animation Optimization

Använd frame-skipping för många NPCs:

```cpp
if (frameCount % 2 == 0) {
    npc->update(deltaTime * 2);
}
```

### Audio Channels

SDL_mixer har begränsat antal kanaler:

```cpp
Mix_AllocateChannels(16); // Max 16 samtidiga ljud
```

---

## Troubleshooting

### Vanliga Problem

**Problem:** Spelet kraschar vid start
**Lösning:** Kolla att alla assets finns i `assets/` mappen

**Problem:** Ingen ljud
**Lösning:** Kolla att SDL2_mixer är installerat och att ljudfiler är .wav/.ogg

**Problem:** Hotspots syns inte
**Lösning:** Aktivera debug rendering i Room.cpp

**Problem:** Sparning fungerar inte
**Lösning:** Kolla att `assets/saves/` mappen finns

**Problem:** Font rendering ser suddig ut
**Lösning:** FontManager skalar automatiskt, men kolla att font-filer finns

---

## Se även

- [API Documentation](../api/)
- [Architecture](../ARCHITECTURE.md)
- [Data Formats](../technical/data-formats.md)
- [Getting Started](getting-started.md)
