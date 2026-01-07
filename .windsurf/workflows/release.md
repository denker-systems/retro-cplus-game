---
description: Production release workflow
---

# Release Workflow

> Enterprise release management

## Release Types

| Type | Frequency | Example |
|------|-----------|---------|
| Major | Quarterly | v2.0.0 |
| Minor | Monthly | v1.3.0 |
| Patch | As needed | v1.2.1 |
| Hotfix | Emergency | v1.2.1-hotfix |

---

## 1. Pre-Release Checklist

### Code Freeze
```powershell
Write-Host "=== Release v$version Code Freeze ===" -ForegroundColor Cyan
```

### Verify Main Branch
```powershell
git checkout main
git pull origin main
git log -5 --oneline
```

### All Tests Pass
```powershell
cd build
cmake --build . --config Release
ctest -C Release --output-on-failure
```

### Code Quality
- [ ] No critical bugs open
- [ ] No blockers in backlog
- [ ] All PRs merged
- [ ] Code review complete

---

## 2. Create Release Branch

```powershell
$version = "1.2.0"
git checkout -b release/v$version
```

---

## 3. Update Version

### CMakeLists.txt
```cmake
project(RetroEngine VERSION 1.2.0)
```

### Version Header
```cpp
// src/engine/Version.h
#define ENGINE_VERSION_MAJOR 1
#define ENGINE_VERSION_MINOR 2
#define ENGINE_VERSION_PATCH 0
#define ENGINE_VERSION_STRING "1.2.0"
```

---

## 4. Update CHANGELOG

```markdown
## [1.2.0] - YYYY-MM-DD

### Added
- Feature 1 (#123)
- Feature 2 (#124)

### Changed
- Change 1 (#125)

### Fixed
- Bug fix 1 (#126)
- Bug fix 2 (#127)

### Deprecated
- Old API (use new API instead)

### Security
- Security fix (#128)
```

---

## 5. Final Testing

### Full Test Suite
```powershell
ctest -C Release --output-on-failure
```

### Smoke Tests
- [ ] Game launches
- [ ] Editor launches
- [ ] Core features work
- [ ] No crashes

### Performance Tests
- [ ] Frame rate acceptable
- [ ] Memory usage normal
- [ ] Load times acceptable

---

## 6. Build Release Artifacts

### Build All Configurations
```powershell
cmake --build . --config Release
cmake --build . --config Debug  # For debugging symbols
```

### Package
```powershell
# Create distribution folder
$dist = "dist/v$version"
New-Item -ItemType Directory -Path $dist -Force

# Copy executables
Copy-Item build/Release/RetroGame.exe $dist/
Copy-Item build/Release/RetroEditor.exe $dist/

# Copy assets
Copy-Item -Recurse assets $dist/assets

# Copy DLLs
Copy-Item build/Release/*.dll $dist/

# Create README
"Retro Engine v$version" | Out-File "$dist/README.txt"
```

### Create Archive
```powershell
Compress-Archive -Path $dist/* -DestinationPath "releases/RetroEngine-v$version.zip"
```

---

## 7. Create Tag

```powershell
git add .
git commit -m "chore: prepare release v$version"
git tag -a "v$version" -m "Release v$version

Highlights:
- Feature 1
- Feature 2
- Bug fixes

See CHANGELOG.md for full details."
```

---

## 8. Merge to Main

```powershell
git checkout main
git merge --no-ff release/v$version
git push origin main --tags
```

---

## 9. Create GitHub Release

### Release Notes Template
```markdown
# Retro Engine v1.2.0

## Highlights
- ✨ Feature 1
- ✨ Feature 2
- 🐛 Important bug fixes

## Downloads
- [Windows x64](link)
- [Source Code](link)

## Changes
See [CHANGELOG.md](CHANGELOG.md) for complete list.

## Upgrade Notes
[Any breaking changes or migration steps]

## Contributors
- @developer1
- @developer2
```

---

## 10. Post-Release

### Announce
```
🚀 **Retro Engine v1.2.0 Released!**

**Highlights:**
- Feature 1
- Feature 2

**Download:** [link]
**Release Notes:** [link]
```

### Cleanup
```powershell
git branch -d release/v$version
```

### Update Roadmap
Mark completed items and plan next release.

---

## Release Checklist

### Pre-Release
- [ ] All tests pass
- [ ] No critical bugs
- [ ] Code review complete
- [ ] Documentation updated

### Release
- [ ] Version updated
- [ ] CHANGELOG updated
- [ ] Tag created
- [ ] Artifacts built
- [ ] GitHub release created

### Post-Release
- [ ] Team notified
- [ ] Users notified
- [ ] Roadmap updated
- [ ] Branch cleaned up
