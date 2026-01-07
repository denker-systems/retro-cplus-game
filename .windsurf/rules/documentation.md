---
trigger: always_on
description: Documentation standards
---

# Documentation Standards

> Enterprise documentation requirements

## Code Documentation

### File Headers (Required)
```cpp
/**
 * @file FileName.cpp
 * @brief Brief description
 */
```

### Class Documentation
```cpp
/**
 * @class ClassName
 * @brief Brief description
 * @details Extended description
 */
```

### Method Documentation
```cpp
/**
 * @brief What it does
 * @param name Description
 * @return What it returns
 */
```

---

## Project Documentation

### Structure
```
docs/
├── README.md           # Quick start
├── CHANGELOG.md        # Release notes
├── ROADMAP.md          # Development plan
├── architecture/       # Technical docs
├── api/               # API reference
├── guides/            # Tutorials
├── adr/               # Decision records
└── dev/               # Development logs
    ├── DEVLOG.md
    └── sessions/
```

---

## CHANGELOG Format

```markdown
## [1.2.0] - YYYY-MM-DD

### Added
- Feature (#issue)

### Changed
- Change

### Fixed
- Bug fix (#issue)
```

---

## ADR Format

```markdown
# ADR-NNN: Title

## Status
Accepted

## Context
Why needed?

## Decision
What decided?

## Consequences
- Positive: ...
- Negative: ...
```

---

## When to Document

### Always
- New classes/systems
- Public APIs
- Architecture decisions
- Breaking changes

### Update When
- Functionality changes
- New parameters
- Deprecations
