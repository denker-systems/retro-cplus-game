# Editor Panels API Documentation

## Overview

Editor panels är ImGui-baserade UI-komponenter för visuell redigering av game content. Alla panels implementerar `IEditorPanel` interface och är dockbara i editor-layouten.

**Location:** `src/editor/panels/`  
**Base Interface:** `IEditorPanel`

---

## IEditorPanel Interface

```cpp
class IEditorPanel {
public:
    virtual ~IEditorPanel() = default;
    
    virtual const std::string& getId() const = 0;
    virtual const std::string& getTitle() const = 0;
    virtual void render() = 0;
    virtual void update(float deltaTime) {}
    
    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }
    void toggleVisible() { m_visible = !m_visible; }
    
protected:
    bool m_visible = true;
};
```

---

## SceneGraphPanel

Hierarkisk node tree viewer med drag-and-drop re-parenting.

### Header

```cpp
class SceneGraphPanel : public IEditorPanel {
public:
    explicit SceneGraphPanel(EditorContext& context);
    
    const std::string& getId() const override;
    const std::string& getTitle() const override;
    void render() override;
    
    void setScene(engine::Scene* scene);
    
private:
    void renderNodeTree(engine::Node* node);
    void renderNodeContextMenu(engine::Node* node);
    
    EditorContext& m_context;
    engine::Scene* m_scene = nullptr;
    engine::Node* m_selectedNode = nullptr;
    bool m_showInactive = true;
};
```

### Features

#### 1. Hierarchical Tree View

Visar alla nodes i scenen som ett expanderbart träd.

```cpp
sceneGraphPanel->setScene(currentScene);
sceneGraphPanel->render();
```

**Tree Flags:**
- `ImGuiTreeNodeFlags_OpenOnArrow` - Expand med pil
- `ImGuiTreeNodeFlags_OpenOnDoubleClick` - Expand med dubbelklick
- `ImGuiTreeNodeFlags_Selected` - Highlight selected node
- `ImGuiTreeNodeFlags_Leaf` - Node utan children

#### 2. Node Selection

Click på node för att välja den.

```cpp
if (ImGui::IsItemClicked()) {
    m_selectedNode = node;
}
```

#### 3. Drag-and-Drop Re-parenting

Dra nodes för att ändra parent/child-relationer.

**Implementation:**

```cpp
// Drag source
if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
    ImGui::SetDragDropPayload("SCENE_NODE", &node, sizeof(engine::Node*));
    ImGui::Text("Move: %s", node->getName().c_str());
    ImGui::EndDragDropSource();
}

// Drop target
if (ImGui::BeginDragDropTarget()) {
    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_NODE")) {
        engine::Node* draggedNode = *(engine::Node**)payload->Data;
        
        // Transfer ownership
        auto ownership = draggedNode->getParent()->removeChild(draggedNode);
        newParent->addChild(std::move(ownership));
    }
    ImGui::EndDragDropTarget();
}
```

**Key Points:**
- Använder unique_ptr för ownership transfer
- Förhindrar circular dependencies (node kan inte bli sitt eget barn)
- Uppdaterar transform hierarchy automatiskt

#### 4. Context Menu

Högerklick på node för context menu.

**Options:**
- **Rename** - Ändra node namn (TODO)
- **Duplicate** - Kopiera node (TODO)
- **Active** - Toggle active state
- **Visible** - Toggle visible state
- **Delete** - Ta bort node (TODO)

```cpp
if (ImGui::BeginPopupContextItem()) {
    bool active = node->isActive();
    if (ImGui::MenuItem("Active", nullptr, &active)) {
        node->setActive(active);
    }
    
    bool visible = node->isVisible();
    if (ImGui::MenuItem("Visible", nullptr, &visible)) {
        node->setVisible(visible);
    }
    
    ImGui::EndPopup();
}
```

#### 5. Filter Options

- **Show Inactive** - Checkbox för att visa/dölja inactive nodes

---

