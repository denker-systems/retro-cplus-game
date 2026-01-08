# 3D Viewport Implementation Checklist

> Retro Engine 3D Rendering Implementation

## Overview
This checklist tracks the implementation of 3D viewport and rendering capabilities in Retro Engine, allowing seamless switching between 2D and 3D modes.

---

## Phase 1: OpenGL Context & Dependencies

### Dependencies Setup
- [x] Add dependencies to `vcpkg.json`:
  ```json
  {
    "dependencies": [
      "glew",
      "glm", 
      "assimp",
      "imguizmo"
    ]
  }
  ```
- [x] Install dependencies via vcpkg
- [x] Update `CMakeLists.txt` to find and link new packages
- [x] Test build with new dependencies

### OpenGL Context
- [x] Create `src/engine/graphics/GLContext.h`
- [x] Implement `GLContext.cpp` with:
  - OpenGL 3.3 Core Profile setup
  - GLEW initialization
  - SDL_GL context creation
  - Error handling
- [x] Integrate GLContext in RetroEditor
- [x] Verify OpenGL context creation works (build succeeded)

---

## Phase 2: Framebuffer & Basic 3D Rendering

### Framebuffer System
- [x] Create `src/engine/graphics/Framebuffer.h`
- [x] Implement `Framebuffer.cpp` with:
  - Color texture attachment
  - Depth buffer attachment
  - Dynamic resize support
  - Bind/unbind methods
- [x] Test framebuffer creation and resizing

### Shader System
- [x] Create `src/engine/graphics/Shader.h`
- [x] Implement `Shader.cpp` with:
  - Vertex/Fragment shader loading
  - Uniform caching
  - Setters for common types (mat4, vec3, float)
- [x] Create basic vertex shader (inline in Viewport3DPanel)
- [x] Create basic fragment shader (inline in Viewport3DPanel)
- [x] Test shader compilation and uniform setting

### Basic 3D Rendering
- [x] Create simple test shader program
- [x] Render basic colored cube to framebuffer
- [x] Verify framebuffer texture can be displayed

---

## Phase 3: 3D Camera & Scene

### Editor Camera
- [x] Create `src/editor/viewport/EditorCamera3D.h`
- [x] Implement `EditorCamera3D.cpp` with:
  - Orbit camera controls
  - Mouse wheel zoom
  - Pan with middle mouse
  - View matrix calculation
  - Projection matrix (perspective)
- [x] Add camera movement with WASD/QE
- [x] Test camera movement and controls

### Basic Mesh System
- [x] Create `src/engine/graphics/Mesh.h`
- [x] Implement `Mesh.cpp` with:
  - Vertex/index buffer management
  - VAO creation
  - Render method
- [x] Implement `createCube()` primitive
- [x] Implement `createPlane()` primitive
- [x] Implement `createGrid()` primitive
- [x] Test mesh rendering with basic shader

### Scene Rendering
- [x] Basic scene rendering in Viewport3DPanel
- [x] Add MVP matrix calculation
- [x] Render grid at origin
- [x] Render test cube
- [x] Verify proper depth testing

---

## Phase 4: ImGui 3D Viewport Integration

### Viewport3DPanel
- [x] Create `src/editor/viewport/Viewport3DPanel.h`
- [x] Implement `Viewport3DPanel.cpp` with:
  - Framebuffer integration
  - ImGui window setup
  - Dynamic framebuffer resize
  - Display framebuffer as ImGui::Image
  - Basic input handling
- [x] Add 3D viewport to editor panel manager
- [x] Test viewport rendering and interaction

### Input Handling
- [x] Handle mouse movement for camera (right-click orbit)
- [x] Handle mouse scroll for zoom
- [x] Handle keyboard input
- [x] Only process input when viewport is focused/hovered
- [x] Test all input controls

---

## Phase 5: ImGuizmo Integration

### Transform Gizmos
- [x] Create `src/editor/gizmos/TransformGizmo3D.h`
- [x] Implement `TransformGizmo3D.cpp` with:
  - Translate gizmo
  - Rotate gizmo
  - Scale gizmo
  - Local/World mode toggle
  - Snap values
