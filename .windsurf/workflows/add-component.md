---
description: Add new actor component with full documentation
---

# Add Component Workflow

> Enterprise component architecture for game engines

## Component Categories

| Category | Examples |
|----------|----------|
| Transform | TransformComponent |
| Rendering | SpriteComponent, MeshComponent |
| Physics | RigidBody2D, Collider2D |
| Audio | AudioSource, AudioListener |
| AI | AIController, BehaviorTree |
| Gameplay | Health, Inventory, Dialog |
| Animation | AnimationController |

---

## 1. Design

### 1.1 Component Purpose
```markdown
## Component: [Name]Component

### Purpose
What functionality does this component add to actors?

### Data
- Property 1: Type, default, description
- Property 2: Type, default, description

### Behavior
- When attached: ...
- During update: ...
- When detached: ...

### Dependencies
- Requires: [Other components]
- Optional: [Other components]
```

---

## 2. Implementation

### 2.1 Header File
Create `src/engine/components/[Name]Component.h`:

```cpp
/**
 * @file [Name]Component.h
 * @brief [Brief description]
 * 
 * @details
 * Component that provides [functionality].
 * Attach to actors that need [capability].
 */
#pragma once

#include "engine/core/ActorComponent.h"

namespace engine {

/**
 * @class [Name]Component
 * @brief [Brief description]
 * 
 * @details
 * Detailed description of the component.
 * 
 * @par Usage
 * @code
 * auto* component = actor->addComponent<[Name]Component>();
 * component->setProperty(value);
 * @endcode
 * 
 * @see ActorComponent
 */
class [Name]Component : public ActorComponent {
public:
    [Name]Component();
    ~[Name]Component() override;
    
    // ActorComponent interface
    void onAttach() override;
    void onDetach() override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    
    // Component API
    
    /// @brief Set property value
    /// @param value New value
    void setProperty(float value);
    
    /// @brief Get property value
    /// @return Current value
    float getProperty() const { return m_property; }
    
    // Serialization
    void serialize(nlohmann::json& json) const;
    void deserialize(const nlohmann::json& json);
    
private:
    float m_property = 0.0f;  ///< Property description
    bool m_enabled = true;     ///< Whether component is active
};

} // namespace engine
```

### 2.2 Implementation File
Create `src/engine/components/[Name]Component.cpp`:

```cpp
/**
 * @file [Name]Component.cpp
 * @brief Implementation of [Name]Component
 */
#include "[Name]Component.h"
#include "engine/core/ActorObject.h"
#include "engine/utils/Logger.h"

namespace engine {

[Name]Component::[Name]Component() {
    LOG_DEBUG("[Name]Component created");
}

[Name]Component::~[Name]Component() {
    LOG_DEBUG("[Name]Component destroyed");
}

void [Name]Component::onAttach() {
    LOG_DEBUG("[Name]Component attached to {}", getOwner()->getName());
    // Initialize component
}

void [Name]Component::onDetach() {
    LOG_DEBUG("[Name]Component detached");
    // Cleanup
}

void [Name]Component::update(float deltaTime) {
    if (!m_enabled) return;
    
    // Update logic
}

void [Name]Component::render(SDL_Renderer* renderer) {
    if (!m_enabled) return;
    
    // Render logic (if visual component)
}

void [Name]Component::setProperty(float value) {
    m_property = value;
}

void [Name]Component::serialize(nlohmann::json& json) const {
    json["property"] = m_property;
    json["enabled"] = m_enabled;
}

void [Name]Component::deserialize(const nlohmann::json& json) {
    if (json.contains("property")) {
        m_property = json["property"].get<float>();
    }
    if (json.contains("enabled")) {
        m_enabled = json["enabled"].get<bool>();
    }
}

} // namespace engine
```

---

## 3. Integration

### 3.1 Update CMakeLists.txt
```cmake
set(ENGINE_SOURCES
    # ... existing
    src/engine/components/[Name]Component.cpp
)
```

### 3.2 Include in Component Headers
```cpp
// In src/engine/components/Components.h (if exists)
#include "[Name]Component.h"
```

### 3.3 Register for Serialization (if using factory)
```cpp
ComponentFactory::registerComponent<[Name]Component>("[Name]Component");
```

---

## 4. Editor Integration (if applicable)

### 4.1 Property Editor
```cpp
void [Name]ComponentEditor::render() {
    ImGui::Text("[Name] Component");
    ImGui::Separator();
    
    float prop = m_component->getProperty();
    if (ImGui::DragFloat("Property", &prop, 0.1f)) {
        m_component->setProperty(prop);
    }
    
    bool enabled = m_component->isEnabled();
    if (ImGui::Checkbox("Enabled", &enabled)) {
        m_component->setEnabled(enabled);
    }
}
```

---

## 5. Testing

### 5.1 Unit Tests
```cpp
TEST([Name]ComponentTest, DefaultValues) {
    [Name]Component component;
    EXPECT_EQ(component.getProperty(), 0.0f);
}

TEST([Name]ComponentTest, SetProperty) {
    [Name]Component component;
    component.setProperty(5.0f);
    EXPECT_EQ(component.getProperty(), 5.0f);
}

TEST([Name]ComponentTest, Serialization) {
    [Name]Component original;
    original.setProperty(10.0f);
    
    nlohmann::json json;
    original.serialize(json);
    
    [Name]Component loaded;
    loaded.deserialize(json);
    
    EXPECT_EQ(loaded.getProperty(), 10.0f);
}
```

---

## 6. Documentation

Create `docs/api/components/[Name]Component.md`:

```markdown
# [Name]Component

> Brief description

## Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| property | float | 0.0 | Description |
| enabled | bool | true | Whether active |

## Usage

\`\`\`cpp
auto actor = std::make_unique<ActorObjectExtended>("MyActor");
auto* comp = actor->addComponent<[Name]Component>();
comp->setProperty(5.0f);
\`\`\`

## Methods

### setProperty(float value)
Sets the property value.

### float getProperty() const
Returns current property value.

## Events

- **onAttach:** Called when added to actor
- **onDetach:** Called when removed from actor

## See Also
- [ActorComponent](ActorComponent.md)
- [Related Component](RelatedComponent.md)
```

---

## 7. Build & Verify

```powershell
cd build
cmake --build . --config Release
ctest -C Release
```

---

## Checklist

- [ ] Header file with documentation
- [ ] Implementation file
- [ ] CMakeLists.txt updated
- [ ] Serialization support
- [ ] Editor integration (if needed)
- [ ] Unit tests
- [ ] API documentation
- [ ] Example usage
