---
description: Research och kunskapsinsamling innan implementation
auto_execution_mode: 1
---

# Research Workflow

> Systematisk kunskapsinsamling för informerade beslut

## 1. Definiera Research-frågan

### Vad ska undersökas?

| Fråga | Svar |
|-------|------|
| **Ämne** | [Vad ska implementeras/uppdateras?] |
| **Syfte** | [Varför behövs research?] |
| **Scope** | [Hur djupt ska vi gå?] |
| **Tidsgräns** | [Max tid för research] |

### Research-typer

| Typ | När | Exempel |
|-----|-----|---------|
| **Feature Research** | Innan ny feature | "Hur implementerar andra engines X?" |
| **Best Practices** | Koduppdatering | "Vad är best practice för Y?" |
| **Bug Investigation** | Svårlöst bugg | "Varför uppstår Z?" |
| **Performance** | Optimering | "Hur optimerar man W?" |
| **Architecture** | Designbeslut | "Vilken pattern passar bäst?" |
| **Library Evaluation** | Nytt beroende | "Vilket library ska vi använda?" |

---

## 2. Primära Källor

### 2.1 Context7 - Library Documentation

Använd för aktuell dokumentation av libraries:

```
Resolve library ID först, sedan query docs
```

**Relevanta libraries för Retro Engine:**
- `/libsdl-org/SDL` - SDL2 dokumentation
- `/erincatto/box2d` - Box2D physics
- `/ocornut/imgui` - Dear ImGui
- `/nlohmann/json` - JSON library
- `/g-truc/glm` - OpenGL Mathematics

### 2.2 Web Search

Sök på webben för bredare information:

**Söktermer att använda:**
```
[ämne] C++ best practices
[ämne] game engine implementation
[ämne] SDL2 tutorial
[ämne] Box2D example
[ämne] Unreal Engine approach
[ämne] Unity implementation
```

---

## 3. Game Engine Källor

### 3.1 Unreal Engine (Epic Games)

**Dokumentation:** https://docs.unrealengine.com

**Relevanta områden:**
- Actor/Component system
- Blueprint architecture
- Physics integration
- Editor tools
- Gameplay framework

**Söktermer:**
```
site:docs.unrealengine.com [ämne]
site:forums.unrealengine.com [ämne]
```

### 3.2 Unity

**Dokumentation:** https://docs.unity3d.com

**Relevanta områden:**
- GameObject/Component pattern
- ScriptableObjects
- Physics system
- Editor extensions
- Asset pipeline

**Söktermer:**
```
site:docs.unity3d.com [ämne]
site:forum.unity.com [ämne]
```

### 3.3 Godot

**Dokumentation:** https://docs.godotengine.org

**Relevanta områden:**
- Node/Scene system
- Signal pattern
- 2D physics
- GDScript patterns

**Söktermer:**
```
site:docs.godotengine.org [ämne]
```

---

## 4. Tekniska Källor

### 4.1 Microsoft / C++

**MSDN:** https://learn.microsoft.com/cpp

**Relevanta områden:**
- Modern C++ (17/20/23)
- STL containers och algorithms
- Memory management
- Concurrency

**Söktermer:**
```
site:learn.microsoft.com/cpp [ämne]
site:devblogs.microsoft.com [ämne] C++
```

### 4.2 NVIDIA

**Developer:** https://developer.nvidia.com

**Relevanta områden:**
- GPU optimization
- Rendering techniques
- Physics simulation
- Performance profiling

**Söktermer:**
```
site:developer.nvidia.com [ämne]
```

### 4.3 Intel

**Developer:** https://www.intel.com/developer

**Relevanta områden:**
- CPU optimization
- SIMD/vectorization
- Threading
- Cache optimization

---

## 5. Community Källor

### 5.1 GitHub

**Sök repositories och kod:**

```
[ämne] language:cpp stars:>100
[ämne] game engine language:cpp
SDL2 [ämne] language:cpp
Box2D [ämne] language:cpp
```

**Relevanta repositories:**
- `libsdl-org/SDL` - SDL2 source
- `erincatto/box2d` - Box2D source
- `ocornut/imgui` - ImGui source
- `TheCherno/Hazel` - Game engine example
- `godotengine/godot` - Godot source

### 5.2 Stack Overflow

**Söktermer:**
```
site:stackoverflow.com [ämne] C++
site:stackoverflow.com [ämne] SDL2
site:stackoverflow.com [ämne] game development
```

**Tags att använda:**
- `[c++]`
- `[sdl-2]`
- `[box2d]`
- `[imgui]`
- `[game-engine]`
- `[game-physics]`

### 5.3 Reddit

**Relevanta subreddits:**
- r/gamedev
- r/cpp
- r/cpp_questions
- r/graphicsprogramming

**Söktermer:**
```
site:reddit.com/r/gamedev [ämne]
site:reddit.com/r/cpp [ämne]
```

---

## 6. Akademiska Källor

### 6.1 Game Programming Patterns

**Bok:** https://gameprogrammingpatterns.com

