---
description: Corporate CI/CD Pipeline for Game Engine Development
trigger: always_on
---

# CI/CD Pipeline Standards

> Enterprise-grade pipeline för game engine med 50+ utvecklare

## PIPELINE STAGES

```
            
   COMMIT        BUILD        TEST        DEPLOY    
            
                                                            
                                                            
  Lint/Format        Compile            Unit Tests        Staging
  Pre-commit         All Configs        Integration       Production
  Hooks              Asset Pipeline     Performance       Hotfix
```

---

## 1. PRE-COMMIT STAGE

### 1.1 Hooks
```yaml
# .pre-commit-config.yaml
repos:
  - repo: local
    hooks:
      - id: clang-format
        name: C++ Formatting
        entry: clang-format -i
        files: \.(cpp|h)$
      
      - id: header-guard
        name: Verify #pragma once
        entry: scripts/check-header-guards.py
        files: \.h$
      
      - id: doxygen-check
        name: Verify Documentation
        entry: scripts/check-doxygen.py
        files: \.(cpp|h)$
```

### 1.2 Commit Message Format
```
type(scope): description

[optional body]

[optional footer]
```

**Types:**
- `feat` - Ny feature
- `fix` - Bugfix
- `docs` - Dokumentation
- `style` - Formattering
- `refactor` - Refactoring
- `perf` - Performance
- `test` - Tester
- `build` - Build system
- `ci` - CI/CD
- `chore` - Maintenance

---

## 2. BUILD STAGE

### 2.1 Build Configurations
```cmake
# Debug - Full symbols, no optimization
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release - Full optimization
cmake -DCMAKE_BUILD_TYPE=Release ..

# RelWithDebInfo - Optimization + symbols
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
```

### 2.2 Build Matrix
| Platform | Compiler | Config |
|----------|----------|--------|
| Windows | MSVC 2022 | Debug, Release |
| Windows | Clang | Debug, Release |
| Linux | GCC 12 | Debug, Release |
| Linux | Clang 15 | Debug, Release |

### 2.3 Asset Pipeline
```yaml
asset_pipeline:
  - name: Texture Compression
    input: assets/raw/textures/*.png
    output: assets/textures/*.dds
    
  - name: Audio Conversion
    input: assets/raw/audio/*.wav
    output: assets/audio/*.ogg
    
  - name: JSON Validation
    input: assets/data/*.json
    schema: schemas/*.json
```

---

## 3. TEST STAGE

### 3.1 Test Pyramid
```
         /\
        /  \       UI Tests (10%)
       /\
      /      \     Integration Tests (20%)
     /\
    /          \   Unit Tests (70%)
   /\
```

### 3.2 Test Categories
```cpp
// Unit Test - Isolerad, snabb
TEST(PhysicsBody, AppliesGravityCorrectly) {
    PhysicsBody body;
    body.setGravity(9.8f);
    body.update(1.0f);
    EXPECT_NEAR(body.getVelocity().y, 9.8f, 0.001f);
}

// Integration Test - Flera system
TEST(SceneIntegration, ActorsReceivePhysicsUpdates) {
    Scene scene;
    auto actor = scene.createActor<PhysicsActor>();
    scene.update(1.0f);
    EXPECT_TRUE(actor->wasPhysicsUpdated());
}

// Performance Test - Timing
TEST(RenderingPerf, Renders1000SpritesUnder16ms) {
    auto start = Clock::now();
    renderer.render(1000_sprites);
    auto duration = Clock::now() - start;
    EXPECT_LT(duration, 16ms);
}
```

### 3.3 Code Coverage
- **Minimum:** 60% line coverage
- **Target:** 80% line coverage
- **Critical paths:** 95% coverage

---

## 4. DEPLOY STAGE

### 4.1 Environments
| Environment | Trigger | Audience |
|-------------|---------|----------|
| Dev | Every commit | Developers |
| Staging | PR merge | QA Team |
| Production | Manual/Tag | Players |

### 4.2 Release Types
- **Nightly** - Automatisk varje natt
- **RC** - Release Candidate för QA
- **Stable** - Public release
- **Hotfix** - Emergency patch

---

## 5. QUALITY GATES

### 5.1 Blockers
- [ ] Build fails
- [ ] Test coverage < 60%
- [ ] Critical bugs > 0
- [ ] Performance regression > 10%

### 5.2 Warnings
- [ ] New TODO without issue reference
- [ ] Undocumented public API
- [ ] Complex cyclomatic complexity > 15

---

## LOCAL COMMANDS

### Build
// turbo
```powershell
cd build; cmake --build . --config Release
```

### Run Tests
```powershell
cd build; ctest -C Release --output-on-failure
```

### Generate Docs
```powershell
doxygen Doxyfile
```

### Format Code
```powershell
clang-format -i src/**/*.cpp src/**/*.h
```
