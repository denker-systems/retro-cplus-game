---
trigger: always_on
description: CI/CD pipeline standards
---

# CI/CD Standards

> Enterprise pipeline requirements

## Pipeline Stages

```
Commit → Build → Test → Review → Deploy
```

---

## Build Matrix

| Platform | Compiler | Configs |
|----------|----------|---------|
| Windows | MSVC 2022 | Debug, Release |
| Windows | Clang | Debug, Release |

---

## Quality Gates

### Blockers (Must Pass)
- [ ] Build succeeds
- [ ] All tests pass
- [ ] Coverage ≥ 60%
- [ ] No critical issues

### Warnings
- [ ] New TODOs without issues
- [ ] Undocumented public API
- [ ] High cyclomatic complexity

---

## Build Commands

### Release
```powershell
cmake --build . --config Release --parallel
```

### Tests
```powershell
ctest -C Release --output-on-failure
```

### Full Pipeline
```powershell
cmake --build . --config Release
ctest -C Release --output-on-failure
```

---

## Environments

| Environment | Trigger | Audience |
|-------------|---------|----------|
| Dev | Every commit | Developers |
| Staging | PR merge | QA |
| Production | Tag/Manual | Users |

---

## Release Types

| Type | Frequency |
|------|-----------|
| Nightly | Daily auto |
| RC | Pre-release |
| Stable | Manual |
| Hotfix | Emergency |
