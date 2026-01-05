# LucasArts-insikter Applicerade på Retro Adventure

> Baserat på djupstudien av Lucasfilm Games/LucasArts adventure-spel och Ron Gilberts designprinciper.

---

## Sammanfattning: Vad vi redan har vs. vad vi bör lägga till

| LucasArts-princip | Vårt spel idag | Åtgärd |
|-------------------|----------------|--------|
| **Låg friktion** | ✅ Point-and-click, E-key | Behåll |
| **Reaktiv värld** | ⚠️ Grundläggande dialog | **Utöka reaktivitet** |
| **Tydliga mål** | ⚠️ QuestSystem finns | **Lägg till hint-system** |
| **Pussel driver story** | ⚠️ Grundläggande | **Designa bättre pussel** |
| **Adaptiv musik (iMUSE)** | ❌ Saknas | **Implementera** |
| **Inga dead-ends** | ✅ Inget perma-fail | Behåll |

---

## 1. Konkreta Implementeringar

### 1.1 Tiered Hint System (Gilbert: "Sub-goals need to be obvious")

**Varför:** Spelare ska aldrig fastna 45+ minuter och tappa storyn.

```cpp
// Förslag: HintSystem.h
enum class HintLevel {
    Goal,       // "Hitta ett sätt att komma in i biblioteket"
    Location,   // "Vakten vid ingången kan kanske hjälpa"
    Method,     // "Vakten gillar öl..."
    Solution    // "Ge ölen till vakten"
};

class HintSystem {
    std::string getHint(const std::string& questId, HintLevel level);
    void requestHint();  // Ökar hint-nivå progressivt
};
```

**UI:** Knapp i spelet (t.ex. "?") som ger hints i nivåer.

---

### 1.2 Adaptiv Musik (iMUSE-princip)

**Varför:** Musik ska regissera scener, inte bara loopa.

**Implementation i vårt AudioManager:**

```cpp
// Lägg till i AudioManager
struct MusicLayer {
    std::string baseTrack;
    std::string tensionLayer;
    std::string resolutionSting;
};

void setMusicState(MusicState state);  // Normal, Tense, Discovery, Victory
void transitionToRoom(const std::string& roomId);  // Sömlös övergång
```

**Praktiskt:**
- Varje rum har ett "tema" som varierar baserat på spelarens progress
- "Puzzle tension" ökar när spelaren är nära lösningen
- Kort "sting" när pussel löses

---

### 1.3 Reaktivitet som Standard (SCUMM-lärdomen)

**Regel:** Minst 2 reaktiva repliker per interagerbart objekt.

| Interaktion | Normal respons | "Dum" respons |
|-------------|----------------|---------------|
| Titta på dörr | "En massiv ekdörr." | - |
| Använd nyckel på dörr | "Den passar!" | "Den nyckeln passar inte." |
| Använd fisk på dörr | - | "Varför skulle jag göra det?" |

**Implementation:** Utöka `Hotspot` med `funnyFails`:

```cpp
struct Hotspot {
    // ... befintliga fält
    std::vector<std::string> funnyFails;  // Roliga svar på dumma försök
};
```

---

### 1.4 Pussel med Två Lösningar (Fate of Atlantis-modellen)

**Princip:** Varje "gate" ska ha minst 2 approaches.

**Exempel för vårt spel:**
```
Gate: Komma förbi vakten

Approach A (Social): Prata med bartendern → få info → övertyga vakten
Approach B (Logik): Hitta hemlig passage via källaren
Approach C (Item): Hitta förklädnad i butiken
```

**Data-struktur:**
```json
{
  "gate_id": "guard_passage",
  "approaches": [
    {"type": "social", "required_dialog": "bartender_info"},
    {"type": "exploration", "required_item": "cellar_key"},
    {"type": "item", "required_item": "disguise"}
  ]
}
```

---

### 1.5 "Reward Intent" System

**Gilbert-principen:** Om spelaren försöker göra rätt sak, hjälp dem.

