# Editor Panels Guide

## Panel-baserad Arkitektur

Editorn använder en modulär panel-arkitektur där varje UI-komponent är en separat klass som implementerar `IEditorPanel` interfacet.

## Panel Interface

```cpp
class IEditorPanel {
public:
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

## Befintliga Paneler

### 1. HierarchyPanel

**Syfte:** Visar hierarkisk lista över alla game-objekt

**Location:** `src/editor/panels/HierarchyPanel.h/cpp`

**Features:**
- Collapsible headers för varje objekttyp
- Click för att välja objekt
- Synkroniserar med `EditorContext`

**Användning:**
```cpp
m_hierarchyPanel = std::make_unique<HierarchyPanel>(m_editorContext);
m_hierarchyPanel->render();
```

**Innehåll:**
- **Rooms** - Alla rum med namn
- **Dialogs** - Alla dialoger med ID
- **Quests** - Alla quests med titel
- **Items** - Alla items med namn
- **NPCs** - Alla NPCs med namn

---

### 2. PropertiesPanel

**Syfte:** Inspector för valt objekt

**Location:** `src/editor/panels/PropertiesPanel.h/cpp`

**Features:**
- Dynamisk rendering baserat på selection type
- Detaljerad information per objekttyp
- "Edit Room" knapp för visuell redigering

**Rendering Methods:**
```cpp
void renderRoomProperties();
void renderDialogProperties();
void renderQuestProperties();
void renderItemProperties();
void renderHotspotProperties();
```

**Room Properties:**
- ID, Name, Background
- Player Spawn koordinater
- Walk Area (min/max X/Y)
- Depth Scale (top/bottom)
- Hotspot lista (klickbar)
- Exit lista

**Dialog Properties:**
- ID, Start Node
- Node count
- Node lista med preview

**Quest Properties:**
- ID, Title, Description
- Objective lista

**Item Properties:**
- ID, Name, Description
- Icon, Combinable flag

**Hotspot Properties:**
- ID, Name
- Position (x, y)
- Size (w, h)
- Target Room (om exit)
- Dialog ID (om NPC)

---

### 3. ViewportPanel

**Syfte:** Visuell preview av rum med editing tools

**Location:** `src/editor/panels/ViewportPanel.h/cpp`

**Features:**
- Zoom 25%-400%
- Grid overlay
- Hotspot visualization
- Walk area visualization
- Player spawn marker

**Toolbar:**
```cpp
- Zoom: [−] [+] [100%]
- ☑ Grid
- ☑ Hotspots
- ☑ Walk Area
```

**Rendering:**
- **Background** - Rum-bakgrund (om laddad)
- **Walk Area** - Cyan rectangle
- **Hotspots** - Röda/gula rectangles med namn
- **Player Spawn** - Magenta cirkel med "Spawn" text

**Auto-loading:**
Laddar automatiskt rum-preview när `EditorContext.selectedRoomId` ändras.

---

### 4. AssetBrowserPanel

**Syfte:** Browser för game assets (sprites, sounds, etc.)

**Location:** `src/editor/panels/AssetBrowserPanel.h/cpp`

**Features:**
- Grid/List view toggle
- Filter per asset type
- Search filter
- File size display
- Refresh funktion

**Asset Types:**
```cpp
enum class AssetType {
    All,
    Sprites,
    Backgrounds,
    Sounds,
    Music,
    Fonts
};
```

**Toolbar:**
```
[Refresh] [Grid/List] [Filter ▼] [Search...]
```

**Grid View:**
- 80x80 pixel celler
- Icon baserat på typ
- Truncated namn
- Tooltip med full info

**List View:**
- Tabell med kolumner: Name, Type, Size, Path
- Sorterad alfabetiskt
- Selectable rows

**Scanning:**
Skannar följande mappar:
- `assets/sprites/`
- `assets/backgrounds/`
- `assets/sounds/`
- `assets/music/`
- `assets/fonts/`

---

### 5. ConsolePanel

**Syfte:** Logg och status information

**Location:** `src/editor/panels/ConsolePanel.h/cpp`

**Features:**
- Timestamped log entries
- Log level filtering (Info, Warning, Error)
- Auto-scroll
- Clear funktion
- Stats display

**Log Levels:**
```cpp
enum class Level { Info, Warning, Error };
```

**API:**
```cpp
console->log("Normal message");           // Info (grå)
console->logWarning("Warning message");   // Warning (gul)
console->logError("Error message");       // Error (röd)
console->clear();                         // Rensa alla entries
```

**Toolbar:**
```
[Clear] ☑ Auto-scroll ☑ Info ☑ Warnings ☑ Errors
```

**Stats:**
Visar antal laddade objekt:
- Rooms, Dialogs, Quests, Items, NPCs

**Max Entries:** 1000 (FIFO)

---

## Skapa Ny Panel

### 1. Skapa Header

```cpp
// src/editor/panels/MyPanel.h
#pragma once
#include "../IEditorPanel.h"

