# Arkitektur Översikt

## Systemdiagram

```
┌─────────────────────────────────────────────────────────────┐
│                         Game                                 │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐           │
│  │ States  │ │Entities │ │Managers │ │ Systems │           │
│  └────┬────┘ └────┬────┘ └────┬────┘ └────┬────┘           │
└───────┼──────────┼──────────┼──────────┼───────────────────┘
        │          │          │          │
        ▼          ▼          ▼          ▼
   MenuState   Character  TextureManager  RoomSystem
   PlayState   Item       AudioManager    DialogSystem
   Options     Hotspot                    QuestSystem
```

## Moduler

| Modul | Beskrivning | Status |
|-------|-------------|--------|
| [States](states.md) | Game state management | ✅ |
| [Entities](entities.md) | Spelobjekt hierarki | 📋 Planerad |
| [Components](components.md) | Delad funktionalitet | 📋 Planerad |
| [Systems](systems.md) | Spelsystem | 📋 Planerad |

## Design Patterns

- **State Pattern** - Game states
- **Singleton** - Managers
- **Composition** - Entity components
- **Observer** - Event system (framtida)

## Se även

- [Entity Hierarki](entities.md)
- [State Pattern](states.md)
