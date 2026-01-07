---
description: Safe refactoring process with testing
---

# Refactor Workflow

> Safe code improvement without changing behavior

## When to Refactor

- Code smells identified
- Technical debt scheduled
- Before adding features
- After learning better patterns

## When NOT to Refactor

- During hotfix
- Without tests
- Near release deadline
- Without clear goal

---

## 1. Plan Refactoring

### Identify Scope
```markdown
## Refactoring: [Name]

**Goal:** What improvement?
**Scope:** Which files/classes?
**Risk:** Low/Medium/High

### Before
- Problem 1
- Problem 2

### After
- Improvement 1
- Improvement 2
```

### Create Branch
```powershell
git checkout -b refactor/XXX-description
```

---

## 2. Ensure Test Coverage

### Check Existing Tests
```powershell
ctest -C Release -R "AffectedModule" --output-on-failure
```

### Add Tests if Needed
```cpp
// Characterization test - capture current behavior
TEST(ModuleTest, CurrentBehavior) {
    // Test that captures existing behavior
    // This should pass before AND after refactoring
}
```

---

## 3. Small Steps

### Refactoring Patterns

#### Extract Method
```cpp
// Before
void process() {
    // 50 lines of code
}

// After
void process() {
    validateInput();
    performCalculation();
    updateState();
}
```

#### Rename for Clarity
```cpp
// Before
int x;
void calc();

// After
int m_playerHealth;
void calculateDamage();
```

#### Extract Class
```cpp
// Before: God class with 20 responsibilities
class GameManager { ... };

// After: Focused classes
class InputManager { ... };
class SceneManager { ... };
class AudioManager { ... };
```

---

## 4. Test After Each Change

### Run Tests Frequently
```powershell
cd build
cmake --build . --config Release
ctest -C Release --output-on-failure
```

### Commit Small Changes
```powershell
git add -p
git commit -m "refactor(scope): extract X method"
```

---

## 5. Common Refactorings

### Code Smells → Solutions

| Smell | Refactoring |
|-------|-------------|
| Long method | Extract method |
| Long class | Extract class |
| Duplicate code | Extract common code |
| Magic numbers | Extract constants |
| Complex conditional | Replace with polymorphism |
| Feature envy | Move method |
| Data clumps | Extract class |

---

## 6. Documentation

### Update Comments
```cpp
// Update any affected documentation
// Remove outdated comments
// Add explanatory comments for complex changes
```

### Update Architecture Docs
If structure changed, update `docs/architecture/`

---

## 7. Code Review

### PR Description
```markdown
## Refactoring: [Title]

**Goal:** [What improvement]

### Changes
- Extracted X from Y
- Renamed A to B
- Moved C to D

### Behavior
No behavior changes. All tests passing.

### Verification
- [ ] All tests pass
- [ ] Manual testing done
- [ ] No regressions
```

---

## 8. Verification

### Before/After Comparison
- Same test results
- Same functionality
- Same or better performance

### Performance Check
```powershell
# If performance-sensitive code
# Run benchmarks before and after
```

---

## Refactoring Checklist

- [ ] Clear goal defined
- [ ] Tests exist/added
- [ ] Small incremental changes
- [ ] Tests pass after each change
- [ ] No behavior changes
- [ ] Code review complete
- [ ] Documentation updated
