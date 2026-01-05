# Entity Hierarki

## Översikt

Entity-systemet följer principen **"Composition over Inheritance"** med max 3 arvsnivåer.

## Hierarki

```
Entity (abstract)
│   - position, size, active
│   - update(), render()
│
├── Character (abstract)
│   │   - health, name
│   │   - MovementComponent
│   │   - AnimationComponent
│   │
│   ├── PlayerCharacter
│   │       - inventory
│   │       - point-and-click
│   │
│   └── AICharacter (abstract)
│       │   - AI behavior
│       │
│       └── NPCCharacter
│               - DialogComponent
│               - schedule
│
├── Item
│       - pickupable
│       - combinable
│
└── Hotspot
        - interactable
        - triggers
```

## Klasser

| Klass | Fil | Status |
|-------|-----|--------|
| Entity | `src/entities/Entity.h` | 📋 Planerad |
| Character | `src/entities/Character.h/.cpp` | 📋 Planerad |
| PlayerCharacter | `src/entities/PlayerCharacter.h/.cpp` | 📋 Planerad |
| AICharacter | `src/entities/AICharacter.h/.cpp` | 📋 Planerad |
| NPCCharacter | `src/entities/NPCCharacter.h/.cpp` | 📋 Planerad |

## API Dokumentation

- [Entity](../api/entities/Entity.md)
- [Character](../api/entities/Character.md)
- [PlayerCharacter](../api/entities/PlayerCharacter.md)

## Se även

- [Components](components.md)
- [OOP Architecture Rules](../../.windsurf/rules/rules-oop-architecture.md)