class EditorContext;

class MyPanel : public IEditorPanel {
public:
    explicit MyPanel(EditorContext& context);
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    void render() override;
    void update(float deltaTime) override;
    
private:
    EditorContext& m_context;
    std::string m_id = "mypanel";
    std::string m_title = "My Panel";
    
    // Panel-specifik state här
};
```

### 2. Implementera CPP

```cpp
// src/editor/panels/MyPanel.cpp
#include "MyPanel.h"
#include "../EditorContext.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

MyPanel::MyPanel(EditorContext& context)
    : m_context(context) {
}

void MyPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        // Din ImGui-kod här
        ImGui::Text("Hello from MyPanel!");
    }
    ImGui::End();
#endif
}

void MyPanel::update(float deltaTime) {
    // Uppdatera panel-logik här
}
```

### 3. Lägg till i CMakeLists.txt

```cmake
set(EDITOR_SOURCES
    # ... andra filer ...
    src/editor/panels/MyPanel.cpp
)
```

### 4. Integrera i EditorState

```cpp
// EditorState.h
class MyPanel;

class EditorState : public IState {
    // ...
    std::unique_ptr<MyPanel> m_myPanel;
};

// EditorState.cpp
#include "../editor/panels/MyPanel.h"

void EditorState::enter() {
    // ...
    m_myPanel = std::make_unique<MyPanel>(m_editorContext);
}

void EditorState::renderImGui() {
    // I View-menyn
    if (ImGui::BeginMenu("View")) {
        bool myPanelVisible = m_myPanel && m_myPanel->isVisible();
        if (ImGui::MenuItem("My Panel", nullptr, &myPanelVisible)) {
            if (m_myPanel) m_myPanel->setVisible(myPanelVisible);
        }
    }
    
    // I docking setup
    ImGui::DockBuilderDockWindow("My Panel", dockSomewhere);
    
    // I render loop
    if (m_myPanel) m_myPanel->render();
}
```

## Best Practices

### State Management
- **Använd EditorContext** för delat state mellan paneler
- **Minimal lokal state** - Endast UI-specifikt state i panelen
- **Readonly DataLoader** - Läs från DataLoader, skriv via EditorContext

### Performance
- **Cacha tunga beräkningar** - T.ex. asset scanning
- **Lazy loading** - Ladda data först när panelen visas
- **Early return** - `if (!m_visible) return;` först i render()

### ImGui Guidelines
- **Använd Begin/End** - Alltid matcha `ImGui::Begin()` med `ImGui::End()`
- **PushID/PopID** - För unika IDs i loopar
- **Conditional rendering** - Använd `if (ImGui::CollapsingHeader())`
- **Tooltips** - `if (ImGui::IsItemHovered()) ImGui::SetTooltip(...)`

### Logging
```cpp
// Använd ConsolePanel för user feedback
if (m_consolePanel) {
    m_consolePanel->log("Operation completed");
    m_consolePanel->logWarning("Potential issue detected");
    m_consolePanel->logError("Operation failed");
}
```

### Error Handling
```cpp
void MyPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        try {
            // Panel-kod här
        } catch (const std::exception& e) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: %s", e.what());
        }
    }
    ImGui::End();
#endif
}
```

## Kommunikation Mellan Paneler

### Via EditorContext
```cpp
// HierarchyPanel sätter selection
m_context.selectedRoomId = roomId;
m_context.selectedType = EditorContext::SelectionType::Room;

// PropertiesPanel läser selection
if (m_context.selectedType == EditorContext::SelectionType::Room) {
    renderRoomProperties();
}
```

### Via Callbacks
```cpp
// HierarchyPanel.h
using SelectionCallback = std::function<void(const std::string&)>;
void setOnRoomSelected(SelectionCallback cb);

// EditorState.cpp
m_hierarchyPanel->setOnRoomSelected([this](const std::string& roomId) {
    // Custom handling här
    m_consolePanel->log("Room selected: " + roomId);
});
```

## Testing

### Manual Testing
1. Bygg editor: `cmake --build build --config Release --target RetroEditor`
2. Kör: `.\build\Release\RetroEditor.exe`
3. Testa panel visibility via View-menyn
4. Testa docking genom att dra panel-tabs
5. Verifiera att data visas korrekt

### Debug Tips
- Aktivera ImGui Demo för att testa widgets
- Använd Console-panelen för debug output
- Kontrollera att `#ifdef HAS_IMGUI` används korrekt
- Verifiera att panelen läggs till i CMakeLists.txt
