---
description: Update roadmap and mark features as complete
---

# Update Roadmap Workflow

> Uppdatera utvecklingsplan

## 1. Check Current Progress
// turbo
```powershell
Get-Content docs/ROADMAP.md | Select-Object -First 50
```

## 2. Mark Feature Complete
Ändra `[ ]` till `[x]` för avslutade features.

## 3. Update Progress
```markdown
### Phase 1: Core Foundation [60%]
- [x] SDL2 setup
- [x] Game loop
- [ ] StateManager   nästa
```

## 4. Add New Tasks
Om nya features behövs, lägg till i relevant fas.

---

## Milestone Checklist

| Milestone | Status |
|-----------|--------|
| M1: Window + rendering |  |
| M2: State system |  |
| M3: Room with player |  |
| M4: Dialog with NPC |  |
| M5: Inventory |  |
| M6: Quest system |  |
| M7: Save/Load |  |
| M8: Playable demo |  |

---

##  Note
**Gör INTE automatisk commit!** Vänta på `/git-commit`.