## LayerEditorPanel

Layer management med parallax preview.

### Header

```cpp
class LayerEditorPanel : public IEditorPanel {
public:
    explicit LayerEditorPanel(EditorContext& context);
    
    const std::string& getId() const override;
    const std::string& getTitle() const override;
    void render() override;
    
    void setLayerManager(engine::LayerManager* manager);
    void setParallaxPreview(bool enable);
    
private:
    void renderLayerList();
    void renderLayerProperties();
    void renderAddLayerDialog();
    void renderParallaxPreview();
    
    EditorContext& m_context;
    engine::LayerManager* m_layerManager = nullptr;
    engine::Layer* m_selectedLayer = nullptr;
    
    bool m_showAddDialog = false;
    bool m_parallaxPreview = false;
    float m_previewScroll = 0.0f;
};
```

### Features

#### 1. Layer List

Visar alla layers med type-ikoner och z-index.

**Icons:**
- `B` - Background
- `M` - Main
- `F` - Foreground
- `U` - UI

```cpp
for (const auto& layer : layers) {
    const char* icon = getIconForType(layer->getLayerType());
    std::string label = std::string(icon) + " " + layer->getName() + 
                       " (z:" + std::to_string(layer->getZIndex()) + ")";
    
    if (ImGui::Selectable(label.c_str(), isSelected)) {
        m_selectedLayer = layer.get();
    }
}
```

#### 2. Layer Properties

Edit properties för selected layer.

**Editable Properties:**
- **Z-Index** - DragInt för rendering order
- **Opacity** - SliderFloat (0.0 - 1.0)
- **Active** - Checkbox
- **Visible** - Checkbox

```cpp
int zIndex = layer->getZIndex();
if (ImGui::DragInt("Z-Index", &zIndex)) {
    layer->setZIndex(zIndex);
}

float opacity = layer->getOpacity();
if (ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f)) {
    layer->setOpacity(opacity);
}
```

#### 3. Add Layer Dialog

Modal dialog för att skapa nya layers.

```cpp
if (ImGui::Button("Add Layer")) {
    m_showAddDialog = true;
}

if (ImGui::BeginPopupModal("Add Layer", &m_showAddDialog)) {
    ImGui::InputText("Name", m_newLayerName, sizeof(m_newLayerName));
    
    const char* typeNames[] = {"Background", "Main", "Foreground", "UI"};
    ImGui::Combo("Type", &m_newLayerType, typeNames, 4);
    
    ImGui::DragInt("Z-Index", &m_newLayerZIndex);
    
    if (ImGui::Button("Create")) {
        LayerType type = static_cast<LayerType>(m_newLayerType);
        m_layerManager->addLayer(m_newLayerName, type, m_newLayerZIndex);
        m_showAddDialog = false;
    }
    
    ImGui::EndPopup();
}
```

#### 4. Parallax Preview

Live scrolling preview för background layers.

**Features:**
- Scroll slider (0-1000)
- Visual representation av parallax effect
- Visar alla background layers samtidigt

```cpp
void LayerEditorPanel::renderParallaxPreview() {
    ImGui::SliderFloat("Scroll Position", &m_previewScroll, 0.0f, 1000.0f);
    
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize(400, 100);
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Background
    drawList->AddRectFilled(canvasPos, 
                           ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
                           IM_COL32(30, 30, 30, 255));
    
    // Simulate parallax layers
    for (const auto& layer : m_layerManager->getLayers()) {
        if (layer->getLayerType() == LayerType::Background) {
            float parallaxFactor = 0.5f;  // Could be layer-specific
            float offset = m_previewScroll * parallaxFactor;
            float x = canvasPos.x + std::fmod(offset, canvasSize.x);
            
            // Draw layer representation
            drawList->AddRectFilled(
                ImVec2(x, canvasPos.y + 20),
                ImVec2(x + 50, canvasPos.y + 80),
                IM_COL32(100, 150, 200, 200)
            );
            
            drawList->AddText(ImVec2(x + 5, canvasPos.y + 40), 
                            IM_COL32(255, 255, 255, 255), 
                            layer->getName().c_str());
        }
    }
    
    ImGui::Dummy(canvasSize);
}
```

