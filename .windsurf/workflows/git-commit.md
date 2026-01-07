---
description: Committa ändringar med conventional format och dokumentationssynk
auto_execution_mode: 1
---

# Git Commit Workflow

> Komplett commit-process för Retro Engine

## 1. Kontrollera Status

// turbo
```powershell
Write-Host "=== GIT STATUS ===" -ForegroundColor Yellow
git status --short
$staged = git diff --cached --name-only | Measure-Object -Line
$unstaged = git diff --name-only | Measure-Object -Line
Write-Host "`nStaged: $($staged.Lines) files"
Write-Host "Unstaged: $($unstaged.Lines) files"
```

---

## 2. Granska ALLA Ändringar

// turbo
```powershell
Write-Host "`n=== ÄNDRADE FILER ===" -ForegroundColor Yellow
git diff --stat HEAD
```

### Detaljerad Diff
```powershell
# Se exakt vad som ändrats
git diff HEAD

# Se ändringar i specifik fil
git diff HEAD -- src/engine/core/Actor.cpp
```

### KRITISKT: Identifiera Ändringstyper

| Fråga | Svar |
|-------|------|
| Nya filer skapade? | Lista alla |
| Filer borttagna? | Lista alla |
| Filer flyttade? | Lista alla |
| Arkitekturändringar? | Beskriv |
| Nya features? | Beskriv |
| Bugfixes? | Beskriv |
| Breaking changes? | Beskriv |

---

## 3. Stage Ändringar

### Stage Allt
```powershell
git add .
```

### Stage Selektivt (Rekommenderat)
```powershell
# Interaktiv staging - välj hunks
git add -p

# Stage specifika filer
git add src/engine/core/PlayerActor.cpp
git add src/engine/core/PlayerActor.h

# Stage hela katalog
git add src/engine/core/
```

### Unstage Om Felstaged
```powershell
# Unstage en fil
git reset HEAD src/engine/core/WrongFile.cpp

# Unstage allt
git reset HEAD
```

### Verifiera Staging
```powershell
git status
git diff --cached --stat
```

---

## 4. Commit Message Format

### Conventional Commits Standard
```
type(scope): kort beskrivning (max 50 tecken)

[valfri body - detaljerad beskrivning]

[valfri footer - issue references, breaking changes]
```

### Types - Fullständig Lista

| Type | Beskrivning | Exempel |
|------|-------------|---------|
| `feat` | Ny funktionalitet | Ny klass, ny feature |
| `fix` | Buggfix | Korrigerar fel beteende |
| `refactor` | Kodändring utan funktionsändring | Omstrukturering |
| `docs` | Endast dokumentation | README, kommentarer |
| `style` | Formattering, ingen kodändring | Whitespace, semicolons |
| `perf` | Prestandaförbättring | Optimering |
| `test` | Lägga till/ändra tester | Unit tests |
| `build` | Build-system ändringar | CMakeLists.txt |
| `ci` | CI-konfiguration | GitHub Actions |
| `chore` | Underhållsuppgifter | Dependencies |
| `revert` | Återställer commit | Undo previous commit |

### Scopes - Retro Engine Specifika

| Scope | Modul | Exempel |
|-------|-------|---------|
| `core` | `src/engine/core/` | Object, Actor, Component |
| `actors` | `src/engine/actors/` | PlayerActor, NPCActor |
| `components` | `src/engine/components/` | SpriteComponent |
| `world` | `src/engine/world/` | World, Level, Scene |
| `physics` | `src/engine/physics/` | Box2D integration |
| `graphics` | `src/engine/graphics/` | Rendering, textures |
| `audio` | `src/engine/audio/` | SDL2_mixer |
| `data` | `src/engine/data/` | JSON loading |
| `game` | `src/game/` | Game states |
| `editor` | `src/editor/` | ImGui panels |
| `panels` | `src/editor/panels/` | ViewportPanel |
| `commands` | `src/editor/commands/` | Undo/Redo |
| `build` | `CMakeLists.txt` | Build configuration |

---

## 5. Commit Exempel

### Enkel Feature
```powershell
git commit -m "feat(core): add PlayerActor class"
```

### Feature Med Body
```powershell
git commit -m "feat(actors): add PlayerActor with walk area support

- Inherits from CharacterActor
- Adds walk area clamping (minX, maxX, minY, maxY)
- Integrates with SpriteComponent for rendering
- Loads walk area from scenes.json via DataLoader

Implements: Point-and-click movement within walk areas"
```

### Bugfix
```powershell
git commit -m "fix(data): walk area loads correctly from scenes.json

Root cause: JSON field 'walkArea' was not being parsed in DataLoader.
Walk area coordinates were defaulting to 0,0,0,0.

Solution: Added walkArea parsing in DataLoader::loadRoom()

Fixes: #123"
```

### Refactoring
```powershell
git commit -m "refactor(editor): extract ViewportPanel rendering logic

- Moved scene rendering to separate method
- Extracted hotspot rendering
- Improved code organization

No functional changes."
```

### Breaking Change
```powershell
git commit -m "feat(core)!: redesign Component API

BREAKING CHANGE: Component::update() signature changed

