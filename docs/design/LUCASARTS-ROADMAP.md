# LucasArts-inspirerade Utökningar - Roadmap

> Prioriterad utvecklingsplan baserad på case study-analysen.

---

## Översikt

```
┌─────────────────────────────────────────────────────────────────────────┐
│ Fas A: Anti-Frustration │ Fas B: Immersion  │ Fas C: Polish & Feel     │
│ ░░░░░░░░░░░░░░░░░░░░░░ │ ░░░░░░░░░░░░░░░░░ │ ░░░░░░░░░░░░░░░░░░░░░░░ │
│ HintSystem, Reaktivitet │ Adaptiv musik     │ Alternativa lösningar    │
│ 2-3 sessioner           │ 2-3 sessioner     │ 3-4 sessioner            │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## Fas A: Anti-Frustration (Prioritet: HÖG)

*Gilbert-principen: "Spelaren ska aldrig fastna och tappa storyn"*

### A.1 Tiered Hint System
**Tid:** 1 session | **Komplexitet:** Medium

- [ ] `HintSystem` singleton
- [ ] 4-nivås hints (Goal → Location → Method → Solution)
- [ ] Cooldown mellan hint-requests
- [ ] Integration med QuestSystem
- [ ] UI-knapp ("?" eller H-tangent)

```cpp
// Exempel-användning
HintSystem::instance().requestHint("find_library_key");
// Nivå 1: "Jag behöver hitta ett sätt in i biblioteket."
// Nivå 2: "Någon i tavernan kanske vet något."
// Nivå 3: "Bartendern verkar veta mer än han säger..."
// Nivå 4: "Prata med bartendern och fråga om bibliotekarien."
```

### A.2 Reaktivitet & Funny Fails
**Tid:** 1 session | **Komplexitet:** Låg

- [ ] Utöka `Hotspot` med `funnyFails` vektor
- [ ] Lägg till reaktiva repliker i JSON
- [ ] "Reward Intent" logik (nästan-rätt → hint istället för fail)
- [ ] Minst 2 reaktioner per interagerbart objekt

```json
{
  "id": "locked_door",
  "funnyFails": [
    "Den är låst. Överraskande, eller hur?",
    "Jag kan inte bara banka på den som en vilde.",
    "Om bara jag hade en nyckel... eller en rambock."
  ]
}
```

### A.3 Snabbresa (Anti-Backtracking)
**Tid:** 0.5 session | **Komplexitet:** Låg

- [ ] Map-knapp (M) som visar besökta rum
- [ ] Klicka för instant-travel
- [ ] Endast till rum utan aktiva "blockers"

---

## Fas B: Immersion (Prioritet: MEDIUM)

*iMUSE-principen: "Musik ska regissera, inte bara loopa"*

### B.1 Adaptiv Musik - Grundläggande
**Tid:** 1-2 sessioner | **Komplexitet:** Medium

- [ ] `MusicState` enum (Normal, Tense, Discovery, Victory)
- [ ] Rum-baserade teman
- [ ] Crossfade mellan states
- [ ] "Sting" vid pussel-lösning

```cpp
enum class MusicState {
    Normal,      // Standard rum-tema
    Tense,       // Nära lösning / fara
    Discovery,   // Hittade något viktigt
    Victory,     // Löste pussel
    Cutscene     // Speciell sekvens
};
```

### B.2 Dialog Tone Preview
**Tid:** 1 session | **Komplexitet:** Medium

- [ ] Visa intention istället för exakt replik i val
- [ ] Ikoner för ton (vänlig/arg/sarkastisk)
- [ ] Undvik "Mass Effect-problemet"

```
Istället för: "Ge mig nyckeln nu, din idiot!"
Visa:        [🔥 Kräv nyckeln aggressivt]
```

### B.3 Recap Barks
**Tid:** 0.5 session | **Komplexitet:** Låg

- [ ] Karaktären sammanfattar situationen när spelaren är inaktiv
- [ ] "Jag borde nog prata med bartendern..."
- [ ] Triggas efter 60 sek inaktivitet

---

## Fas C: Polish & Feel (Prioritet: SENARE)

*Fate of Atlantis-principen: "Ge spelaren val"*

### C.1 Alternativa Pussel-lösningar
**Tid:** 2-3 sessioner | **Komplexitet:** Hög

- [ ] Designa 2-3 "approach lanes" per major gate
- [ ] Social / Logik / Item-baserade vägar
- [ ] Gate-system i QuestSystem

```
Gate: "Komma in i biblioteket"
├── Social:  Övertyga vakten (dialog-träd)
├── Logik:   Hitta hemlig ingång (utforska källaren)
└── Item:    Få lånekort (hjälp bibliotekarien)
```

### C.2 Journal/Recap System
**Tid:** 1 session | **Komplexitet:** Medium

- [ ] Automatisk loggning av viktiga händelser
- [ ] "Vad vet jag?" sektion
- [ ] "Vad ska jag göra?" sektion
- [ ] Sökbar/filterbar

### C.3 Komedi-timing i UI
**Tid:** 0.5 session | **Komplexitet:** Låg

- [ ] Snabbare dialog-framsteg (klick = nästa)
- [ ] Skip-knapp för redan sedd dialog
- [ ] Paus före punchlines (timing)

---

## Implementeringsordning

```
Session N:   [A.1] HintSystem grundstruktur
Session N+1: [A.2] Reaktivitet & funnyFails
Session N+2: [B.1] Adaptiv musik (basic)
Session N+3: [A.3] Snabbresa + [B.3] Recap barks
Session N+4: [B.2] Dialog tone preview
Session N+5: [C.1] Alternativa lösningar (design)
Session N+6: [C.1] Alternativa lösningar (implementation)
Session N+7: [C.2] Journal system
Session N+8: [C.3] UI polish
```

---

## Mätbara Mål

| Feature | Mål | Mätning |
|---------|-----|---------|
| HintSystem | <5 min stuck time | Playtest feedback |
| Reaktivitet | 2+ svar per objekt | Content audit |
| Adaptiv musik | Sömlösa övergångar | Lyssna-test |
| Alt. lösningar | 2+ vägar per gate | Design review |

---

## Definition of Done

### Fas A klar när:
- [ ] Spelaren kan få hints utan att känna sig dum
- [ ] Varje objekt har minst ett roligt "fail"-svar
- [ ] Snabbresa fungerar mellan alla besökta rum

### Fas B klar när:
- [ ] Musiken ändras baserat på situation
- [ ] Spelaren vet vad dialog-val innebär
- [ ] Karaktären "tänker högt" vid inaktivitet

### Fas C klar när:
- [ ] Minst 1 major gate har 2+ lösningar
- [ ] Journal visar spelarens kunskap
- [ ] Dialog känns snabb och rolig

---

## Referenser

- `docs/design/lucasarts-insights-applied.md` - Detaljerad analys
- `docs/studies/lucasfilm-case-study.md` - Grundstudie
- `docs/studies/lucasfilm-in-depth.md` - Djupstudie
- [Ron Gilbert: Why Adventure Games Suck](https://grumpygamer.com/why_adventure_games_suck/)
