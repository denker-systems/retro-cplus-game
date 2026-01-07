---
description: Commit changes with proper format and documentation sync
---

# Git Commit Workflow

> Committa ändringar med konventionellt format

## 1. Check Status
// turbo
```powershell
git status --short
```

## 2. Review Changes
// turbo
```powershell
git diff --stat HEAD
```

---

## 3. Stage Changes
```powershell
git add .
# Or selectively:
git add src/specific/file.cpp
```

---

## 4. Commit with Format

```powershell
git commit -m "type(scope): description" -m "- Detail 1" -m "- Detail 2"
```

### Commit Types
| Type | Usage |
|------|-------|
| `feat` | New feature |
| `fix` | Bug fix |
| `docs` | Documentation |
| `refactor` | Code refactoring |
| `style` | Formatting |
| `test` | Tests |
| `build` | Build system |
| `chore` | Maintenance |

### Examples
```powershell
git commit -m "feat(physics): add Box2D integration"
git commit -m "fix(player): clamp to walk area correctly"
git commit -m "docs: update ROADMAP progress"
```

---

## 5. Update DEVLOG

Lägg till i `docs/dev/DEVLOG.md`:

```markdown
- `HASH` type(scope): description
  - Detail 1
  - Session: [YYYY-MM-DD](sessions/YYYY-MM-DD.md)
```

---

## 6. Update Session Report

Lägg till commit i dagens session report:

```markdown
## Git Commits

| Hash | Type | Description |
|------|------|-------------|
| `abc123` | feat | Description |
```

---

## 7. Verify
// turbo
```powershell
git log -1 --stat
```

---

##  VIKTIGT

- **Analysera ALLA ändringar** innan commit
- **Dokumentera arkitekturändringar** om relevanta
- **Synka DEVLOG** med commit