**Patterns:**
- Component pattern
- State pattern
- Observer pattern
- Command pattern
- Object pool
- Spatial partition

### 6.2 Real-Time Rendering

**Bok:** https://www.realtimerendering.com

### 6.3 Game Engine Architecture

**Bok av Jason Gregory**

---

## 7. Research Process

### Steg 1: Snabb Översikt (5-10 min)

1. **Web Search** - Bred sökning för att förstå landskapet
2. **Wikipedia/Tutorials** - Grundläggande förståelse
3. **Identifiera nyckeltermer** - Tekniska termer att söka vidare på

### Steg 2: Djupdykning (15-30 min)

1. **Context7** - Aktuell library-dokumentation
2. **GitHub Examples** - Konkreta implementationer
3. **Stack Overflow** - Vanliga problem och lösningar

### Steg 3: Best Practices (10-20 min)

1. **Unreal/Unity Docs** - Hur gör professionella engines?
2. **Game Programming Patterns** - Etablerade patterns
3. **Microsoft C++ Docs** - Språk-specifika best practices

### Steg 4: Sammanställning (5-10 min)

1. **Dokumentera findings** - Samla i research-rapport
2. **Identifiera approach** - Välj bästa lösningen
3. **Lista trade-offs** - Dokumentera för/nackdelar

---

## 8. Research Rapport Template

```markdown
# Research: [Ämne]

## Datum
YYYY-MM-DD

## Frågeställning
[Vad ska undersökas?]

## Källor

### Dokumentation
- [Källa 1](url) - [Sammanfattning]
- [Källa 2](url) - [Sammanfattning]

### Kod-exempel
- [Repository](url) - [Relevant för]
- [Gist/Snippet](url) - [Vad den visar]

### Stack Overflow
- [Fråga](url) - [Key insight]

## Sammanfattning

### Alternativ 1: [Namn]
**Beskrivning:** [Hur fungerar det]
**Fördelar:** 
- [Fördel 1]
- [Fördel 2]
**Nackdelar:**
- [Nackdel 1]
**Används av:** [Unreal/Unity/etc]

### Alternativ 2: [Namn]
**Beskrivning:** [Hur fungerar det]
**Fördelar:** 
- [Fördel 1]
**Nackdelar:**
- [Nackdel 1]

## Rekommendation

**Valt alternativ:** [Alternativ X]

**Motivering:**
[Varför detta alternativ passar bäst för Retro Engine]

## Implementation Notes

- [Steg 1]
- [Steg 2]
- [Potentiella fallgropar]

## Relaterade ADRs
- ADR-XXX: [Relaterat beslut]
```

---

## 9. Verktyg att Använda

### MCP Tools

| Tool | Användning |
|------|------------|
| **mcp0_resolve-library-id** | Hitta Context7 library ID |
| **mcp0_query-docs** | Sök i library-dokumentation |
| **mcp1_ask_question** | Fråga om GitHub repo |
| **mcp1_read_wiki_structure** | Se repo-dokumentation |
| **search_web** | Bred web-sökning |
| **read_url_content** | Läs specifik URL |

### Sökstrategi

```
1. Bred sökning → Identifiera relevanta termer
2. Context7 → Aktuell library-doc
3. GitHub → Kod-exempel
4. Stack Overflow → Problem/lösningar
5. Engine docs → Best practices
```

---

## 10. Exempel: Research Session

### Fråga: "Hur implementerar vi ett animation state machine?"

**Steg 1: Web Search**
```
animation state machine C++ game engine
animation blend tree implementation
```

**Steg 2: Context7**
```
Library: SDL2, Box2D (för physics-driven animation)
Query: "animation state machine"
```

**Steg 3: GitHub**
```
animation state machine language:cpp stars:>50
game engine animation language:cpp
```

**Steg 4: Engine Docs**
- Unreal: Animation Blueprints
- Unity: Animator Controller
- Godot: AnimationTree

**Steg 5: Stack Overflow**
```
site:stackoverflow.com animation state machine C++
```

**Steg 6: Sammanställning**
- Dokumentera 2-3 bästa approaches
- Välj en som passar Retro Engine
- Skapa implementation plan

---

## Research Checklista

### Innan du börjar
- [ ] Frågeställning definierad
- [ ] Scope avgränsad
- [ ] Tidsgräns satt

### Under research
- [ ] Web search genomförd
- [ ] Context7 konsulterad
- [ ] GitHub-exempel hittade
- [ ] Stack Overflow kollad
- [ ] Engine docs lästa

### Efter research
- [ ] Rapport skapad
- [ ] Alternativ dokumenterade
- [ ] Rekommendation given
- [ ] Implementation notes klara

---

## ⚠️ Viktiga Principer

1. **Verifiera källor** - Kontrollera att information är aktuell
2. **Jämför flera källor** - Lita inte på en enda källa
3. **Prioritera officiell dokumentation** - Före tutorials/bloggar
4. **Dokumentera allt** - Spara länkar och sammanfattningar
5. **Tidsbegränsa** - Sätt max-tid för research
6. **Fokus på applicerbarhet** - Hur passar det Retro Engine?
