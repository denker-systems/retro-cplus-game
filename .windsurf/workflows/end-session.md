---
description: Avsluta utvecklingssession med full dokumentationssynk
auto_execution_mode: 1
---

# End Session Workflow

> Komplett sessionsavslutning för Retro Engine

## 1. Session Info

// turbo
```powershell
$today = Get-Date -Format 'yyyy-MM-dd'
$time = Get-Date -Format 'HH:mm'
Write-Host "============================================" -ForegroundColor Cyan
Write-Host "=== SESSION END: $today $time ===" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan
```

---

## 2. Kontrollera Uncommitted Changes

// turbo
```powershell
Write-Host "`n=== UNCOMMITTED CHANGES ===" -ForegroundColor Yellow
git status --short
$changes = git status --porcelain | Measure-Object -Line
Write-Host "`nAntal ändrade filer: $($changes.Lines)" -ForegroundColor Cyan
```

### Om Uncommitted Changes Finns
- **Staged changes?** - Överväg commit eller unstage
- **Modified files?** - Spara eller discard
- **Untracked files?** - Lägg till i .gitignore eller stage

⚠️ **VARNING:** Lämna ALDRIG kritiska ändringar uncommitted!

---

## 3. Lista Dagens Commits

// turbo
```powershell
Write-Host "`n=== COMMITS IDAG ===" -ForegroundColor Yellow
git log --oneline --since="today 00:00" --format="%h %s (%ar)"
$commits = git log --oneline --since="today 00:00" | Measure-Object -Line
Write-Host "`nAntal commits idag: $($commits.Lines)" -ForegroundColor Cyan
```

---

## 4. Analysera ALLA Commits

**KRITISKT:** För VARJE commit denna session, kör:

```powershell
git show --stat <HASH>
```

### Dokumentera för Varje Commit

| Fråga | Svar |
|-------|------|
| Vilka filer ändrades? | Lista alla |
| Nya filer/klasser skapade? | Ja/Nej - vilka? |
| Filer flyttade/borttagna? | Ja/Nej - vilka? |
| Arkitekturändringar? | Ja/Nej - beskriv |
| Nya features? | Ja/Nej - beskriv |
| Bugfixes? | Ja/Nej - beskriv |
| Build-ändringar? | Ja/Nej - beskriv |

### Exempel på Analys
```powershell
# Commit abc123
git show --stat abc123

# Output:
#  src/engine/core/PlayerActor.cpp | 150 ++++++++++++
#  src/engine/core/PlayerActor.h   |  45 ++++
#  CMakeLists.txt                  |   2 +
#  3 files changed, 197 insertions(+)
```

**Dokumentation:**
- **Nya filer:** `PlayerActor.cpp`, `PlayerActor.h`
- **Feature:** Ny PlayerActor-klass med walk area support
- **Build:** CMakeLists.txt uppdaterad med nya filer

---

## 5. Uppdatera Session Report

Slutför `docs/dev/sessions/YYYY-MM-DD.md`:

```markdown
## Session Sammanfattning

**Duration:** X timmar
**Commits:** Y st
**Lines Changed:** +A / -B

---

## Utfört Arbete

### Klart
- [x] Uppgift 1 - [Beskrivning]
- [x] Uppgift 2 - [Beskrivning]

### Delvis Klart
- [ ] Uppgift 3 - [Status: XX% klart]

### Ej Påbörjat
- [ ] Uppgift 4 - [Varför ej påbörjat]

---

## Commits Denna Session

| Tid | Hash | Type | Scope | Beskrivning |
|-----|------|------|-------|-------------|
| 09:15 | `abc123` | feat | core | Added PlayerActor with walk area |
| 11:30 | `def456` | fix | world | Fixed walk area loading from JSON |
| 14:45 | `ghi789` | refactor | editor | Cleaned up ViewportPanel code |

### Commit Detaljer

