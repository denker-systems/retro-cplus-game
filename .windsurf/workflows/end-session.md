---
description: End session and sync documentation
---

# End Session Workflow

> Avsluta arbetsdagen och synka dokumentation

## 1. Get Date
// turbo
```powershell
$today = Get-Date -Format 'yyyy-MM-dd'; Write-Host "=== Avslutar Session $today ===" -ForegroundColor Cyan
```

## 2. Check Today's Commits
// turbo
```powershell
$today = Get-Date -Format 'yyyy-MM-dd'
git log --oneline --after="$today 00:00" --before="$today 23:59" --format="%h %s"
```

## 3. Check Uncommitted Changes
// turbo
```powershell
git status --short
```

---

## 4. Update Session Report

Uppdatera `docs/dev/sessions/YYYY-MM-DD.md`:

```markdown
# Session YYYY-MM-DD

## Summary
Kort sammanfattning av dagens arbete.

---

## Completed

### [Area 1]
- Task completed
- Another task

---

## Git Commits

| Hash | Type | Description |
|------|------|-------------|
| `abc123` | feat | Description |

---

## Next Session
- [ ] Task 1
- [ ] Task 2
```

---

## 5. Update DEVLOG

Lägg till i `docs/dev/DEVLOG.md`:

```markdown
## YYYY-MM-DD

- `abc123` feat(scope): description
  - Detail 1
  - Session: [YYYY-MM-DD](sessions/YYYY-MM-DD.md)
```

---

## 6. Update CHANGELOG (if features added)

Under `[Unreleased]` i `docs/CHANGELOG.md`:

```markdown
### Added
- New feature (#issue)

### Fixed
- Bug fix (#issue)
```

---

## 7. Update ROADMAP (if features completed)

Markera klara features som `[x]` i `docs/ROADMAP.md`.

---

## Checklist

- [ ] Session report uppdaterad
- [ ] DEVLOG uppdaterad
- [ ] CHANGELOG uppdaterad (om relevant)
- [ ] ROADMAP uppdaterad (om relevant)
- [ ] Uncommitted changes hanterade

---

##  PÅMINNELSE

**Gör INTE automatisk commit!** Vänta på användarens `/git-commit`.
