---
description: Starta en utvecklingssession med full kontextladdning
auto_execution_mode: 1
---

# Init Session Workflow

> Komplett sessioninitiering för Retro Engine

## 1. Session Info och Tidsstämpel

// turbo
```powershell
$today = Get-Date -Format 'yyyy-MM-dd HH:mm'
$dev = $env:USERNAME
Write-Host "============================================" -ForegroundColor Cyan
Write-Host "=== SESSION START: $today ===" -ForegroundColor Cyan
Write-Host "=== Developer: $dev ===" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan
```

---

## 2. Git Status - Kontrollera Working Directory

// turbo
```powershell
Write-Host "`n=== GIT STATUS ===" -ForegroundColor Yellow
git status --short
Write-Host "`nCurrent Branch: $(git branch --show-current)" -ForegroundColor Green
```

### Analysera Status
- **Uncommitted changes?** - Hantera innan nytt arbete
- **Untracked files?** - Lägg till i .gitignore eller stage
- **Merge conflicts?** - Lös innan fortsättning

---

## 3. Senaste Commits - Förstå Kontext

// turbo
```powershell
Write-Host "`n=== SENASTE 10 COMMITS ===" -ForegroundColor Yellow
git log --oneline -10 --format="%h %an: %s (%ar)"
```

### Analysera
- Vem arbetade senast?
- Vilka system påverkades?
- Finns det pågående features?

---

## 4. Commits Senaste 24h - Teamaktivitet

// turbo
```powershell
Write-Host "`n=== COMMITS SENASTE 24H ===" -ForegroundColor Yellow
git log --oneline --since="24 hours ago" --format="%h %an: %s"
```

---

## 5. Ladda Föregående Session Report

// turbo
```powershell
Write-Host "`n=== FÖREGÅENDE SESSION ===" -ForegroundColor Yellow
$latest = Get-ChildItem docs/dev/sessions/*.md -ErrorAction SilentlyContinue | Sort-Object Name -Descending | Select-Object -First 1
if ($latest) { 
    Write-Host "Fil: $($latest.Name)" -ForegroundColor Cyan
    Write-Host "----------------------------------------"
    Get-Content $latest.FullName | Select-Object -First 50
} else {
    Write-Host "Ingen session report hittad" -ForegroundColor Gray
}
```

### Granska
- Vad blev klart förra sessionen?
- Finns det handoff-anteckningar?
- Blockers att vara medveten om?

---

## 6. ROADMAP Progress - Nuvarande Fas

// turbo
```powershell
Write-Host "`n=== ROADMAP ===" -ForegroundColor Yellow
Get-Content docs/ROADMAP.md -ErrorAction SilentlyContinue | Select-Object -First 60
```

### Identifiera
- Vilken fas är projektet i?
- Vilka uppgifter är pågående?
- Vad är nästa prioritet?

---

## 7. CHANGELOG - Senaste Ändringar

// turbo
```powershell
Write-Host "`n=== CHANGELOG (Unreleased) ===" -ForegroundColor Yellow
Get-Content docs/CHANGELOG.md -ErrorAction SilentlyContinue | Select-Object -First 40
```

---

## 8. Build Status - Verifiera Att Allt Fungerar

// turbo
```powershell
Write-Host "`n=== BUILD CHECK ===" -ForegroundColor Yellow
cd build
cmake --build . --config Release 2>&1 | Select-Object -Last 10
if ($LASTEXITCODE -eq 0) {
    Write-Host "`n✓ BUILD SUCCESSFUL" -ForegroundColor Green
} else {
    Write-Host "`n✗ BUILD FAILED - Fix before continuing!" -ForegroundColor Red
}
```

### Om Build Misslyckas
1. Kör `git pull` för att hämta senaste ändringar
2. Kör full rebuild om nödvändigt
3. Kontakta senaste developer om problemet kvarstår

---

## 9. Kör Game och Editor - Smoke Test

```powershell
# Starta spelet för snabb verifiering
.\Release\RetroGame.exe

# Starta editorn för att verifiera funktionalitet
.\Release\RetroEditor.exe
```

### Verifiera
- [ ] Spelet startar utan crash
- [ ] Editorn startar utan crash
- [ ] Grundläggande funktionalitet fungerar

---

## 10. Skapa Dagens Session Report

Skapa `docs/dev/sessions/YYYY-MM-DD.md`:

```markdown
# Session YYYY-MM-DD

**Developer:** [Namn]
**Branch:** [branch-name]
**Start:** HH:MM
**Focus:** [Dagens huvuduppgift]

---

## Sessionsmål

### Primära Mål
- [ ] Mål 1 - [Beskrivning]
- [ ] Mål 2 - [Beskrivning]

### Sekundära Mål
- [ ] Mål 3 - [Beskrivning]

---

## Progress Log

### HH:MM - Session Start
- Git status: [antal uncommitted files]
- Branch: [branch-name]
- Build status: [OK/FAIL]
- Senaste commit: [hash] [beskrivning]

### HH:MM - [Aktivitet]
- [Vad gjordes]
- [Resultat]

---

## Commits Denna Session

| Tid | Hash | Type | Scope | Beskrivning |
|-----|------|------|-------|-------------|
| HH:MM | `abc123` | feat | core | Beskrivning |

---

## Tekniska Beslut

### Beslut 1: [Titel]
- **Kontext:** Varför behövdes beslutet?
- **Beslut:** Vad beslutades?
- **Konsekvenser:** Påverkan på systemet

---

## Blockers och Problem

| Problem | Status | Lösning |
|---------|--------|---------|
| [Problem] | [Löst/Pågående/Blockerad] | [Hur det löstes] |

---

## Kodändringar

### Nya Filer
- `path/to/file.cpp` - [Beskrivning]

### Modifierade Filer
- `path/to/file.cpp` - [Vad ändrades]

### Borttagna Filer
- `path/to/old-file.cpp` - [Varför borttagen]

---

## Handoff Notes

### Nuvarande Status
- [Beskriv vad som är klart och vad som återstår]

### Nästa Prioritet
- [Vad bör nästa developer fokusera på]

### Varningar
- [Saker att vara medveten om]

---

## Session Slut

**Slut:** HH:MM
**Commits:** [antal]
**Lines Changed:** +X / -Y
```

---

## 11. Identifiera Dagens Arbete

### Baserat på:
1. **ROADMAP** - Vilken uppgift är nästa i ordningen?
2. **Issues** - Finns tilldelade issues?
3. **Handoff** - Vad lämnade föregående developer?
4. **Blockers** - Finns kritiska buggar att fixa?

### Prioriteringsordning
1. 🔴 **Blockers** - Kritiska buggar som stoppar arbete
2. 🟠 **Hotfixes** - Produktion-kritiska fixes
3. 🟡 **Current Sprint** - Planerade uppgifter
4. 🟢 **Backlog** - Framtida uppgifter

---

## Session Start Checklista

### Obligatoriskt
- [ ] Git status kontrollerad
- [ ] Inga kritiska uncommitted changes
- [ ] Build fungerar (Release)
- [ ] Game startar utan crash
- [ ] Editor startar utan crash

### Rekommenderat
- [ ] Föregående session report läst
- [ ] ROADMAP progress kontrollerad
- [ ] CHANGELOG granskad
- [ ] Dagens session report skapad
- [ ] Dagens mål identifierade

---

## ⚠️ VIKTIGA REGLER

1. **ALDRIG git commit/push utan explicit instruktion**
2. **Dokumentera ALLA betydande ändringar**
3. **Kör build efter varje större ändring**
4. **Uppdatera session report löpande**

---

## Quick Reference

### Retro Engine Struktur
```
src/
├── engine/         # RetroCore - Statiskt bibliotek
│   ├── core/       # Object, Actor, Component
│   ├── components/ # SpriteComponent, etc.
│   ├── world/      # World, Level, Scene
│   ├── physics/    # Box2D integration
│   └── audio/      # SDL2_mixer
├── game/           # RetroGame - Spelexekverbar
│   └── states/     # MenuState, PlayState
└── editor/         # RetroEditor - Editor
    ├── panels/     # ImGui panels
    └── commands/   # Undo/Redo
```

### Vanliga Kommandon
```powershell
# Build
cd build; cmake --build . --config Release

# Kör Game
.\Release\RetroGame.exe

# Kör Editor
.\Release\RetroEditor.exe

# Git status
git status --short

# Senaste commits
git log --oneline -5
```
