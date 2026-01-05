---
description: Uppdaterar roadmap och markerar features som klara
---

# Update Roadmap Workflow

## 1. Öppna roadmap
`powershell
code docs/ROADMAP.md
`

## 2. Markera feature som klar
Ändra `[ ]` till `[x]` för avslutade features.

## 3. Uppdatera progress-bar
`
Fas 1: Core          40%
`

## 4. Lägg till nya upptäckter
Om nya features behövs, lägg till i relevant fas.

## 5. Commit
`powershell
git add docs/ROADMAP.md
git commit -m "docs: update roadmap progress"
`

## Milestone Checklist
- [ ] M1: Window + rendering 
- [ ] M2: State system + meny
- [ ] M3: Rum med spelare
- [ ] M4: Dialog med NPC
- [ ] M5: Inventory fungerar
- [ ] M6: Quest system
- [ ] M7: Save/Load
- [ ] M8: Playable demo
