---
description: Standard code review process for pull requests
---

# Code Review Workflow

> Enterprise code review for quality assurance

## Review Process

```
PR Created → Self-Review → Peer Review → Approval → Merge
```

---

## 1. As Author

### 1.1 Before Requesting Review

```powershell
# Check diff
git diff main...HEAD --stat

# Verify build
cd build; cmake --build . --config Release

# Run tests
ctest -C Release --output-on-failure

# Check for debug code
git diff main...HEAD | Select-String "console.log|std::cout|DEBUG"
```

### 1.2 Self-Review Checklist

- [ ] Code compiles without warnings
- [ ] All tests pass
- [ ] No debug/temp code left
- [ ] No commented-out code
- [ ] Documentation updated
- [ ] CHANGELOG entry added
- [ ] Commit messages follow convention

### 1.3 PR Description Template

```markdown
## Summary
Brief description of changes.

## Issue
Closes #XXX

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation
- [ ] Refactoring
- [ ] Performance improvement

## Changes
- Change 1
- Change 2

## Testing
- [ ] Unit tests added/updated
- [ ] Integration tests added/updated
- [ ] Manual testing performed

## Screenshots (if UI)
[Images]

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review performed
- [ ] Documentation updated
- [ ] Tests passing
```

---

## 2. As Reviewer

### 2.1 Getting Started

```powershell
# Fetch PR branch
git fetch origin pull/XXX/head:pr-XXX
git checkout pr-XXX

# Build and test
cd build
cmake --build . --config Release
ctest -C Release --output-on-failure

# Run the application
.\Release\RetroGame.exe
```

### 2.2 Review Checklist

#### Code Quality
- [ ] Follows coding standards
- [ ] Readable and understandable
- [ ] No code duplication
- [ ] Appropriate complexity
- [ ] Efficient implementation

#### Architecture
- [ ] Follows SOLID principles
- [ ] Correct abstraction level
- [ ] No circular dependencies
- [ ] Component pattern used correctly

#### Documentation
- [ ] File headers present
- [ ] Classes documented
- [ ] Public methods documented
- [ ] Complex logic explained
- [ ] No obvious comments

#### Safety
- [ ] Memory management correct
- [ ] Thread safety considered
- [ ] Error handling complete
- [ ] No memory leaks
- [ ] No undefined behavior

#### Testing
- [ ] Sufficient test coverage
- [ ] Edge cases tested
- [ ] Performance impact considered

---

## 3. Review Comments

### Comment Prefixes

| Prefix | Meaning | Blocks Merge |
|--------|---------|--------------|
| `[BLOCKER]` | Must fix | Yes |
| `[MAJOR]` | Should fix | Usually |
| `[MINOR]` | Nice to have | No |
| `[NIT]` | Style preference | No |
| `[QUESTION]` | Need clarification | Maybe |
| `[PRAISE]` | Good job! | No |

### Example Comments

```cpp
// [BLOCKER] Memory leak - texture never freed
SDL_Texture* tex = IMG_LoadTexture(renderer, path);

// [MAJOR] Missing null check - potential crash
void process(Actor* actor) {
    actor->update();  // What if nullptr?
}

// [MINOR] Consider using range-based for
for (int i = 0; i < vec.size(); i++) { ... }

// [NIT] Prefer explicit type over auto here
auto x = getValue();

// [PRAISE] Great separation of concerns!
```

---

## 4. Approval Requirements

### Standard Changes
- **1 approval** from team member
- All CI checks passing
- No blockers

### Architecture Changes
- **2 approvals** (at least 1 senior)
- ADR documented
- Team lead sign-off

### Critical Systems (Physics, Save, Core)
- **2 approvals** from core team
- Performance benchmarks
- Security review if applicable

---

## 5. Response Times

| Priority | First Response | Resolution |
|----------|----------------|------------|
| Critical/Hotfix | 2 hours | 24 hours |
| High | 4 hours | 48 hours |
| Normal | 24 hours | 1 week |
| Low | 48 hours | 2 weeks |

---

## 6. Merge Process

### Pre-Merge
```powershell
# Ensure up to date
git checkout main
git pull origin main
git checkout pr-XXX
git rebase main

# Verify build
cd build
cmake --build . --config Release
ctest -C Release
```

### Merge
```powershell
# Squash merge
git checkout main
git merge --squash pr-XXX
git commit -m "feat(scope): description (#XXX)"
git push origin main
```

### Post-Merge
- Delete feature branch
- Close PR
- Update issue
- Notify team

---

## 7. Conflict Resolution

### Technical Disagreement
1. Discuss in PR comments
2. If unresolved, involve tech lead
3. Document decision in ADR if significant

### Style/Preference
1. Defer to coding standards
2. If not covered, author's choice
3. Consider adding to standards

---

## Review Metrics

| Metric | Target |
|--------|--------|
| Time to first review | < 24h |
| Review cycles | < 3 |
| PR size | < 400 lines |
| Comments resolved | 100% |
