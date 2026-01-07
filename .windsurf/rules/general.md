---
trigger: always_on
description: General project rules and communication
---

# General Rules

> Grundläggande projektregler

## Communication

- **Språk:** Svenska
- **Stil:** Koncis och direkt
- **Kod:** Engelska (class names, comments)

---

## Tech Stack

| Komponent | Teknologi |
|-----------|-----------|
| Språk | C++17 |
| Build | CMake 3.16+ |
| Packages | vcpkg |
| Graphics | SDL2 |
| Physics | Box2D |
| Data | nlohmann/json |
| Editor UI | Dear ImGui |

---

## Code Style

- **PascalCase** för klasser
- **camelCase** för metoder
- **m_** prefix för medlemsvariabler
- **smart pointers** för ownership
- **RAII** för resurshantering

---

## Project Structure

```
src/
 engine/    # Core engine (static lib)
 game/      # Game executable
 editor/    # Editor executable
```

---

## Workflows

| Workflow | Användning |
|----------|------------|
| `/init-session` | Starta arbetsdag |
| `/git-commit` | Committa ändringar |
| `/end-session` | Avsluta arbetsdag |
| `/build` | Bygg projektet |
