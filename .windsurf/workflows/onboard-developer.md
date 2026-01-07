---
description: New developer onboarding process
---

# Onboard Developer Workflow

> Getting new team members productive quickly

## Day 1: Environment Setup

### 1.1 Install Tools
```powershell
# Required
# - Visual Studio 2022
# - CMake 3.20+
# - Git

# vcpkg
git clone https://github.com/microsoft/vcpkg C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat
```

### 1.2 Install Dependencies
```powershell
vcpkg install sdl2:x64-windows
vcpkg install sdl2-image:x64-windows
vcpkg install sdl2-mixer:x64-windows
vcpkg install box2d:x64-windows
vcpkg install nlohmann-json:x64-windows
vcpkg install imgui[docking-experimental]:x64-windows
vcpkg install imnodes:x64-windows
```

### 1.3 Clone Repository
```powershell
git clone https://github.com/[org]/retro-cplus-game.git
cd retro-cplus-game
```

### 1.4 First Build
```powershell
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
.\Release\RetroGame.exe
```

---

## Day 2: Codebase Overview

### 2.1 Project Structure
```
src/
├── engine/     # Core engine (static library)
├── game/       # Game executable
└── editor/     # Editor executable
```

### 2.2 Key Documentation
- `docs/README.md` - Quick start
- `docs/architecture/` - System design
- `docs/ROADMAP.md` - Development plan

### 2.3 Important Classes
| Class | Purpose |
|-------|---------|
| Object | Root base class |
| ActorObject | Game entity base |
| ActorComponent | Component base |
| Scene | Actor container |
| IState | Game state interface |

---

## Day 3: Development Workflow

### 3.1 Daily Workflow
```
1. /init-session     # Start day
2. [work on task]
3. /git-commit       # Commit changes
4. /end-session      # End day
```

### 3.2 Feature Development
```
1. /new-feature      # Create branch
2. [implement]
3. /update-docs      # Documentation
4. /generate-pr      # Create PR
```

### 3.3 Code Review
- All PRs need 1+ approval
- Respond to feedback within 24h
- Follow code review checklist

---

## Week 1: First Contribution

### 4.1 Find Issue
Look for `good-first-issue` label in GitHub Issues

### 4.2 Create Branch
```powershell
git checkout -b feature/XXX-my-first-feature
```

### 4.3 Implement
- Follow coding standards
- Document new code
- Write tests

### 4.4 Submit PR
```powershell
git push origin feature/XXX-my-first-feature
# Create PR on GitHub
```

---

## Resources

### Documentation
| Doc | Purpose |
|-----|---------|
| `docs/README.md` | Quick start |
| `docs/architecture/` | Technical design |
| `docs/api/` | API reference |
| `.windsurf/rules/` | Coding standards |
| `.windsurf/workflows/` | Development workflows |

### Contacts
- Tech Lead: [name]
- Mentor: [name]
- Team Channel: #engine-dev

### Learning Resources
- [Unreal Engine Coding Standards](https://docs.unrealengine.com/)
- [Game Programming Patterns](https://gameprogrammingpatterns.com/)
- [SDL2 Wiki](https://wiki.libsdl.org/)

---

## Onboarding Checklist

### Day 1
- [ ] Tools installed
- [ ] Repository cloned
- [ ] First build successful
- [ ] Game runs locally

### Day 2
- [ ] Project structure understood
- [ ] Key docs read
- [ ] Architecture overview

### Day 3
- [ ] Workflows learned
- [ ] Git process understood
- [ ] Code review process

### Week 1
- [ ] First issue assigned
- [ ] First PR submitted
- [ ] First code review
- [ ] Mentor check-in

### Month 1
- [ ] Multiple features completed
- [ ] Independent work capable
- [ ] Team integration complete
