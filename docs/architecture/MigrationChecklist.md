# Migration Checklist - UE5-liknande Hierarki

## 📋 ÖVERSIKT

**Status:** Sprint 1 - Foundation
**Startdatum:** 2026-01-05
**Estimerad färdigställning:** 6-8 veckor

---

## ✅ SPRINT 1: FOUNDATION (Vecka 1)

### Nya Filer att Skapa

- [ ] `src/engine/core/CoreRedirects.h` - Alias för backward compatibility
- [ ] `src/engine/core/ActorComponent.h` - Component bas-klass
- [ ] `src/engine/core/ActorComponent.cpp` - Component implementation
- [ ] `src/engine/core/SceneComponent.h` - Component med transform
- [ ] `src/engine/core/SceneComponent.cpp` - SceneComponent implementation
- [ ] `src/engine/core/ActorObjectExtended.h` - Extended ActorObject med components
- [ ] `src/engine/core/ActorObjectExtended.cpp` - ActorObjectExtended implementation
- [ ] `src/engine/core/MigrationConfig.h` - Feature flags för migration

### Filer att Modifiera

- [ ] `CMakeLists.txt` - Lägg till nya source files i CORE_SOURCES
- [ ] `src/engine/core/Node.h` - Lägg till deprecation warning (optional)
- [ ] `src/engine/core/Node2D.h` - Lägg till deprecation warning (optional)

### Tester

- [ ] Kompilera projektet - ska fungera utan errors
- [ ] Verifiera att gamla Node/Node2D fungerar
- [ ] Verifiera att nya Object/ActorObject fungerar
- [ ] Verifiera att ActorComponent kan skapas

### Dokumentation

- [ ] Uppdatera `docs/architecture/Final-Hierarchy.md` med implementation status
- [ ] Dokumentera i DEVLOG

---

## ✅ SPRINT 2: FIRST COMPONENT (Vecka 2)

### Nya Filer att Skapa

- [ ] `src/engine/components/SpriteComponent.h` - Ny sprite component
- [ ] `src/engine/components/SpriteComponent.cpp` - SpriteComponent implementation
- [ ] `src/engine/core/MigrationHelpers.h` - Adapter helpers
- [ ] `src/engine/core/MigrationHelpers.cpp` - Adapter implementation

### Filer att Modifiera

- [ ] `CMakeLists.txt` - Lägg till SpriteComponent i CORE_SOURCES

### Tester

- [ ] Skapa test-actor med SpriteComponent
- [ ] Jämför rendering: gamla Sprite vs SpriteComponent
- [ ] Benchmark: Ingen prestanda-försämring
- [ ] Skapa **ETT** test-rum med nya systemet

### Dokumentation

- [ ] Dokumentera SpriteComponent API
- [ ] Dokumentera skillnader gamla vs nya
- [ ] Dokumentera i DEVLOG

---

## ✅ SPRINT 3: SCENE ROUTING (Vecka 3)

### Filer att Modifiera

- [ ] `src/engine/world/Scene.h` - Lägg till addActor() method
- [ ] `src/engine/world/Scene.cpp` - Implementera routing logic
- [ ] `src/engine/world/Scene.h` - Lägg till m_actors vector
- [ ] `src/engine/world/Scene.cpp` - Uppdatera update() och render()

### Tester

- [ ] Testa addChild() med gamla Node - ska fungera
- [ ] Testa addActor() med nya ActorObjectExtended - ska fungera
- [ ] Testa parallell körning - båda samtidigt
- [ ] Verifiera rendering av båda systemen

### Dokumentation

- [ ] Dokumentera Scene API ändringar
- [ ] Dokumentera routing logic
- [ ] Dokumentera i DEVLOG

---

## ✅ SPRINT 4: PAWN + CONTROLLER (Vecka 4)

### Nya Filer att Skapa

- [ ] `src/engine/actors/Pawn.h` - Pawn bas-klass
- [ ] `src/engine/actors/Pawn.cpp` - Pawn implementation
- [ ] `src/engine/actors/Controller.h` - Controller bas-klass
- [ ] `src/engine/actors/Controller.cpp` - Controller implementation
- [ ] `src/engine/actors/PlayerController.h` - PlayerController
- [ ] `src/engine/actors/PlayerController.cpp` - PlayerController implementation
- [ ] `src/engine/actors/AIController.h` - AIController
- [ ] `src/engine/actors/AIController.cpp` - AIController implementation

### Filer att Modifiera

- [ ] `CMakeLists.txt` - Lägg till nya actors i CORE_SOURCES
- [ ] `src/engine/entities/Character.h` - Ärva från Pawn istället för Entity
- [ ] `src/engine/entities/Character.cpp` - Uppdatera implementation

### Tester

- [ ] Skapa test-pawn och possess med Controller
- [ ] Testa PlayerController med input
- [ ] Testa AIController med behavior
- [ ] Verifiera att Character fortfarande fungerar

### Dokumentation