#### `abc123` - feat(core): Added PlayerActor with walk area
**Nya filer:**
- `src/engine/core/PlayerActor.cpp` - Implementering
- `src/engine/core/PlayerActor.h` - Header

**Ändrade filer:**
- `CMakeLists.txt` - La till nya källfiler

**Beskrivning:**
Ny PlayerActor-klass som ärver från CharacterActor.
Hanterar walk area clamping och movement.

---

## Tekniska Beslut

### Beslut: Använde CharacterActor som basklass
- **Kontext:** Behövde återanvändbar movement-logik
- **Alternativ:** Kunde skapat helt ny klass
- **Beslut:** Ärva från CharacterActor för kod-återanvändning
- **Konsekvenser:** Måste följa CharacterActor's API

---

## Problem och Lösningar

| Problem | Root Cause | Lösning |
|---------|------------|---------|
| Walk area laddades inte | JSON field saknades i loader | La till parsing i DataLoader |

---

## Handoff Notes

### Nuvarande Status
- PlayerActor implementerad och fungerar
- Walk area integration klar
- Editor visar walk area korrekt

### Nästa Prioritet
1. Implementera NPCActor
2. Lägga till pathfinding
3. Integrera med DialogSystem

### Varningar
- `RoomToSceneConverter` behöver uppdateras för nya actor-typer
- Walk area coordinates är i world space, inte local space

### Öppna Frågor
- Ska NPCActor ha egen walk area eller dela med PlayerActor?

---

## Session Slut

**Slut:** HH:MM
**Total tid:** X timmar Y minuter
```

---

## 6. Uppdatera DEVLOG

Lägg till i `docs/dev/DEVLOG.md`:

```markdown
## YYYY-MM-DD

### [Developer Name] - Session X

#### Commits
- `abc123` feat(core): Added PlayerActor with walk area
  - Ny PlayerActor-klass med movement
  - Walk area clamping implementerat
  - Se: [Session Report](sessions/YYYY-MM-DD.md)

- `def456` fix(world): Fixed walk area loading from JSON
  - Root cause: JSON field parsades inte
  - Lösning: La till parsing i DataLoader

#### Sammanfattning
- Implementerade PlayerActor med full walk area support
- Fixade bug där walk area inte laddades från JSON
- Förberedde för NPCActor implementation

#### Nästa Steg
- [ ] NPCActor implementation
- [ ] Pathfinding integration
```

---

## 7. Uppdatera CHANGELOG

Under `[Unreleased]` i `docs/CHANGELOG.md`:

```markdown
## [Unreleased]

### Added
- `PlayerActor` - Spelarkontrollerad karaktär med walk area support
- Walk area clamping i CharacterActor
- Editor visar walk area med grön ram

### Changed
- `CharacterActor` har nu walk area members
- `DataLoader` parsar walk area från JSON

### Fixed
- Walk area laddas nu korrekt från scenes.json
- Player spawn position respekteras vid rumsbyte

### Technical
- Ny actor-hierarki: Object → ActorObject → CharacterActor → PlayerActor
```

---

## 8. Uppdatera ROADMAP

Markera klara uppgifter i `docs/ROADMAP.md`:

```markdown
## Fas 5: Actor System

### 5A: Core Classes ✅
- [x] Object base class
- [x] ActorObject med transform
- [x] ActorObjectExtended med components

### 5B: Character Actors ⏳
- [x] CharacterActor base class
- [x] PlayerActor implementation
- [ ] NPCActor implementation (0%)
- [ ] EnemyActor implementation (0%)

### 5C: Component System ⏳
- [x] SpriteComponent rendering
- [x] AnimationComponent playback
- [ ] CollisionComponent (0%)
- [ ] DialogComponent (0%)

### Progress: 60% complete
```

---

## 9. Skapa ADR (Om Arkitekturbeslut Gjordes)

Om betydande arkitekturbeslut gjordes, skapa `docs/adr/NNN-title.md`:

```markdown
# ADR-XXX: [Beslut Titel]

## Status
Accepted

## Datum
YYYY-MM-DD

