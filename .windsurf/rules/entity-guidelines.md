---
trigger: glob
globs: ["src/entities/*.cpp", "src/entities/*.h"]
---

# Entity Guidelines

<base_entity>
Alla spelobjekt ärver från Entity:
`cpp
class Entity {
protected:
    int m_id;
    Vector2 m_position;
    bool m_active = true;
    
public:
    virtual ~Entity() = default;
    virtual void update(float dt) = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
    
    int getId() const { return m_id; }
    Vector2 getPosition() const { return m_position; }
    void setPosition(const Vector2& pos) { m_position = pos; }
    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }
};
`
</base_entity>

<entity_types>
| Entity | Ansvar |
|--------|--------|
| Player | Spelarkaraktär, input, animation |
| NPC | AI-styrda karaktärer, dialog |
| Item | Plockbara föremål |
| Hotspot | Interaktiva områden (dörrar, objekt) |
</entity_types>

<vector2>
Enkel 2D-vektor:
`cpp
struct Vector2 {
    float x = 0, y = 0;
    Vector2 operator+(const Vector2& o) const { return {x+o.x, y+o.y}; }
    Vector2 operator*(float s) const { return {x*s, y*s}; }
};
`
</vector2>
