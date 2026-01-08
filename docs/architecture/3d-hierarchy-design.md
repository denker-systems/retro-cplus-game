# 3D Hierarchy Architecture

> Design document for World/Level/Scene 3D rendering

## Overview

Extend 3D viewport to all hierarchy levels with consistent 2D/3D toggle.

```
┌─────────────────────────────────────────────────────────────┐
│ World View                                                   │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐                   │
│  │ Level 1  │  │ Level 2  │  │ Level 3  │  (3D boxes)       │
│  └──────────┘  └──────────┘  └──────────┘                   │
└─────────────────────────────────────────────────────────────┘
                        │ Click Level
                        ▼
┌─────────────────────────────────────────────────────────────┐
│ Level View                                                   │
│  ┌────┐   ┌────┐                                            │
│  │Room│───│Room│  (3D tiles with connections)               │
│  └────┘   └────┘                                            │
│     │       │                                                │
│  ┌────┐   ┌────┐                                            │
│  │Room│───│Room│                                            │
│  └────┘   └────┘                                            │
└─────────────────────────────────────────────────────────────┘
                        │ Click Scene
                        ▼
┌─────────────────────────────────────────────────────────────┐
│ Scene View                                                   │
│  Full 3D scene with actors, sprites, collision boxes        │
└─────────────────────────────────────────────────────────────┘
```

---

## Rendering Strategy

### World View (3D)

**Renders:**
- Each Level as a 3D box/region
- Position based on Level grid coordinates
- Size based on number of scenes
- Color-coded by level type/theme

**Camera:**
- Orbit camera centered on world origin
- Zoom to fit all levels
- Click level to navigate

**Optimization:**
- Simple box meshes only
- No textures at this level
- Frustum culling for large worlds

### Level View (3D)

**Renders:**
- Each Scene as a 3D tile/platform
- Connections between scenes (lines/arrows)
- Scene thumbnails as textures (optional)
- Spawn points and exit markers

**Camera:**
- Orbit camera centered on level
- Zoom to current level bounds
- Click scene to navigate

**Optimization:**
- Scene tiles as simple quads
- Instanced rendering for many scenes
- LOD: distant scenes show less detail

### Scene View (3D) - Already Implemented

**Renders:**
- Full actor geometry
- Collision boxes
- Walk areas
- Sprites and animations

**Camera:**
- Free orbit/pan camera
- Gizmo manipulation

---

## Class Design

### Viewport3DPanel Refactor

```cpp
class Viewport3DPanel {
public:
    enum class ViewLevel {
        World,  // Show all levels as boxes
        Level,  // Show all scenes as tiles
        Scene   // Show full scene detail
    };
    
    void setViewLevel(ViewLevel level);
    void setWorld(engine::World* world);
    void setLevel(engine::Level* level);
    void setScene(engine::Scene* scene);
    
private:
    void renderWorldView();
    void renderLevelView();
    void renderSceneView();  // Existing
    
    // Meshes
    std::unique_ptr<Mesh> m_boxMesh;    // For levels
    std::unique_ptr<Mesh> m_tileMesh;   // For scenes
    
    ViewLevel m_currentLevel = ViewLevel::Scene;
};
```

### New Mesh Primitives

```cpp
// Add to Mesh.cpp
static std::unique_ptr<Mesh> createBox(float w, float h, float d);
static std::unique_ptr<Mesh> createTile(float w, float d, float height = 0.1f);
static std::unique_ptr<Mesh> createArrow(float length);
```

---

## ViewportPanel Integration

### Unified Toolbar

```
[2D] [3D] | [S] [R] | Zoom: 100% ... | Grid | Hotspots | ...
```

Toggle works on ALL levels:
- World → 2D spatial map OR 3D box view
- Level → 2D scene grid OR 3D tile view  
- Scene → 2D editor OR 3D scene view

### Breadcrumb Navigation

Same as current, but clicking navigates both 2D and 3D:
```
🌍 World > 📁 Main Game > 🎬 Tavern
```

---

## Rendering Optimization

### Frustum Culling

```cpp
bool isInFrustum(const glm::mat4& mvp, const AABB& bounds) {
    // Test AABB against view frustum
    // Skip rendering if outside
}
```

### Level of Detail (LOD)

| View Level | Object Type | LOD |
|------------|-------------|-----|
| World | Level boxes | Low (solid color) |
| Level | Scene tiles | Medium (thumbnail) |
| Scene | Actors | Full (sprites/meshes) |

### Instanced Rendering

For many similar objects:
```cpp
void renderInstanced(const Mesh& mesh, 
                     const std::vector<glm::mat4>& transforms,
                     const std::vector<glm::vec3>& colors);
```

---

## Implementation Phases

### Phase 1: Unified Toggle ⬅️ START HERE
- [ ] Move 2D/3D toggle to all view levels
- [ ] Refactor ViewportPanel to use ViewportMode consistently
- [ ] Pass mode to World/Level views

### Phase 2: Level View 3D
- [ ] Create tile mesh primitive
- [ ] Render scenes as tiles
- [ ] Add connection lines
- [ ] Camera orbit for level

### Phase 3: World View 3D
- [ ] Create box mesh primitive  
- [ ] Render levels as boxes
- [ ] Camera for world overview
- [ ] Click to navigate

### Phase 4: Optimization
- [ ] Frustum culling
- [ ] Instanced rendering
- [ ] LOD system
- [ ] Thumbnail generation

---

## Success Criteria

1. ✅ 2D/3D toggle works on World, Level, Scene views
2. ✅ 3D World shows levels as navigable boxes
3. ✅ 3D Level shows scenes as connected tiles
4. ✅ Smooth navigation between levels
5. ✅ Performance: 60 FPS with 100+ scenes
6. ✅ No rendering when object outside frustum

---

*Created: 2026-01-08*