## Kontext
[Varför behövdes detta beslut?]
[Vilka problem eller behov uppstod?]

## Beslutsalternativ

### Alternativ 1: [Namn]
- **Fördelar:** X, Y, Z
- **Nackdelar:** A, B, C

### Alternativ 2: [Namn]
- **Fördelar:** X, Y, Z
- **Nackdelar:** A, B, C

## Beslut
[Vilket alternativ valdes och varför?]

## Konsekvenser

### Positiva
- [Fördel 1]
- [Fördel 2]

### Negativa
- [Nackdel 1]
- [Nackdel 2]

### Neutrala
- [Påverkan som varken är positiv eller negativ]

## Relaterat
- [ADR-XXX](XXX-related-decision.md)
- [Issue #XXX](link)
```

---

## 10. Verifiera Build

// turbo
```powershell
Write-Host "`n=== FINAL BUILD CHECK ===" -ForegroundColor Yellow
cd build
cmake --build . --config Release 2>&1 | Select-Object -Last 10
if ($LASTEXITCODE -eq 0) {
    Write-Host "`n✓ BUILD SUCCESSFUL - Ready for handoff" -ForegroundColor Green
} else {
    Write-Host "`n✗ BUILD FAILED - Fix before ending session!" -ForegroundColor Red
}
```

### Om Build Misslyckas
1. **FIXA PROBLEMET** - Lämna aldrig broken build
2. Kör `cmake --build . --config Release` igen
3. Om du inte kan fixa, dokumentera problemet i handoff notes

---

## 11. Smoke Test

```powershell
# Verifiera att spelet fungerar
.\Release\RetroGame.exe

# Verifiera att editorn fungerar
.\Release\RetroEditor.exe
```

### Kontrollera
- [ ] Game startar utan crash
- [ ] Editor startar utan crash
- [ ] Nya features fungerar
- [ ] Inga uppenbara regressioner

---

## 12. Final Checklist

### Dokumentation
- [ ] Session report slutförd
- [ ] DEVLOG uppdaterad med alla commits
- [ ] CHANGELOG uppdaterad (om features/fixes)
- [ ] ROADMAP uppdaterad (om milestones)
- [ ] ADR skapad (om arkitekturbeslut)

### Kod
- [ ] Build fungerar (Release)
- [ ] Smoke test godkänt
- [ ] Inga kritiska uncommitted changes
- [ ] Kod följer standards

### Handoff
- [ ] Handoff notes skrivna
- [ ] Nästa prioritet identifierad
- [ ] Varningar dokumenterade
- [ ] Öppna frågor listade

---

## 13. Annonsera Session Slut

```powershell
Write-Host "`n============================================" -ForegroundColor Green
Write-Host "=== SESSION COMPLETE ===" -ForegroundColor Green
Write-Host "============================================" -ForegroundColor Green
$commits = git log --oneline --since="today 00:00" | Measure-Object -Line
Write-Host "Commits today: $($commits.Lines)"
Write-Host "Build status: OK"
Write-Host "Ready for handoff: YES"
```

---

## ⚠️ VIKTIGA REGLER

1. **ALDRIG git commit/push utan explicit instruktion**
2. **LÄMNA ALDRIG broken build**
3. **DOKUMENTERA ALLT** - Nästa developer ska förstå vad som gjordes
4. **HANDOFF NOTES ÄR OBLIGATORISKA**

---

## Session Metrics

### Vad Ska Rapporteras

| Metric | Hur |
|--------|-----|
| Session duration | Start → Slut tid |
| Commits | `git log --since="today"` |
| Lines changed | `git diff --stat HEAD~X` |
| Files changed | `git diff --name-only HEAD~X` |
| Tests passing | `ctest -C Release` |
| Build time | Notera vid build |

### Exempel Output
```
Session: 2026-01-07
Duration: 4h 30m
Commits: 5
Lines: +450 / -120
Files: 12 changed
Build: OK (45s)
Tests: 24/24 passing
```
