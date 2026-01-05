---
description: Uppdaterar kommentarer i källkod för att förklara vad koden gör
---

# Update Commentary Workflow

## Principer
- Korta, tydliga kommentarer
- Förklara VAD och VARFÖR, inte självklarheter
- Alla filer ska ha fil-header
- Alla klasser och metoder ska dokumenteras

## 1. Välj fil att kommentera
Gå igenom filer utan kommentarer.

## 2. Lägg till fil-header
`cpp
/**
 * @file FileName.cpp
 * @brief Kort beskrivning av filen
 */
`

## 3. Dokumentera klasser
`cpp
/**
 * @brief Kort beskrivning av klassen
 */
class ClassName {
`

## 4. Dokumentera metoder
`cpp
/**
 * @brief Vad metoden gör
 * @param param1 Beskrivning
 * @return Vad som returneras
 */
`

## 5. Inline-kommentarer
`cpp
// Beräkna delta time för frame-oberoende rörelse
float deltaTime = (current - last) / 1000.0f;

// Ladda textur och cacha för återanvändning
m_textures[path] = texture;
`

## 6. Undvik
- Självklara kommentarer: `// Öka i med 1` för `i++`
- För långa kommentarer
- Kommentarer som inte stämmer med koden

## 7. Bygg för att verifiera
// turbo
`powershell
cd build; cmake --build . --config Release
`
