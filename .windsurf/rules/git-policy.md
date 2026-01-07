---
trigger: always_on
description: Git workflow and version control policy
---

# Git Policy

> Enterprise version control standards

## ⚠️ Critical Rule

**Never auto-commit without explicit user instruction!**

### Forbidden (without permission)
- `git commit`
- `git push`
- `git add`

### Always Allowed (read-only)
- `git status`
- `git log`
- `git diff`
- `git branch`

---

## Branch Strategy

```
main                    # Production-ready
├── develop            # Integration (optional)
├── feature/XXX-desc   # New features
├── bugfix/XXX-desc    # Bug fixes
├── hotfix/XXX-desc    # Emergency fixes
└── release/vX.Y.Z     # Release prep
```

---

## Commit Format

```
type(scope): description

[body]

[footer]
```

### Types
| Type | Description |
|------|-------------|
| feat | New feature |
| fix | Bug fix |
| docs | Documentation |
| style | Formatting |
| refactor | Code restructure |
| perf | Performance |
| test | Tests |
| build | Build system |
| ci | CI/CD |
| chore | Maintenance |

---

## Merge Strategy

- **Feature → main**: Squash merge
- **Hotfix → main**: Merge commit
- **Release → main**: Merge commit + tag

---

## Code Review

### Requirements
| Change Type | Approvals |
|-------------|-----------|
| Standard | 1 |
| Architecture | 2 (1 senior) |
| Critical | 2 core team |

---

## Documentation Sync

| Document | When Updated |
|----------|--------------|
| Session report | During session |
| DEVLOG | At commit |
| CHANGELOG | At release |
| ROADMAP | At feature completion |
