# Unified Viewport Architecture

> All functionality available at all hierarchy levels

## Core Principle

**World, Level, Scene are ONLY for rendering optimization - NOT functionality separation.**

Every feature works identically at every level:
- Actors can be placed/edited at World, Level, or Scene
- 2D/3D toggle works the same everywhere
- Selection, transform, properties - same UI
- Only difference: what's currently visible/rendered

---

## Hierarchy Purpose

```
World
├── Contains: Levels (chunks of game world)
├── Renders: Only active Level + neighbors (LOD)
└── Editor: Can place/edit actors that span levels

Level  
├── Contains: Scenes (rooms/areas)
├── Renders: Only active Scene + connected scenes
└── Editor: Can place/edit actors within level

Scene
├── Contains: Actors (objects, NPCs, items)
├── Renders: Full detail of current scene
└── Editor: Full actor manipulation
```

---

## Unified Feature Set

### Available at ALL levels:

| Feature | World | Level | Scene |
|---------|-------|-------|-------|
| Place Actor | ✅ | ✅ | ✅ |
| Move/Rotate/Scale | ✅ | ✅ | ✅ |
| Select/Multi-select | ✅ | ✅ | ✅ |
| Copy/Paste | ✅ | ✅ | ✅ |
| Undo/Redo | ✅ | ✅ | ✅ |
| Properties Panel | ✅ | ✅ | ✅ |
| 2D View | ✅ | ✅ | ✅ |
| 3D View | ✅ | ✅ | ✅ |
| Grid Snap | ✅ | ✅ | ✅ |
| Gizmos | ✅ | ✅ | ✅ |
| Zoom/Pan/Orbit | ✅ | ✅ | ✅ |

### Level-specific visibility:

| What's Rendered | World | Level | Scene |
|-----------------|-------|-------|-------|
| All Levels | ✅ (LOD) | ❌ | ❌ |
| All Scenes in Level | ❌ | ✅ (LOD) | ❌ |
| All Actors in Scene | ❌ | ❌ | ✅ (Full) |
| Neighbor preview | ✅ | ✅ | Optional |

---

## New Class Architecture

```
src/editor/viewport/
│
├── ViewportPanel.h/cpp
│   └── Main docking panel, toolbar, breadcrumbs
│
├── ViewportRenderer.h/cpp
│   ├── render2D() / render3D()
│   ├── Handles: grid, actors, selection highlights
│   └── LOD based on current hierarchy level
│
├── ViewportCamera.h/cpp
│   ├── 2D: pan, zoom
│   ├── 3D: orbit, pan, zoom
│   └── Shared state between 2D/3D
│
├── ViewportInput.h/cpp
│   ├── Mouse picking (2D + 3D ray cast)
│   ├── Drag selection box
│   ├── Transform handles
│   └── Keyboard shortcuts
│
├── ViewportSelection.h/cpp
│   ├── Selected actors list
│   ├── Multi-select
│   ├── Bounds calculation
│   └── Highlight rendering
│
└── ViewportGizmo.h/cpp
    ├── Translate/Rotate/Scale handles
    ├── Works in 2D and 3D
    └── ImGuizmo integration
```

---

## ViewportRenderer Design

```cpp
class ViewportRenderer {
public:
    void render(RenderContext& ctx);
    
    void setHierarchyLevel(HierarchyLevel level);
    void setRenderMode(RenderMode mode); // Mode2D, Mode3D
    
    void setWorld(World* world);
    void setActiveLevel(Level* level);
    void setActiveScene(Scene* scene);

private:
    // Renders appropriate content based on hierarchy level
    void renderWorld();   // All levels as boxes/regions
    void renderLevel();   // All scenes as tiles
    void renderScene();   // All actors in detail
    
    // Common rendering (same at all levels)
    void renderGrid();
    void renderSelection();
    void renderGizmos();
    void renderActors(const std::vector<Actor*>& actors);
    
    // LOD helpers
    float calculateLOD(const glm::vec3& objectPos);
    bool isInFrustum(const AABB& bounds);
};
```

---

## Actor Ownership

Actors can exist at any level:

```cpp
class World {
    std::vector<std::unique_ptr<Actor>> m_worldActors;  // Span multiple levels
};

class Level {
    std::vector<std::unique_ptr<Actor>> m_levelActors;  // Span multiple scenes
};

class Scene {
    std::vector<std::unique_ptr<Actor>> m_sceneActors;  // Local to scene
};
```

When rendering:
- Scene view: Render sceneActors + visible levelActors + visible worldActors
- Level view: Render levelActors + sceneActors (LOD) + visible worldActors
- World view: Render worldActors + levelActors (LOD) + sceneActors (icons only)

---

## Implementation Phases

### Phase 1: Combine Folders
- [ ] Move all viewport files to `src/editor/viewport/`
- [ ] Update CMakeLists.txt
- [ ] Fix includes

### Phase 2: Create ViewportRenderer
- [ ] Extract rendering logic from ViewportPanel_*.cpp
- [ ] Single class with render2D/render3D methods
- [ ] LOD based on hierarchy level

### Phase 3: Unified Input
- [ ] Single ViewportInput class
- [ ] Works same at all levels
- [ ] 2D click + 3D ray casting

### Phase 4: Selection System
- [ ] Unified selection across levels
- [ ] Multi-select support
- [ ] Properties panel integration

### Phase 5: Gizmos
- [ ] ImGuizmo integration
- [ ] Works in 2D and 3D
- [ ] Translate/Rotate/Scale

---

## File Migration Plan

### Delete (after migration):
```
src/editor/panels/viewport/
├── ViewportPanel_Actors.cpp   → merge into ViewportRenderer
├── ViewportPanel_Core.cpp     → merge into ViewportPanel
├── ViewportPanel_Input.cpp    → merge into ViewportInput
├── ViewportPanel_Level.cpp    → merge into ViewportRenderer
├── ViewportPanel_Scene.cpp    → merge into ViewportRenderer
└── ViewportPanel_World.cpp    → merge into ViewportRenderer
```

### Keep/Refactor:
```
src/editor/viewport/
├── EditorCamera3D.h/cpp       → rename to ViewportCamera
├── Viewport3DPanel.h/cpp      → merge into ViewportRenderer
└── ViewportPanel.h            → keep, simplify
```

### New Files:
```
src/editor/viewport/
├── ViewportRenderer.h/cpp
├── ViewportInput.h/cpp
├── ViewportSelection.h/cpp
└── ViewportGizmo.h/cpp
```

---

*Created: 2026-01-08*