**Use Case:**
Visualisera hur background layers scrollar i olika hastigheter för att skapa depth effect.

---

## TileMapEditorPanel

Interactive tilemap painting med auto-tiling.

### Header

```cpp
class TileMapEditorPanel : public IEditorPanel {
public:
    explicit TileMapEditorPanel(EditorContext& context);
    
    const std::string& getId() const override;
    const std::string& getTitle() const override;
    void render() override;
    
    void setTileMap(engine::TileMapLayer* tilemap);
    
private:
    void renderTilePalette();
    void renderTileMapView();
    void renderToolbar();
    void autoTileNeighbors(int x, int y);
    
    EditorContext& m_context;
    engine::TileMapLayer* m_tilemap = nullptr;
    int m_selectedTile = 1;
    bool m_paintMode = true;
    bool m_collisionMode = false;
};
```

### Features

#### 1. Tile Palette

4x4 grid för tile selection.

```cpp
for (int i = 0; i < 16; i++) {
    if (i % 4 != 0) ImGui::SameLine();
    
    bool selected = (m_selectedTile == i + 1);
    if (ImGui::Selectable(std::to_string(i + 1).c_str(), selected, 0, ImVec2(40, 40))) {
        m_selectedTile = i + 1;
    }
}
```

#### 2. Toolbar

Mode toggles och settings.

```cpp
if (ImGui::RadioButton("Paint", m_paintMode)) {
    m_paintMode = true;
}
ImGui::SameLine();
if (ImGui::RadioButton("Erase", !m_paintMode)) {
    m_paintMode = false;
}
ImGui::SameLine();
ImGui::Checkbox("Collision Mode", &m_collisionMode);
```

#### 3. Interactive Grid Painting

Click-and-drag för att måla tiles.

```cpp
const float cellSize = 32.0f;
ImVec2 canvasPos = ImGui::GetCursorScreenPos();
ImDrawList* drawList = ImGui::GetWindowDrawList();

// Draw grid
for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
        ImVec2 p1(canvasPos.x + x * cellSize, canvasPos.y + y * cellSize);
        ImVec2 p2(p1.x + cellSize, p1.y + cellSize);
        
        int tile = m_tilemap->getTile(x, y);
        ImU32 color = tile > 0 ? IM_COL32(100, 100, 200, 255) : IM_COL32(50, 50, 50, 255);
        
        drawList->AddRectFilled(p1, p2, color);
        
        if (tile > 0) {
            char buf[8];
            snprintf(buf, sizeof(buf), "%d", tile);
            drawList->AddText(ImVec2(p1.x + 8, p1.y + 8), IM_COL32(255, 255, 255, 255), buf);
        }
        
        drawList->AddRect(p1, p2, IM_COL32(100, 100, 100, 255));
    }
}

// Handle mouse input
ImVec2 canvasSize(width * cellSize, height * cellSize);
ImGui::InvisibleButton("canvas", canvasSize);

if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0)) {
    ImVec2 mousePos = ImGui::GetMousePos();
    int gridX = (int)((mousePos.x - canvasPos.x) / cellSize);
    int gridY = (int)((mousePos.y - canvasPos.y) / cellSize);
    
    if (gridX >= 0 && gridX < width && gridY >= 0 && gridY < height) {
        if (m_paintMode) {
            m_tilemap->setTile(gridX, gridY, m_selectedTile);
            autoTileNeighbors(gridX, gridY);
        } else {
            m_tilemap->setTile(gridX, gridY, 0);
        }
    }
}
```

#### 4. Auto-tiling Algorithm

Neighbor-based tile variant selection.

