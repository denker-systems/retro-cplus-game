---
description: Komplett PR-skapande för Retro Engine
auto_execution_mode: 1
---

# Generate PR Workflow

> Enterprise Pull Request process för Retro Engine

## 1. Verifiera Branch Status

// turbo
```powershell
Write-Host "============================================" -ForegroundColor Cyan
Write-Host "=== PR PREPARATION ===" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan

$branch = git branch --show-current
$commits = git rev-list --count main..HEAD
Write-Host "`nBranch: $branch"
Write-Host "Commits: $commits"
```

---

## 2. Lista Alla Commits

// turbo
```powershell
Write-Host "`n=== COMMITS I DENNA BRANCH ===" -ForegroundColor Yellow
git log main..HEAD --oneline --reverse
```

### Analysera Commits

| Commit | Type | Scope | Breaking? |
|--------|------|-------|-----------|
| abc123 | feat | core | Nej |
| def456 | fix | data | Nej |

---

## 3. Kolla Diff Statistik

// turbo
```powershell
Write-Host "`n=== ÄNDRINGAR ===" -ForegroundColor Yellow
git diff main...HEAD --stat
$lines = git diff main...HEAD --numstat | ForEach-Object { $_.Split("`t")[0..1] } | Measure-Object -Sum
Write-Host "`nTotalt ändrade rader: ~$($lines.Sum)"
```

---

## 4. Pre-PR Verifiering

### 4.1 Build Check
// turbo
```powershell
Write-Host "`n=== BUILD CHECK ===" -ForegroundColor Yellow
cd build
cmake --build . --config Release
if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ Build PASSED" -ForegroundColor Green
} else {
    Write-Host "✗ Build FAILED - Fix before PR!" -ForegroundColor Red
}
```

### 4.2 Kör Tester
```powershell
ctest -C Release --output-on-failure
```

### 4.3 Kontrollera för Debug-kod
```powershell
Write-Host "`n=== DEBUG CODE CHECK ===" -ForegroundColor Yellow
$debug = git diff main...HEAD | Select-String "TODO|FIXME|std::cout.*DEBUG|console.log"
if ($debug) {
    Write-Host "⚠ Potentiell debug-kod hittad:" -ForegroundColor Yellow
    $debug | ForEach-Object { Write-Host "  $_" }
} else {
    Write-Host "✓ Ingen debug-kod hittad" -ForegroundColor Green
}
```

### 4.4 Self-Review
```powershell
# Granska alla ändringar
git diff main...HEAD
```

---

## 5. PR Beskrivning Template

### 5.1 Standard PR Template

```markdown
## Sammanfattning
[Kort beskrivning av vad denna PR gör - max 2-3 meningar]

## Relaterad Issue
Closes #XXX

---

## Typ av Ändring

- [ ] ✨ **Feature** - Ny funktionalitet
- [ ] 🐛 **Bugfix** - Fix av ett problem
- [ ] 🔧 **Refactoring** - Kodändring utan funktionsändring
- [ ] 📚 **Documentation** - Endast dokumentation
- [ ] ⚡ **Performance** - Prestandaförbättring
- [ ] 🧪 **Tests** - Lägga till/ändra tester
- [ ] 💥 **Breaking Change** - Ändring som bryter bakåtkompatibilitet

---

## Ändringar

### Added
- `NewClass` - [Beskrivning]
- `newMethod()` i `ExistingClass` - [Beskrivning]

### Changed
- `ExistingClass` - [Vad och varför]

### Fixed
- [Bug som fixades]

### Removed
- [Vad togs bort och varför]

---

## Commits

| Hash | Type | Scope | Beskrivning |
|------|------|-------|-------------|
| `abc123` | feat | core | Add NewActor |
| `def456` | fix | data | Fix JSON loading |

---

## Implementation Details

### Arkitektur
[Beskriv eventuella arkitekturbeslut]

### Nya Filer
- `src/engine/actors/NewActor.h` - Header
- `src/engine/actors/NewActor.cpp` - Implementation

### Modifierade Filer
- `CMakeLists.txt` - Lade till nya filer
- `src/engine/world/Scene.cpp` - Integrerade NewActor

---

## Testning

### Automatiska Tester
- [ ] Alla befintliga tester passerar
- [ ] Nya unit tests tillagda (om relevant)

### Manuell Testning
- [x] Testat i RetroGame.exe
- [x] Testat i RetroEditor.exe
- [x] Verifierat inga regressioner

### Teststeg
1. Starta spelet
2. [Specifika steg för att testa featuren]
3. Verifiera [förväntat resultat]

---

## Screenshots (Om Visuella Ändringar)

| Före | Efter |
|------|-------|
| [screenshot] | [screenshot] |

---

## Dokumentation

- [x] Kod dokumenterad med Doxygen
- [x] CHANGELOG uppdaterad
- [ ] README uppdaterad (om nödvändigt)
- [ ] API docs uppdaterade (om public API)

---

## Performance Impact

- [ ] Ingen påverkan
- [ ] Testat: [resultat]

---

## Checklista

### Kod
- [ ] Följer coding standards (PascalCase, m_ prefix, etc.)
- [ ] Ingen debug-kod kvar
- [ ] Ingen utkommenterad kod
- [ ] Inga TODOs utan issue-referens

