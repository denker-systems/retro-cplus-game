---
trigger: glob
globs: ["*.cpp", "*.h"]
---

# Code Comments Guidelines

<file_header>
Varje fil ska ha en kort header:
`cpp
/**
 * @file Game.cpp
 * @brief Huvudspelloop och SDL-initiering
 */
`
</file_header>

<class_comments>
Kort beskrivning av klassens syfte:
`cpp
/**
 * @brief Hanterar all texturladdning och caching
 */
class TextureManager {
`
</class_comments>

<method_comments>
Dokumentera publika metoder:
`cpp
/**
 * @brief Laddar textur från fil, cachar om redan laddad
 * @param path Sökväg till bildfil
 * @return Pekare till textur, nullptr vid fel
 */
SDL_Texture* load(const std::string& path);
`
</method_comments>

<inline_comments>
Förklara icke-uppenbara saker:
`cpp
// Beräkna frame-oberoende rörelse
float deltaTime = (current - last) / 1000.0f;

// Flippa sprite horisontellt om vi går vänster
SDL_RendererFlip flip = flipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
`
</inline_comments>

<avoid>
Undvik självklarheter:
`cpp
// DÅLIGT:
i++;  // Öka i med 1
return true;  // Returnera true

// BRA:
i++;  // Gå till nästa frame
return true;  // Laddning lyckades
`
</avoid>

<member_variables>
Kort förklaring för icke-uppenbara:
`cpp
float m_frameTime = 0.1f;  // Sekunder per frame
bool m_loop = true;        // Upprepa animation
`
</member_variables>
