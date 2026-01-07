---
trigger: always_on
description: Git workflow and version control policy
---

# Git Policy

> Version control för små team med skalbar struktur

##  KRITISK REGEL

**ALDRIG git commit/push/add utan explicit instruktion!**

### Förbjudet (utan tillåtelse)
- `git commit`
- `git push`
- `git add`

### Alltid tillåtet (read-only)
- `git status`
- `git log`
- `git diff`
- `git branch`

---

## Branch Strategy

```
main           # Stabil, releasebar kod
 feature/*  # Feature branches
 bugfix/*   # Bug fixes
 hotfix/*   # Kritiska fixes
```

---

## Commit Format

```
type(scope): beskrivning

[valfri body]
```

**Types:**
- `feat` - Ny feature
- `fix` - Bugfix
- `docs` - Dokumentation
- `refactor` - Refactoring
- `style` - Formattering
- `test` - Tester
- `build` - Build system
- `chore` - Maintenance

---

## Dokumentation Sync

| Dokument | Uppdateras |
|----------|------------|
| `docs/dev/sessions/YYYY-MM-DD.md` | Under session |
| `docs/dev/DEVLOG.md` | Vid commit |
| `docs/CHANGELOG.md` | Vid release |
| `docs/ROADMAP.md` | Vid feature completion |

---

## Workflows

| Workflow | Trigger |
|----------|---------|
| `/init-session` | Session start |
| `/git-commit` | Explicit instruktion |
| `/end-session` | Session slut |
| `/generate-pr` | Innan PR |
