# Legacy Node System - Deprecation Plan

**Status:** ⚠️ DEPRECATED (2026-01-05)  
**Replacement:** ActorObject / ActorObjectExtended

---

## Nuvarande Situation

### Node-baserade klasser som fortfarande finns:

```
Node (DEPRECATED)
└── Node2D (DEPRECATED)
    ├── Sprite
    ├── AnimatedSprite
    ├── Label
    ├── TileMapLayer
    ├── ParallaxLayer
    ├── InteractiveArea
    ├── WalkArea
    └── Marker
```

### Vad som hänt:

✅ **Fas 5A-5C Klart:**
- Object & ActorObject skapade
- ActorObjectExtended med components
- SpriteComponent & AnimationComponent fungerande
- RoomToSceneConverter skapar SpriteActors
- ViewportPanel renderar Actors

⚠️ **Legacy Nodes Kvar:**
- Används fortfarande av gamla Sprite/Label/etc
- ViewportPanel har `renderSceneNode()` för bakåtkompatibilitet
- Inga nya features ska använda Nodes

---

## Migration Plan

### Fas 1: Markera som DEPRECATED ✅ KLART
- [x] Lägg till deprecation warnings i Node.h
- [x] Lägg till deprecation warnings i Node2D.h
- [x] Uppdatera CoreRedirects.h
- [x] Dokumentera i LEGACY_NODES.md

### Fas 2: Ersätt Node-användning
- [ ] Migrera Sprite → SpriteActor + SpriteComponent
- [ ] Migrera AnimatedSprite → SpriteActor + AnimationComponent
- [ ] Migrera Label → TextActor + TextComponent (ny)
- [ ] Migrera InteractiveArea → InteractiveActor
- [ ] Migrera WalkArea → WalkAreaActor (ny)
- [ ] Migrera Marker → PropActor eller DebugActor

### Fas 3: Ta bort Node-system
- [ ] Ta bort `renderSceneNode()` från ViewportPanel
- [ ] Ta bort Node/Node2D includes från editor
- [ ] Kommentera ut Node/Node2D klasser
- [ ] Eventuellt flytta till `src/engine/legacy/` mapp

---

## Hur man migrerar kod

### Före (Node-baserad):
```cpp
auto sprite = std::make_unique<Sprite>("MySprite");
sprite->setTexture(texture);
sprite->setPosition(100, 100);
scene->addChild(std::move(sprite));
```

### Efter (Actor-baserad):
```cpp
auto sprite = std::make_unique<SpriteActor>("MySprite");
sprite->initializeSprite("assets/sprites/texture.png");
sprite->setPosition(100, 100);
scene->addActor(std::move(sprite));
```

---

## Viktig Information

⚠️ **Använd INTE Node/Node2D i ny kod!**

✅ **Använd istället:**
- `ActorObject` - För enkel transform
- `ActorObjectExtended` - För component support
- `SpriteActor` - För sprites
- `CharacterActor` - För player/NPCs
- `EnvironmentActor` - För miljö

---

## Timeline

- **2026-01-05:** Fas 1 klar (DEPRECATED markering)
- **2026-01-06+:** Fas 2 (Migration)
- **2026-01-08+:** Fas 3 (Borttagning)

**Estimerad tid:** 2-3 sessioner för komplett migration