### Build
- [ ] Kompilerar utan errors
- [ ] Inga nya warnings

### Test
- [ ] Smoke test godkänt
- [ ] Inga regressioner

### Docs
- [ ] Doxygen-dokumentation komplett
- [ ] CHANGELOG uppdaterad

---

## Reviewer Notes

[Speciella instruktioner för reviewers, t.ex.:]
- Fokusera på X
- Y är medvetet designat så här p.g.a. Z
- Fråga om Z är oklart
```

### 5.2 Minimal PR Template (Små Ändringar)

```markdown
## Sammanfattning
[Kort beskrivning]

## Typ
- [x] Bugfix

## Ändringar
- Fixed [bug description]

## Testning
- [x] Verifierat lokalt

## Checklista
- [x] Build OK
- [x] Ingen debug-kod
```

---

## 6. Skapa PR

### 6.1 Push Branch

```powershell
# Pusha till remote
git push origin $(git branch --show-current)

# Om första push till denna branch
git push -u origin $(git branch --show-current)
```

### 6.2 Skapa PR på GitHub

1. **Navigera till GitHub:**
   ```
   https://github.com/[owner]/retro-cplus-game/compare/main...[branch-name]
   ```

2. **Fyll i PR Template:**
   - Titel: `feat(scope): description` (samma format som commit)
   - Body: Kopiera template ovan och fyll i

3. **Lägg till Labels:**
   - `needs-review`
   - `feature` / `bugfix` / `refactor`
   - Prioritet: `p1` / `p2` / `p3`
   - Storlek: `size/S` / `size/M` / `size/L`

4. **Request Reviewers:**
   - Minst 1 peer review
   - 2 reviews för arkitekturändringar

5. **Skapa PR**

### 6.3 Via GitHub CLI (Valfritt)

```powershell
gh pr create `
  --title "feat(scope): description" `
  --body-file PR_DESCRIPTION.md `
  --base main `
  --head $(git branch --show-current) `
  --label "needs-review" `
  --label "feature"
```

---

## 7. Reviewer Requirements

### Standard Ändringar
- **1 approval** från team member
- Alla CI checks gröna

### Arkitektur Ändringar
- **2 approvals** (minst 1 senior)
- Arkitektur-diskussion genomförd

### Core/Critical Ändringar
- **2 approvals** från core team
- Extra testning dokumenterad

---

## 8. PR Storlek Guidelines

| Storlek | Rader | Review Tid | Rekommendation |
|---------|-------|------------|----------------|
| **XS** | < 50 | 15 min | ✅ Ideal |
| **S** | 50-200 | 30 min | ✅ Bra |
| **M** | 200-400 | 1 timme | ⚠️ OK |
| **L** | 400-800 | 2 timmar | ⚠️ Överväg split |
| **XL** | > 800 | 4+ timmar | ❌ Dela upp |

### Hur dela upp stor PR

1. **Separera refactoring** från feature-ändringar
2. **Infrastruktur först** - CMake, headers
3. **Core implementation** - Huvudlogik
4. **Integration** - Hook in i befintliga system
5. **Tests och docs** - Separat om stor

---

## 9. Efter PR Skapad

### 9.1 Notifiera Team

```markdown
📝 **PR Ready for Review**

**Titel:** feat(core): Add NewActor with walk area support
**PR:** #123
**Branch:** feature/new-actor
**Storlek:** M (~250 rader)

**Sammanfattning:**
Ny NewActor-klass med stöd för walk area och integration med Scene.

**Reviewers:** @reviewer1, @reviewer2
```

### 9.2 Länka i Issue

Lägg till kommentar i relaterad issue:
```
PR created: #123
```

### 9.3 Övervaka CI

Kontrollera att CI/CD pipeline passerar:
- Build
- Tests
- Linting

---

## 10. Hantera Review Feedback

### 10.1 Svara på Kommentarer

- **Adressera alla kommentarer** - Svara eller gör ändringar
- **Förklara beslut** - Om du inte håller med, förklara varför
- **Var snabb** - Svara inom 24 timmar

### 10.2 Göra Ändringar

```powershell
# Gör begärda ändringar
# ...

# Stage och commit
git add .
git commit -m "fix: address review feedback

- Changed X as suggested
- Fixed Y
- Kept Z because [reason]"

# Push
git push origin $(git branch --show-current)
```

### 10.3 Re-request Review

Efter ändringar, klicka "Re-request review" på GitHub.

---

## PR Checklista

### Innan PR
- [ ] Branch uppdaterad med main
- [ ] Build fungerar
- [ ] Tester passerar
- [ ] Self-review genomförd
- [ ] Ingen debug-kod
- [ ] Dokumentation uppdaterad
- [ ] CHANGELOG uppdaterad

### PR Skapad
- [ ] Titel följer konvention
- [ ] Beskrivning komplett
- [ ] Labels tillagda
- [ ] Reviewers tilldelade
- [ ] Issue länkad

### Efter Review
- [ ] Alla kommentarer adresserade
- [ ] CI passerar
- [ ] Approvals mottagna

---

## ⚠️ VIKTIGA REGLER

1. **ALDRIG push utan explicit instruktion**
2. **ALDRIG merge utan godkännande**
3. **ALDRIG force push till shared branches**
4. **ALLTID testa innan PR**
5. **ALLTID uppdatera dokumentation**