- [x] Integrate ImGuizmo with Viewport3DPanel
- [x] Add gizmo operation selector in UI
- [ ] Test gizmo manipulation

### Object Selection
- [x] Implement basic ray casting
- [x] Select objects by clicking
- [x] Show gizmo for selected object
- [x] Transform selected object with gizmo
- [ ] Test selection and manipulation

---

## Phase 6: ViewportPanel 2D/3D Integration

### Mode Selector
- [x] Update `ViewportPanel.h` with ViewMode enum
- [x] Add mode selector UI (2D/3D toggle buttons)
- [x] Integrate Viewport3DPanel
- [x] Switch between 2D and 3D rendering
- [x] Maintain separate camera states
- [x] Test mode switching

### Data Migration
- [x] Ensure 2D scene data works in 3D
- [x] Convert 2D positions to 3D (z=0)
- [x] Test with existing scenes

---

## Testing & Verification

### Build Tests
- [x] Build succeeds on all configurations
- [ ] No new warnings
- [x] All dependencies link correctly

### Functional Tests
- [x] 3D viewport opens without errors
- [x] Camera controls work smoothly
- [x] Grid renders correctly
- [x] Basic primitives render
- [x] Mode switching works
- [ ] No memory leaks (check with debugger)

### Performance Tests
- [x] FPS stays above 60 with simple scene
- [x] Viewport resize is smooth
- [x] No stuttering during camera movement

### Integration Tests
- [x] Editor launches correctly
- [x] 2D mode still works
- [x] 3D mode works independently
- [x] No crashes when switching modes

---

## Documentation

### Code Documentation
- [ ] All new classes have Doxygen comments
- [ ] Complex algorithms explained
- [ ] Usage examples in headers

### User Documentation
- [ ] Update README with 3D features
- [ ] Add 3D viewport controls to manual
- [ ] Create tutorial for basic 3D editing

### Technical Documentation
- [ ] Document architecture decisions
- [ ] Create ADR for 3D rendering approach
- [ ] Update CHANGELOG

---

## Known Issues & Limitations

### Current Limitations
- [ ] No lighting system (basic directional only)
- [ ] No material system
- [ ] No animation support
- [ ] No physics in 3D

### Future Improvements
- [ ] PBR rendering
- [ ] Shadow mapping
- [ ] Post-processing effects
- [ ] 3D model importer with materials

---

## Review Checklist

### Before PR
- [ ] Code follows project coding standards
- [ ] No TODO/FIXME comments without issues
- [ ] All tests pass
- [ ] Documentation is complete
- [ ] Performance is acceptable

### Code Review Items
- [ ] OpenGL resource management is correct
- [ ] No memory leaks
- [ ] Error handling is robust
- [ ] Thread safety considered
- [ ] Platform compatibility checked

---

## Notes

### Implementation Decisions
- **OpenGL 3.3 Core**: Chosen for compatibility and modern features
- **GLEW**: Simpler setup than GLAD, works well with SDL2
- **Framebuffer approach**: Allows seamless ImGui integration
- **ImGuizmo**: Industry standard for 3D gizmos

### Performance Considerations
- Keep draw calls minimal
- Batch static geometry
- Use frustum culling for large scenes
- Consider instancing for many similar objects

### Security Notes
- Validate all loaded 3D models
- Check for buffer overflows in mesh loading
- Sanitize user input in gizmo manipulation

---

## Completion Criteria

The 3D viewport implementation is considered complete when:

1. ✅ All Phase 1-6 items are checked off
2. ✅ Editor can switch between 2D and 3D modes
3. ✅ Basic 3D objects can be created and manipulated
4. ✅ Camera controls are intuitive and responsive
5. ✅ No regressions in 2D functionality
6. ✅ Documentation is updated
7. ✅ Code passes all reviews

---

*Last Updated: 2026-01-08*
*Version: 1.0*
