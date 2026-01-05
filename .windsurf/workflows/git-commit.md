---
description: Commit ändringar till git med konventionell commit-format
---

# Git Commit Workflow

## 1. Kolla status
// turbo
`powershell
git status --short
`

## 2. Se ändringar
// turbo
`powershell
git diff --stat
`

## 3. Stage ändringar
`powershell
git add .
`

## 4. Commit med konventionellt format
Format: `type(scope): description`

**Types:**
- `feat` - Ny feature
- `fix` - Bugfix
- `refactor` - Omstrukturering
- `docs` - Dokumentation
- `style` - Formatering
- `test` - Tester
- `chore` - Övrigt

**Scopes för detta projekt:**
- `core` - Game, Window, Renderer
- `states` - StateManager, MenuState
- `entities` - Player, NPC, Item
- `systems` - Dialog, Quest, Inventory
- `audio` - AudioManager
- `ui` - UI widgets
- `build` - CMake, vcpkg

**Exempel:**
`powershell
git commit -m "feat(states): add StateManager with push/pop"
git commit -m "fix(player): correct delta time movement"
git commit -m "docs: update ROADMAP with milestones"
`

## 5. Push (valfritt)
`powershell
git push origin main
`