Old: void update()
New: void update(float deltaTime)

Migration: Update all component implementations to accept deltaTime parameter.

Affected components:
- SpriteComponent
- AnimationComponent
- All custom components"
```

### Multi-Scope Commit (Undvik om möjligt)
```powershell
git commit -m "feat(core,editor): add Actor selection system

Core:
- Added ActorObject::setSelected() method
- Added selection state to ActorObjectExtended

Editor:
- ViewportPanel shows selection highlight
- Inspector updates on selection change"
```

---

## 6. Dokumentationssynk

### KRITISKT: Uppdatera DEVLOG

Lägg till i `docs/dev/DEVLOG.md`:

```markdown
## YYYY-MM-DD

- `HASH` type(scope): beskrivning
  - **Nya filer:** lista
  - **Ändrade filer:** lista
  - **Feature:** beskrivning
  - **Bugfix:** beskrivning
  - Session: [YYYY-MM-DD](sessions/YYYY-MM-DD.md)
```

### Uppdatera Session Report

Lägg till i `docs/dev/sessions/YYYY-MM-DD.md`:

```markdown
## Commits

| Tid | Hash | Type | Scope | Beskrivning |
|-----|------|------|-------|-------------|
| HH:MM | `abc123` | feat | core | Add PlayerActor |

### Commit Detaljer

#### `abc123` - feat(core): Add PlayerActor
**Nya filer:**
- `src/engine/core/PlayerActor.cpp`
- `src/engine/core/PlayerActor.h`

**Ändrade filer:**
- `CMakeLists.txt` - La till nya källfiler

**Beskrivning:**
[Detaljerad beskrivning av ändringen]
```

---

## 7. Verifiera Commit

// turbo
```powershell
Write-Host "`n=== SENASTE COMMIT ===" -ForegroundColor Yellow
git log -1 --stat
```

### Kontrollera Commit Message
```powershell
git log -1 --format="%B"
```

### Kontrollera Ändrade Filer
```powershell
git show --name-only HEAD
```

---

## 8. Ångra/Ändra Commits

### Ändra Senaste Commit Message
```powershell
git commit --amend -m "feat(core): corrected message"
```

### Lägg Till Glömd Fil Till Senaste Commit
```powershell
git add forgotten-file.cpp
git commit --amend --no-edit
```

### Ångra Senaste Commit (Behåll Ändringar)
```powershell
git reset --soft HEAD~1
# Ändringar är nu staged igen
```

### Ångra Senaste Commit (Ta Bort Ändringar)
```powershell
git reset --hard HEAD~1
# VARNING: Ändringar försvinner permanent!
```

### Ångra Commit Med Ny Commit (Säkrare)
```powershell
git revert HEAD
# Skapar ny commit som reverserar ändringar
```

---

## 9. Push (Endast På Explicit Instruktion)

```powershell
# Push till feature branch
git push origin $(git branch --show-current)

# Om branch inte finns på remote än
git push -u origin $(git branch --show-current)
```

### Force Push (FARLIGT - Endast Feature Branches)
```powershell
# Endast om du ändrat historia på din egen branch
git push --force-with-lease origin feature/my-branch
```

---

## Commit Best Practices

### GÖR
- ✅ Skriv meningsfulla commit messages
- ✅ Håll commits atomära (en logisk ändring)
- ✅ Referera issue-nummer i footer
- ✅ Uppdatera dokumentation med kodändringar
- ✅ Kör build innan commit för att verifiera
- ✅ Granska diff innan commit

### GÖR INTE
- ❌ Commita genererade filer (build/, *.o)
- ❌ Inkludera debug/temp-kod
- ❌ Gör enorma commits med många ändringar
- ❌ Använd vaga messages ("fix stuff", "updates")
- ❌ Commita broken code
- ❌ Force push till delade branches

---

## Commit Checklista

### Innan Commit
- [ ] `git diff` granskat
- [ ] Alla ändringar är avsiktliga
- [ ] Ingen debug-kod kvar
- [ ] Build fungerar
- [ ] Commit message följer format

### Efter Commit
- [ ] `git log -1` verifierat
- [ ] DEVLOG uppdaterad
- [ ] Session report uppdaterad
- [ ] CHANGELOG uppdaterad (om feature/fix)

---

## ⚠️ KRITISKA REGLER

1. **ALDRIG git commit utan explicit instruktion från användaren**
2. **ALDRIG git push utan explicit instruktion från användaren**
3. **ALDRIG git add utan explicit instruktion från användaren**
4. **ALDRIG force push till main/develop**

### Tillåtna Operationer (Read-Only)
```powershell
git status      # ✅ Alltid OK
git log         # ✅ Alltid OK
git diff        # ✅ Alltid OK
git branch      # ✅ Alltid OK
git show        # ✅ Alltid OK
```

### Kräver Explicit Instruktion
```powershell
git add         # ⚠️ Kräver tillåtelse
git commit      # ⚠️ Kräver tillåtelse
git push        # ⚠️ Kräver tillåtelse
git reset       # ⚠️ Kräver tillåtelse
git revert      # ⚠️ Kräver tillåtelse
```
