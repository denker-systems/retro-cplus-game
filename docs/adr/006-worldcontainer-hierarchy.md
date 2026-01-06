# ADR 006: WorldContainer Abstract Base Class Pattern

## Status
✅ Accepted - Implemented 2026-01-05

## Context

Efter implementationen av Node Scene Graph-systemet behövde vi migrera till UE5-inspirerad Actor-baserad arkitektur. Vi stod inför flera utmaningar:

1. **Code Duplication:** World, Level och Scene hade duplicerad funktionalitet för namn-management och actor-management
2. **Inkonsistent Interface:** Varje klass hade sina egna metoder för update/render utan gemensamt interface
3. **Node Dependency:** Scene ärvde från Node, vilket gjorde den kopplad till det gamla systemet
4. **Hierarki-behov:** Ville ha tydlig World → Level → Scene struktur inspirerad av UE5

## Decision

Skapa `WorldContainer` abstract base class som gemensam bas för World, Level och Scene.

### Design

```cpp
class WorldContainer {
public:
    // Pure virtual - Template Method Pattern
    virtual void update(float deltaTime) = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
    
    // Shared functionality
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
    
    void addActor(std::unique_ptr<ActorObjectExtended> actor);
    const std::vector<std::unique_ptr<ActorObjectExtended>>& getActors() const;
    ActorObjectExtended* findActor(const std::string& name) const;
    
protected:
    WorldContainer(const std::string& name) : m_name(name) {}
    virtual ~WorldContainer() = default;
    
    std::string m_name;
    std::vector<std::unique_ptr<ActorObjectExtended>> m_actors;
};
```

### Hierarki

```
WorldContainer (abstract)
├── World : public WorldContainer
│   └── Owns/manages Levels
│   └── Scene stack för overlays
├── Level : public WorldContainer
│   └── Owns/manages Scenes
│   └── Scene transitions
└── Scene : public WorldContainer
    └── Owns/manages Actors
    └── Camera via CameraComponent
```

### Mappning till UE5

| Retro Adventure | Unreal Engine 5 | Ansvar |
|-----------------|-----------------|---------|
| WorldContainer | - (interface) | Abstract base |
| World | UWorld | Högsta nivån, owns levels |
| Level | ULevel | Mellanled, owns sub-levels |
| Scene | Sub-level/Streaming level | Actor container |
| ActorObjectExtended | AActor | Spelentiteter |

## Consequences

### Positiva

✅ **Eliminerad duplicering:** getName/setName/addActor/getActors i EN plats  
✅ **Konsistent interface:** Alla använder update()/render() via polymorfism  
✅ **UE5-alignment:** Tydlig mappning till Unreal Engine-koncept  
✅ **Framtidssäker:** Redo för full Actor/Component-migration  
✅ **Type safety:** Template Method Pattern garanterar korrekt interface  
✅ **Node-fri Scene:** Scene har INGEN koppling till Node-systemet längre

### Negativa

⚠️ **Breaking change:** Scene ärver inte längre från Node - kräver editor-uppdateringar  
⚠️ **Migration work:** Panels som använde Node-metoder måste omskrivas  
⚠️ **Hybrid solution:** Temporärt två renderingssystem (Node + RoomData)

### Mitigations

- **ViewportPanel:** Implementerade hybrid rendering via RoomData tills SpriteComponents är klara
- **RoomToSceneConverter:** Konverterar RoomData till Actors automatiskt
- **Panels disabled:** LayerEditorPanel och SceneGraphPanel disabled under migration
- **New panels:** WorldViewPanel och LevelViewPanel för ny hierarki-navigation

## Implementation Details

### Före och Efter

**Före:**
```cpp
// Scene.h
class Scene : public Node {
    void addChild(std::unique_ptr<Node> child);
    std::vector<std::unique_ptr<Node>> m_children;
    std::string m_name;
};

// World.h
class World {
    void update(float deltaTime);
    std::string m_name;
};
```

**Efter:**
```cpp
// WorldContainer.h
class WorldContainer {
    virtual void update(float deltaTime) = 0;
protected:
    std::string m_name;
    std::vector<std::unique_ptr<ActorObjectExtended>> m_actors;
};

// Scene.h
class Scene : public WorldContainer {
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
};

// World.h
class World : public WorldContainer {
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
};
```

### Migrations-problem Lösta

1. **Editor rendering:** Hybrid RoomData rendering i ViewportPanel
2. **RoomToSceneConverter:** Skapar Actors istället för Nodes
3. **Editor panels:** Disabled Node-beroende panels, visar Actor count
4. **Duplicerad kod:** WorldContainer eliminerar duplication

## Related

- **ADR 005:** Separate Engine, Game and Editor (arkitekturgrund)
- **Fas 4:** Node Scene Graph system (vad vi migrerar från)
- **Fas 5:** Actor-Based Architecture (vad vi migrerar till)

## Files Changed

- **Nya filer:** WorldContainer.h, Level.h/cpp, RoomToSceneConverter.h/cpp
- **Modifierade:** Scene.h/cpp, World.h/cpp, ViewportPanel.cpp, EditorState.cpp
- **Disabled:** LayerEditorPanel.cpp, SceneGraphPanel.cpp (temporärt)
- **Build:** CMakeLists.txt uppdaterad med nya filer

## Statistics

- 90+ filer ändrade
- 1,215 insertions, 531 deletions
- 7 nya filer
- 3 nya klasser (WorldContainer, Level, RoomToSceneConverter)

## Next Steps

- [ ] Implementera SpriteComponent för rendering
- [ ] Implementera InteractionComponent för hotspots
- [ ] Migrera walk area till physics-baserad Actor
- [ ] Ta bort Node/Node2D helt från projektet
- [ ] Återställ LayerEditorPanel med Actor-baserad design