- [ ] Dokumentera Pawn API
- [ ] Dokumentera Controller API
- [ ] Dokumentera possess/unpossess workflow
- [ ] Dokumentera i DEVLOG

---

## ✅ SPRINT 5: GRADVIS MIGRATION (Vecka 5-8)

### Rum att Migrera (Ett i taget)

- [ ] Tavern (test-rum)
- [ ] Town Square
- [ ] Shop
- [ ] Library
- [ ] Cellar
- [ ] Dark Forest
- [ ] Cave

### För Varje Rum

- [ ] Konvertera Sprite → SpriteComponent
- [ ] Konvertera NPCs → Character med Components
- [ ] Konvertera Items → ItemActor med Components
- [ ] Testa gameplay - ska fungera som innan
- [ ] Benchmark - ingen försämring
- [ ] Dokumentera lärdomar

### Filer att Modifiera

- [ ] `src/engine/world/RoomToSceneConverter.cpp` - Uppdatera till nya systemet
- [ ] Room JSON files (om nödvändigt)

### Dokumentation

- [ ] Dokumentera per rum-migration
- [ ] Samla lärdomar och patterns
- [ ] Uppdatera migration strategy om behövs

---

## ✅ SPRINT 6: DEPRECATION (Vecka 9+)

### Filer att Modifiera

- [ ] `src/engine/core/Node.h` - Lägg till [[deprecated]]
- [ ] `src/engine/core/Node2D.h` - Lägg till [[deprecated]]
- [ ] `src/engine/entities/Entity.h` - Lägg till [[deprecated]]

### Migration Cleanup

- [ ] Räkna legacy API usage i codebase
- [ ] Planera fasning ut av legacy
- [ ] Sätt deadline för removal
- [ ] Skapa compile-time checks (optional)

### Dokumentation

- [ ] Dokumentera deprecation timeline
- [ ] Uppdatera all dokumentation till nya systemet
- [ ] Migrera examples till nya API

---

## 📊 PROGRESS TRACKING

### Overall Progress

```
Sprint 1 (Foundation):        [ ] 0%
Sprint 2 (First Component):   [ ] 0%
Sprint 3 (Scene Routing):     [ ] 0%
Sprint 4 (Pawn + Controller): [ ] 0%
Sprint 5 (Gradvis Migration): [ ] 0%
Sprint 6 (Deprecation):       [ ] 0%

TOTAL PROGRESS: 0/6 sprints (0%)
```

### Files Created

```
Total New Files: 0/25
Total Modified Files: 0/15
```

### Legacy Code Removal

```
Node usage: 100%
Node2D usage: 100%
Entity usage: 100%

Target: 0% (alla migrerade)
```

---

## 🎯 CURRENT SPRINT: Sprint 1

### Today's Tasks

1. [ ] Skapa CoreRedirects.h
2. [ ] Skapa ActorComponent.h
3. [ ] Skapa ActorComponent.cpp
4. [ ] Skapa SceneComponent.h
5. [ ] Skapa SceneComponent.cpp
6. [ ] Uppdatera CMakeLists.txt
7. [ ] Testa kompilering

### Blockers

- Inga blockers

### Notes

- Inga git commits utan explicit instruktion från användaren
- Feature flags: TBD
- Deprecation warnings: TBD
- Deadline för legacy removal: TBD

---

## 📝 DECISION LOG

| Datum | Beslut | Motivering |
|-------|--------|------------|
| 2026-01-05 | Använd Strangler Fig Pattern | Säker inkrementell migration |
| 2026-01-05 | Använd CoreRedirects (alias) | Noll breaking changes |
| 2026-01-05 | Feature flags för toggle | Kan rollback vid problem |
| 2026-01-05 | Inga git commits utan instruktion | Användarens säkerhet |

---

## ⚠️ RISK LOG

| Risk | Status | Mitigation |
|------|--------|------------|
| Breaking Changes | Låg | Feature flags + Git tags |
| Halvfärdig Migration | Medel | Deadline + Tracking |
| Prestanda-försämring | Låg | Benchmark varje sprint |
| Data-förlust | Låg | Backup + Adapters |

---

## 🔄 ROLLBACK PLAN

### Om Sprint 1 Misslyckas

1. Ta bort nya filer (CoreRedirects.h, ActorComponent.h, etc.)
2. Återställ CMakeLists.txt
3. Projektet är tillbaka till original state

### Om Sprint 2-6 Misslyckas

1. Sätt MigrationConfig::enableMigration(false)
2. Gamla systemet används automatiskt
3. Kan fortsätta arbeta medan problem fixas

---

## 📚 REFERENSER

- [Safe-Migration-Strategy.md](Safe-Migration-Strategy.md) - Detaljerad strategi
- [Final-Hierarchy.md](Final-Hierarchy.md) - Målbild för hierarki
- [Migration-Strategy.md](Migration-Strategy.md) - Original migrations-plan

---

**Senast uppdaterad:** 2026-01-05
**Uppdaterad av:** AI Assistant
**Nästa review:** Efter Sprint 1
