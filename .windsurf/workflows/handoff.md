---
description: Team handoff between developers, shifts, or timezones
---

# Handoff Workflow

> Seamless knowledge transfer between team members

## When to Use

- End of workday (timezone handoff)
- Task reassignment
- Vacation/PTO coverage
- Pair programming switch

---

## 1. Current State Summary

### Code Status
```powershell
git status --short
git branch --show-current
git log -3 --oneline
```

### Build Status
```powershell
cd build; cmake --build . --config Release 2>&1 | Select-Object -Last 3
```

### Test Status
```powershell
cd build; ctest -C Release --output-on-failure 2>&1 | Select-Object -Last 10
```

---

## 2. Create Handoff Document

Create `docs/dev/handoffs/YYYY-MM-DD-from-to.md`:

```markdown
# Handoff: [From] → [To]

**Date:** YYYY-MM-DD HH:MM
**Branch:** feature/xxx

---

## Current Task

**Issue:** #123 - [Issue Title]
**Status:** In Progress (70%)

### What's Done
- [x] Implemented base class
- [x] Added unit tests
- [x] Documentation started

### What's Left
- [ ] Integration with SystemX
- [ ] Performance optimization
- [ ] Final review

---

## Code Changes

### New Files
- `src/engine/NewSystem.h` - Main header
- `src/engine/NewSystem.cpp` - Implementation

### Modified Files
- `CMakeLists.txt` - Added new source
- `src/game/Game.cpp` - Integration point

---

## Technical Context

### Architecture Decisions
- Using Strategy pattern for X because Y
- Chose Box2D over custom physics for Z

### Known Issues
- [ ] Memory leak in debug mode (low priority)
- [ ] Performance regression in Scene::render()

### Gotchas
- Don't forget to call `init()` before `update()`
- JSON format changed - see `docs/api/formats.md`

---

## Environment Notes

### Local Setup Required
- Environment variable: `GAME_DEBUG=1`
- Asset path: Must run from `build/Release/`

### Dependencies Changed
- Added `nlohmann-json` via vcpkg
- Upgraded SDL2 to 2.28.0

---

## Testing Notes

### Manual Tests Needed
1. Run game, enter Room "tavern"
2. Click on NPC "bartender"
3. Verify dialog appears

### Automated Tests
- All passing except `TestPhysicsCollision` (known flaky)

---

## Priority for Next Developer

1. **HIGH:** Fix collision detection edge case
2. **MEDIUM:** Complete dialog integration
3. **LOW:** Refactor logging system

---

## Questions/Blockers

- Need design review for inventory UI
- Waiting on art assets for new character

---

## Contact

- Slack: @[username]
- Available: [timezone] [hours]
```

---

## 3. Update Issue Tracker

```markdown
**Status Update** (Handoff to @nextdev)

Progress: 70%
Branch: feature/xxx
Commits: abc123, def456

Next steps:
- [ ] Task 1
- [ ] Task 2

Notes: [brief context]
```

---

## 4. Slack/Teams Notification

```
🔄 **Handoff:** [Task Name]
**From:** @[outgoing]
**To:** @[incoming]
**Branch:** `feature/xxx`
**Status:** In Progress (70%)

📋 Handoff doc: [link]
🎫 Issue: #123

**Priority:** Complete collision detection
**Blocker:** None
```

---

## 5. Sync Meeting (Optional)

If complex handoff:
- 15-minute video call
- Screen share current state
- Walk through code changes
- Answer questions

---

## Receiving Handoff

### 1. Read Documentation
```powershell
Get-Content "docs/dev/handoffs/YYYY-MM-DD-*.md"
```

### 2. Pull Latest
```powershell
git fetch origin
git checkout feature/xxx
git pull origin feature/xxx
```

### 3. Verify Build
```powershell
cd build; cmake --build . --config Release
```

### 4. Review Changes
```powershell
git log --oneline -10
git diff main...HEAD --stat
```

### 5. Acknowledge Receipt
Update issue tracker and notify team.

---

## Checklist

### Outgoing Developer
- [ ] Code committed and pushed
- [ ] Handoff document created
- [ ] Issue tracker updated
- [ ] Team notified
- [ ] Available for questions (specify duration)

### Incoming Developer
- [ ] Handoff document read
- [ ] Code pulled and building
- [ ] Context understood
- [ ] Questions asked/answered
- [ ] Ready to continue
