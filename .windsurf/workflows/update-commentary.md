---
description: Corporate-grade code commentary workflow for large development teams
auto_execution_mode: 1
---

# Update Commentary Workflow (Corporate Edition)

> För 50+ utvecklarteam - Baserat på Unreal Engine och Unity standards

## PRINCIPER

### Dokumentationsnivåer
1. **Fil-nivå** - Varje fil har komplett header
2. **Klass-nivå** - Varje klass har Doxygen-dokumentation
3. **Metod-nivå** - Alla publika metoder dokumenterade
4. **Inline-nivå** - Komplex logik förklaras

### Dokumentera VARFÖR, inte VAD
```cpp
// DÅLIGT - Beskriver VAD (självklart)
i++;  // Öka i med 1

// BRA - Beskriver VARFÖR
i++;  // Gå till nästa frame i animationen
```

---

## STEG 1: FIL-HEADER (OBLIGATORISK)

```cpp
/**
 * @file FileName.cpp
 * @brief Kort beskrivning (en mening)
 * 
 * @details
 * Längre beskrivning som förklarar:
 * - Filens huvudsakliga syfte
 * - Vilka klasser/system som implementeras
 * - Viktiga beroenden
 * 
 * @author TeamName / DeveloperName
 * @date YYYY-MM-DD
 * 
 * @see RelatedFile.h
 * @see docs/architecture/module.md
 */
```

---

## STEG 2: KLASS-DOKUMENTATION

```cpp
/**
 * @class ClassName
 * @brief Kort beskrivning av klassens syfte
 * 
 * @details
 * Detaljerad beskrivning:
 * - Vad klassen ansvarar för
 * - Hur den används
 * - Viktiga design-beslut
 * 
 * @par Exempel:
 * @code
 * ClassName obj;
 * obj.doSomething();
 * @endcode
 * 
 * @note Viktiga begränsningar eller anmärkningar
 * 
 * @see BaseClass
 * @see RelatedClass
 */
class ClassName : public BaseClass {
```

---

## STEG 3: METOD-DOKUMENTATION

### Publika metoder (ALLTID dokumentera)
```cpp
/**
 * @brief Kort beskrivning av vad metoden gör
 * 
 * @details
 * Längre beskrivning om det behövs.
 * Förklara algoritm, komplexitet, bieffekter.
 * 
 * @param[in] inputParam Beskrivning av input-parameter
 * @param[out] outputParam Beskrivning av output-parameter
 * @param[in,out] inoutParam Parameter som läses och skrivs
 * 
 * @return Vad som returneras
 * @retval true Om operation lyckades
 * @retval false Om operation misslyckades
 * 
 * @throws std::invalid_argument Om parametrar är ogiltiga
 * @throws std::runtime_error Om operation misslyckas
 * 
 * @pre Förutsättningar som måste vara uppfyllda
 * @post Tillstånd efter anropet
 * 
 * @note Viktiga anmärkningar
 * @warning Varningar om potentiella problem
 * @deprecated Använd newMethod() istället
 * 
 * @see relatedMethod()
 */
ReturnType methodName(ParamType inputParam);
```

### Privata metoder (dokumentera om komplex)
```cpp
/**
 * @brief Intern hjälpmetod för X
 */
void helperMethod();
```

---

## STEG 4: INLINE-KOMMENTARER

### När ska man kommentera?
- Komplex logik
- Icke-uppenbara optimeringar
- Workarounds för buggar
- Magiska nummer
- Viktiga antaganden

### Format
```cpp
// ============================================================
// SECTION NAME - Använd för att dela upp stora filer
// ============================================================

// Förklara komplex beräkning
// Använder Taylor-expansion för snabb approximation av sin()
float fastSin = x - (x*x*x)/6.0f + (x*x*x*x*x)/120.0f;

// TODO(developer): Beskrivning - Issue #123
// FIXME(developer): Beskrivning - Bug #456
// HACK(developer): Varför detta hack behövs
// NOTE: Viktig information för framtida utvecklare

// [PERF] Hot path - undvik allokering
// [THREAD] Inte thread-safe
// [DEPRECATED] Kommer tas bort i v2.0
```

---

## STEG 5: MEMBER VARIABLES

```cpp
private:
    // === Transform ===
    Vec2 m_position{0.0f, 0.0f};    ///< World position in pixels
    float m_rotation = 0.0f;         ///< Rotation in degrees
    Vec2 m_scale{1.0f, 1.0f};        ///< Scale factor
    
    // === Physics ===
    Vec2 m_velocity{0.0f, 0.0f};     ///< Current velocity (pixels/sec)
    float m_mass = 1.0f;             ///< Mass in kg for physics
    
    // === State ===
    bool m_isActive = true;          ///< Whether actor is updated
    bool m_isVisible = true;         ///< Whether actor is rendered
```

---

## STEG 6: ENUM DOKUMENTATION

```cpp
/**
 * @enum ActorState
 * @brief Möjliga tillstånd för en Actor
 */
enum class ActorState {
    Inactive,   ///< Actor is disabled, not updated
    Active,     ///< Actor is running normally
    Paused,     ///< Actor is paused, not updated but rendered
    Destroyed   ///< Actor is marked for deletion
};
```

---

## STEG 7: NAMESPACE DOKUMENTATION

```cpp
/**
 * @namespace engine
 * @brief Core engine functionality
 * 
 * Contains all core engine systems:
 * - Rendering
 * - Physics
 * - Audio
 * - Input
 */
namespace engine {

/**
 * @namespace engine::physics
 * @brief Physics simulation systems
 */
namespace physics {
```

---

## VERIFIERING

### 1. Bygg projektet
// turbo
```powershell
cd build; cmake --build . --config Release
```

### 2. Generera Doxygen (om konfigurerat)
```powershell
doxygen Doxyfile
```

### 3. Review checklista
- [ ] Alla filer har fil-header?
- [ ] Alla klasser har @class dokumentation?
- [ ] Alla publika metoder har @brief?
- [ ] Komplexa algoritmer förklarade?
- [ ] Inga självklara kommentarer?
- [ ] Bygger utan fel?

---

## ANTI-PATTERNS

### Undvik dessa:
```cpp
// DÅLIGT: Självklart
x = 5;  // Sätt x till 5

// DÅLIGT: Utdaterad kommentar
// Returnerar användarens ålder
int getHealth() { return m_health; }

// DÅLIGT: Kommentar istället för bra namn
int x;  // Spelarens hälsa
// BRA:
int m_playerHealth;

// DÅLIGT: För lång kommentar på en rad
// Detta är en väldigt lång kommentar som förklarar exakt vad koden gör men som är svår att läsa eftersom den är på en enda rad utan radbrytningar

// BRA:
// Detta är en kommentar som förklarar vad koden gör.
// Den är uppdelad på flera rader för läsbarhet.
```
