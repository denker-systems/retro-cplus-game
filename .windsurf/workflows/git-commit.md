---
description: Commit och push ändringar med detaljerad information
---

# Git Commit & Push Workflow

## 1. Kolla status
// turbo
`powershell
git status --short
`

## 2. Se detaljerade ändringar
// turbo
`powershell
git diff --stat; Write-Host ""; git diff --name-status
`

## 3. Stage alla ändringar
`powershell
git add -A
`

## 4. Commit med konventionellt format och body
Format: `type(scope): kort beskrivning`

**Types:**
- `feat` - Ny feature
- `fix` - Bugfix
- `refactor` - Omstrukturering
- `docs` - Dokumentation
- `style` - Formatering
- `test` - Tester
- `chore` - Övrigt
- `perf` - Prestandaförbättring
- `content` - Spelinnehåll (JSON data)

**Scopes för detta projekt:**
- `core` - Game, Window, Renderer
- `states` - StateManager, States
- `entities` - Player, NPC, Item
- `systems` - Dialog, Quest, Inventory, Room
- `audio` - AudioManager
- `graphics` - TextureManager, FontManager
- `ui` - UI widgets
- `utils` - Logger, Config
- `data` - JSON data, DataLoader
- `build` - CMake, vcpkg
- `input` - Input handling

**Commit med multi-line body:**
`powershell
git commit -m "feat(scope): kort beskrivning" -m "- Detalj 1" -m "- Detalj 2" -m "- Detalj 3"
`

**Enkelt commit:**
`powershell
git commit -m "fix(player): correct delta time movement"
`

## 5. Visa commit-detaljer
// turbo
`powershell
git log -1 --stat --format="Commit: %h%nAuthor: %an%nDate: %ad%nBranch: $(git branch --show-current)%n%nMessage:%n%B"
`

## 6. Push till remote
`powershell
git push origin $(git branch --show-current)
`

## 7. Visa push-status
// turbo
`powershell
Write-Host "=== Push Complete ===" -ForegroundColor Green; git log origin/$(git branch --show-current)..HEAD --oneline; if ($LASTEXITCODE -eq 0 -and (git log origin/$(git branch --show-current)..HEAD --oneline).Count -eq 0) { Write-Host "All commits pushed!" -ForegroundColor Green }
`

---

## Quick Commit & Push (allt i ett)
`powershell
git add -A; git commit -m "TYPE(SCOPE): MESSAGE" -m "DETAILS"; git push origin $(git branch --show-current)
`

## Se senaste commits innan PR
// turbo
`powershell
git log --oneline -10 --decorate
`
