---
trigger: always_on
description: Documentation structure and standards
---

# Documentation Standards

> Skalbar dokumentationsstruktur

## Document Structure

```
docs/
 README.md              # Quick start
 CHANGELOG.md           # Release notes
 ROADMAP.md             # Development plan

 architecture/          # Technical architecture
    overview.md
    systems.md

 api/                   # API reference (per modul)
    engine/
    editor/

 guides/                # Developer guides
    getting-started.md
    tutorials/

 dev/                   # Development logs
     DEVLOG.md          # Commit history
     sessions/          # Daily session reports
```

---

## File Formats

### README.md
```markdown
# Project Name

Brief description.

## Quick Start
Build and run instructions.

## Documentation
Links to key docs.
```

### CHANGELOG.md
```markdown
## [Unreleased]

### Added
- New features

### Changed
- Changes

### Fixed
- Bug fixes
```

### Session Report
```markdown
# Session YYYY-MM-DD

## Summary
What was accomplished.

## Git Commits
| Hash | Type | Description |

## Next Steps
- [ ] Task 1
```

---

## When to Document

### Always Document
- Nya klasser/system
- Publika API:er
- Arkitekturbeslut
- Breaking changes

### Update When
- Ändrad funktionalitet
- Nya parametrar
- Deprecated features

---

## Naming

| Typ | Format | Exempel |
|-----|--------|---------|
| Folders | kebab-case | `getting-started/` |
| Files | kebab-case.md | `quick-start.md` |
| API docs | PascalCase.md | `PlayerActor.md` |