**Implementation:**
```cpp
// I PlayState::interactWithHotspot
if (isCloseToSolution(hotspot, usedItem)) {
    // Ge diegetisk hint istället för "det fungerar inte"
    showReaction("Hmm, jag är på rätt spår men behöver något mer...");
} else {
    showReaction(hotspot->getRandomFunnyFail());
}
```

---

## 2. Prioriterad Implementeringsordning

```
1. HintSystem (4 nivåer)           # Anti-frustration
2. Utökad reaktivitet i dialog     # Världen känns levande
3. Adaptiv musik (basic)           # Emotionell navigering
4. Pussel med alternativa lösningar # Player agency
5. "Reward Intent" logik           # Hjälp spelaren
```

---

## 3. Design Pillars (Från Case Study)

### Pillar 1: Story genom handling
> Varje pussel ska ändra relation, plats, hot, eller kunskap.

**Checklista för varje pussel:**
- [ ] Ändrar detta spelarens relation till en NPC?
- [ ] Öppnar detta ett nytt område?
- [ ] Avslöjar detta ny information?
- [ ] Skapar detta nytt hot/möjlighet?

### Pillar 2: Noll obligatoriska minispel
> Inga "Monkey Kombat"-moment som blockerar progression.

**Regel:** Om vi lägger till minispel, gör dem:
- Valfria
- Skippbara efter 2-3 försök
- Aldrig i slutet av spelet

### Pillar 3: UI respekterar tempo
> Sam & Max/Curse-filosofin: få actions, snabb interaktion.

**Vår nuvarande UI:**
- Klick för interaktion ✅
- E-key för närhet ✅
- **Lägg till:** Radial-meny för snabbare verb-val?

### Pillar 4: "Backwards Puzzles"
> Problemet ska hittas innan lösningen.

**Design-regel:** Spelaren ska se crevassen innan de hittar repet.

---

## 4. Specifika Förbättringar per System

### DialogSystem
- [ ] Lägg till `tone_preview` (visa intention, inte exakt replik)
- [ ] Variantdialog baserat på ordning (vad spelaren redan vet)
- [ ] "Recap barks" (karaktären sammanfattar situationen)

### QuestSystem
- [ ] Integrera med HintSystem
- [ ] Visa delmål tydligare
- [ ] "Journal" som sammanfattar vad spelaren vet

### AudioManager
- [ ] Room-baserade teman
- [ ] Sömlösa övergångar (crossfade)
- [ ] State-baserad musik (normal/tense/victory)

### RoomManager
- [ ] Snabbresa mellan kända rum (minska backtracking)
- [ ] "Map view" för större världar

---

## 5. Anti-Patterns att Undvika

| Anti-Pattern | LucasArts-exempel | Vår åtgärd |
|--------------|-------------------|------------|
| Moon logic | The Dig | Alla pussel ska följa etablerade världsregler |
| Backtracking | Monkey Island 2 | Snabbresa, sammanfattningsjournal |
| Missade items | Sierra-spel | Inga omissliga items tidigt |
| Klumpiga kontroller | Grim Fandango | Point-and-click, aldrig tank controls |
| Obligatoriska minispel | Escape from MI | Alla minispel valfria/skippbara |

---

## 6. Nästa Steg

### Immediate (denna session)
1. Skapa `HintSystem` grundstruktur
2. Utöka `Hotspot` med `funnyFails`

### Short-term (nästa session)
3. Implementera basic adaptiv musik i `AudioManager`
4. Lägg till reaktivitet i befintliga dialoger

### Medium-term
5. Designa pussel med alternativa lösningar
6. Implementera "tone preview" i dialog-UI

---

## Referenser

- [Ron Gilbert: Why Adventure Games Suck](https://grumpygamer.com/why_adventure_games_suck/)
- [iMUSE Wikipedia](https://en.wikipedia.org/wiki/IMUSE)
- [SCUMM Wikipedia](https://en.wikipedia.org/wiki/SCUMM)
- Lokala case studies: `docs/studies/lucasfilm-case-study.md`, `docs/studies/lucasfilm-in-depth.md`