```cpp
void TileMapEditorPanel::autoTileNeighbors(int x, int y) {
    int neighbors = 0;
    
    // Check 4 cardinal directions
    if (x > 0 && m_tilemap->getTile(x - 1, y) > 0) neighbors |= 1;  // Left
    if (x < width - 1 && m_tilemap->getTile(x + 1, y) > 0) neighbors |= 2;  // Right
    if (y > 0 && m_tilemap->getTile(x, y - 1) > 0) neighbors |= 4;  // Up
    if (y < height - 1 && m_tilemap->getTile(x, y + 1) > 0) neighbors |= 8;  // Down
    
    // Adjust tile based on neighbor configuration
    int currentTile = m_tilemap->getTile(x, y);
    if (currentTile > 0 && neighbors > 0) {
        // Could adjust tile variant here based on neighbors
        // Example: corners, edges, fills
        // int variant = calculateVariant(neighbors);
        // m_tilemap->setTile(x, y, baseType + variant);
    }
}
```

**Neighbor Bitmask:**
```
0001 (1)  = Left
0010 (2)  = Right
0100 (4)  = Up
1000 (8)  = Down

Examples:
0011 (3)  = Left + Right (horizontal)
1100 (12) = Up + Down (vertical)
1111 (15) = All sides (center tile)
```

---

## Panel Integration

### EditorState Setup

```cpp
class EditorState {
    std::unique_ptr<SceneGraphPanel> m_sceneGraphPanel;
    std::unique_ptr<LayerEditorPanel> m_layerEditorPanel;
    std::unique_ptr<TileMapEditorPanel> m_tileMapEditorPanel;
    
    void enter() override {
        m_sceneGraphPanel = std::make_unique<SceneGraphPanel>(m_editorContext);
        m_layerEditorPanel = std::make_unique<LayerEditorPanel>(m_editorContext);
        m_tileMapEditorPanel = std::make_unique<TileMapEditorPanel>(m_editorContext);
    }
    
    void renderImGui() override {
        if (m_sceneGraphPanel) m_sceneGraphPanel->render();
        if (m_layerEditorPanel) m_layerEditorPanel->render();
        if (m_tileMapEditorPanel) m_tileMapEditorPanel->render();
    }
};
```

### Dock Layout

```cpp
ImGui::DockBuilderDockWindow("Scene Graph", dockCenter);
ImGui::DockBuilderDockWindow("Layer Editor", dockCenter);
ImGui::DockBuilderDockWindow("TileMap Editor", dockCenter);
```

Alla panels dockas som tabs i center dock.

---

## Best Practices

### 1. Null Checks

Kontrollera alltid att data finns innan rendering.

```cpp
void render() override {
    if (!m_visible) return;
    
    if (!ImGui::Begin(m_title.c_str(), &m_visible)) {
        ImGui::End();
        return;
    }
    
    if (!m_scene) {
        ImGui::TextDisabled("No scene loaded");
        ImGui::End();
        return;
    }
    
    // Render content...
    ImGui::End();
}
```

### 2. Ownership Transfer

Vid drag-and-drop, använd unique_ptr korrekt.

```cpp
// ✅ Correct
auto ownership = oldParent->removeChild(node);
newParent->addChild(std::move(ownership));

// ❌ Incorrect
newParent->addChild(node);  // Compile error - can't convert raw pointer
```

### 3. ImGui State

Matcha alla Begin/End calls.

```cpp
// ✅ Correct
if (ImGui::Begin("Panel", &m_visible)) {
    // Content
}
ImGui::End();  // Always call End

// ❌ Incorrect
if (ImGui::Begin("Panel", &m_visible)) {
    if (condition) return;  // Missing End!
    // Content
}
ImGui::End();
```

---

## See Also

- [IEditorPanel](IEditorPanel.md) - Base interface
- [EditorContext](EditorContext.md) - Shared editor state
- [Node Scene Graph](../features/NodeSceneGraph.md) - Complete system documentation

---

**Created:** 2026-01-05  
**Last Updated:** 2026-01-05
