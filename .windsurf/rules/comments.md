---
trigger: glob
globs: ["*.cpp", "*.h"]
description: Code commenting standards (Doxygen-compatible)
---

# Code Comments

> Doxygen-kompatibel dokumentation

## Fil-header (OBLIGATORISK)

```cpp
/**
 * @file FileName.cpp
 * @brief Kort beskrivning av filen
 */
```

---

## Klass-dokumentation

```cpp
/**
 * @brief Kort beskrivning av klassen
 * 
 * Längre beskrivning om relevant:
 * - Ansvar
 * - Användning
 */
class ClassName {
```

---

## Metod-dokumentation

```cpp
/**
 * @brief Vad metoden gör
 * @param name Parameterbeskrivning
 * @return Vad som returneras
 */
ReturnType methodName(ParamType name);
```

---

## Inline-kommentarer

```cpp
// Förklara VARFÖR, inte VAD
position += velocity * deltaTime;  // Frame-independent movement

// Förklara icke-uppenbara saker
float distSq = dx*dx + dy*dy;  // Squared distance - avoids sqrt()
```

---

## Member Variables

```cpp
private:
    Vec2 m_position{0, 0};     ///< World position in pixels
    float m_speed = 100.0f;    ///< Movement speed (pixels/sec)
    bool m_active = true;      ///< Whether actor is updated
```

---

## Undvik

```cpp
// DÅLIGT - självklart
i++;  // Öka i med 1

// BRA - förklarar varför
i++;  // Nästa animation frame
```
