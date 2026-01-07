# Research: AI Chatt-Agent för Retro Editor

## Datum
2026-01-07

## Frågeställning
Hur implementerar vi en AI chatt-agent i Retro Editor som kan:
1. Ta emot användarinstruktioner i naturligt språk
2. Utföra actions i editorn (skapa rum, placera actors, redigera hotspots)
3. Generera innehåll (dialog-träd, quest-strukturer, level layouts)

Inspiration: Windsurf Cascade, Cursor, GitHub Copilot

---

## Källor

### Dokumentation
- [How Cursor Works](https://blog.sshh.io/p/how-cursor-ai-ide-works) - Djupgående analys av AI IDE-arkitektur
- [ai-sdk-cpp](https://github.com/ClickHouse/ai-sdk-cpp) - C++ SDK för OpenAI/Anthropic
- [MCP Specification](https://modelcontextprotocol.io) - Model Context Protocol standard
- [llama.cpp](https://github.com/ggml-org/llama.cpp) - Lokal LLM-inferens i C++
- [Game Programming Patterns: Command](https://gameprogrammingpatterns.com/command.html) - Command pattern för AI

### Kod-exempel
- `src/editor/core/IEditorCommand.h` - Vårt befintliga command-system
- `src/editor/core/CommandManager.h` - Undo/redo-hantering

---

## Sammanfattning av Findings

### 1. Hur Cursor/Cascade Fungerar

**Arkitektur (från blog.sshh.io):**
```
User Input → LLM med System Prompt → Tool Calls → Execute → Result → LLM → Response
```

**Nyckelkomponenter:**
1. **Chat UI** - Input/output för användaren
2. **System Prompt** - Instruktioner till LLM:en
3. **Tool Definitions** - JSON Schema för varje action
4. **Tool Execution** - Kör actions och returnerar resultat
5. **Context Management** - Filer, state, historia

**Core Tools i IDE:**
- `read_file(path)` - Läs fil
- `write_file(path, content)` - Skriv fil
- `run_command(cmd)` - Kör kommando
- `search(query)` - Sök i kod

### 2. LLM API-alternativ

| Alternativ | Fördelar | Nackdelar | Kostnad |
|------------|----------|-----------|---------|
| **OpenAI GPT-4** | Bäst tool calling, stabil | Kräver internet, dyr | ~$0.03/1K tokens |
| **Anthropic Claude** | Utmärkt för kod, säker | Kräver internet | ~$0.015/1K tokens |
| **Ollama (lokal)** | Gratis, offline, privat | Kräver GPU, sämre tool calling | Gratis |
| **llama.cpp (lokal)** | Pure C++, snabb | Komplex setup, variabel kvalitet | Gratis |

### 3. C++ Integration

**ai-sdk-cpp (ClickHouse):**
```cpp
#include <ai/openai.h>
#include <ai/tools.h>

ai::ToolSet tools = {
    {"create_room", ai::create_simple_tool(
        "create_room",
        "Create a new room in the game",
        {{"name", "string"}, {"background", "string"}},
        create_room_handler
    )}
};

auto result = client.generate_text({
    .model = "gpt-4o",
    .prompt = user_message,
    .tools = tools,
    .max_steps = 5
});
```

**Fördelar:**
- Modern C++20
- Stöd för OpenAI + Anthropic
- Inbyggd tool calling
- Streaming support
- Minimal dependencies (bara nlohmann/json + httplib)

### 4. Tool-System Design (MCP-inspirerat)

**Tool Definition:**
```cpp
struct EditorTool {
    std::string name;
    std::string description;
    nlohmann::json inputSchema;  // JSON Schema
    std::function<nlohmann::json(const nlohmann::json&)> handler;
};
```

**Exempel-tools för Retro Editor:**

| Tool | Beskrivning | Parametrar |
|------|-------------|------------|
| `create_room` | Skapa nytt rum | name, background, walkArea |
| `create_hotspot` | Skapa hotspot | roomId, name, type, rect |
| `create_npc` | Skapa NPC | name, dialogId, position |
| `create_dialog` | Skapa dialog-träd | id, nodes[] |
| `create_quest` | Skapa quest | id, title, objectives[] |
| `modify_room` | Ändra rum | roomId, changes{} |
| `list_rooms` | Lista alla rum | - |
| `get_room` | Hämta rum-data | roomId |
| `search_content` | Sök i content | query, type |

### 5. Befintligt Command-System

Vi har redan ett robust command-system:

```cpp
class IEditorCommand {
    virtual bool execute() = 0;
    virtual bool undo() = 0;
    virtual std::string getDescription() const = 0;
};
```

**Befintliga commands:**
- `HotspotCommands.h` - Hotspot manipulation
- `ActorCommands.h` - Actor manipulation

**Integration:** AI-tools kan generera `IEditorCommand`-objekt för undo/redo!

---

## Alternativ

### Alternativ 1: Cloud API (OpenAI/Anthropic)
**Beskrivning:** Använd ai-sdk-cpp för att anropa cloud API:er

**Fördelar:**
- Bästa kvalitet på tool calling
- Minimal lokal setup
- Alltid uppdaterade modeller
- Streaming för snabb respons

**Nackdelar:**
- Kräver internet
- Kostnad per token
- API-nyckel behövs
- Latency ~1-3 sekunder

**Används av:** Cursor, Copilot, Windsurf

### Alternativ 2: Lokal LLM (Ollama/llama.cpp)
**Beskrivning:** Kör LLM lokalt på användarens dator

**Fördelar:**
- Gratis
- Offline-kapabel
- Full privacy
- Ingen API-nyckel

**Nackdelar:**
- Kräver GPU (8GB+ VRAM)
- Sämre tool calling
- Större binär
- Komplex distribution

**Används av:** Continue.dev, LM Studio

### Alternativ 3: Hybrid (Cloud + Lokal fallback)
**Beskrivning:** Primärt cloud, fallback till lokal

**Fördelar:**
- Bäst av båda världar
- Fungerar offline
- Användaren väljer

**Nackdelar:**
- Mer komplex implementation
- Två kodbaser att underhålla

---

## Rekommendation

**Valt alternativ:** Alternativ 1 (Cloud API) med framtida lokal expansion

**Motivering:**
1. **Tool calling kvalitet** - GPT-4 och Claude är överlägsna på structured output
2. **Snabb implementation** - ai-sdk-cpp ger oss 90% av funktionaliteten
3. **Retro Editor-kontext** - Vi behöver bra förståelse av JSON-strukturer
4. **Vårt command-system** - Perfekt integration med IEditorCommand

**Fas 1:** Implementera med OpenAI/Anthropic
**Fas 2 (framtida):** Lägg till Ollama-stöd för offline-användning

---

## Implementation Notes

### Arkitektur

```
┌─────────────────────────────────────────────────────────────┐
│                      RetroEditor                            │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌──────────────┐  ┌───────────────────┐   │
│  │ AIChatPanel │──│ AIAgentSystem │──│ EditorToolRegistry│   │
│  │  (ImGui UI) │  │  (Orchestrator)│  │  (Tool definitions)│  │
│  └─────────────┘  └──────────────┘  └───────────────────┘   │
│                          │                    │             │
│                          ▼                    ▼             │
│                   ┌──────────────┐  ┌───────────────────┐   │
│                   │ LLMProvider  │  │ CommandManager    │   │
│                   │ (ai-sdk-cpp) │  │ (Undo/Redo)       │   │
│                   └──────────────┘  └───────────────────┘   │
│                          │                    │             │
│                          ▼                    ▼             │
│                   ┌──────────────┐  ┌───────────────────┐   │
│                   │ OpenAI/      │  │ IEditorCommand    │   │
│                   │ Anthropic API│  │ implementations   │   │
│                   └──────────────┘  └───────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

### Komponenter

1. **AIChatPanel** - ImGui-panel med chat-interface
2. **AIAgentSystem** - Orchestrator som hanterar konversation
3. **EditorToolRegistry** - Register av tillgängliga tools
4. **LLMProvider** - Abstraction för LLM API
5. **EditorTool** - Interface för AI-anropbara funktioner

### System Prompt (exempel)

```
Du är en AI-assistent för Retro Engine Editor.

Du kan utföra följande actions:
- Skapa och modifiera rum (rooms)
- Placera och konfigurera hotspots
- Skapa NPCs och dialog-träd
- Designa quests och objectives
- Generera level layouts

Använd tools för att utföra ändringar. Fråga användaren om du är osäker.
Alla ändringar kan ångras med Ctrl+Z.

Nuvarande rum: {current_room}
Tillgängliga assets: {asset_list}
```

### Säkerhet

- **Bekräftelse** - Destruktiva actions kräver användarbekräftelse
- **Undo** - Alla AI-actions går genom CommandManager
- **Sandbox** - AI kan inte köra godtyckliga kommandon
- **Rate limiting** - Max X requests per minut
- **API-nyckel** - Lagras säkert (environment variable eller encrypted config)

---

## Implementation Plan

### Fas 1: Foundation (2-3 sessioner)

- [ ] Integrera ai-sdk-cpp (eller httplib + nlohmann/json)
- [ ] Skapa `LLMProvider` abstraction
- [ ] Skapa `EditorTool` interface
- [ ] Skapa `EditorToolRegistry`
- [ ] Implementera 3-4 basic tools (create_room, list_rooms, etc.)

### Fas 2: UI & Integration (2 sessioner)

- [ ] Skapa `AIChatPanel` med ImGui
- [ ] Implementera `AIAgentSystem` orchestrator
- [ ] Koppla tools till CommandManager
- [ ] Lägg till context (current room, selection, etc.)

### Fas 3: Tools Expansion (2-3 sessioner)

- [ ] Hotspot tools (create, modify, delete)
- [ ] NPC/Dialog tools
- [ ] Quest tools
- [ ] Content generation tools

### Fas 4: Polish (1-2 sessioner)

- [ ] Streaming responses
- [ ] Conversation history
- [ ] Error handling
- [ ] Settings panel för API-nyckel

---

## Relaterade ADRs

- ADR-007: AI Agent Integration (att skapa)

---

## Appendix: ai-sdk-cpp Integration

**vcpkg manifest addition:**
```json
{
  "dependencies": [
    "ai-sdk-cpp"
  ]
}
```

Alternativt header-only med httplib:
```cpp
// Minimal implementation utan externt beroende
#include <httplib.h>
#include <nlohmann/json.hpp>

class OpenAIClient {
    httplib::SSLClient client{"api.openai.com"};
    std::string apiKey;
    
public:
    std::string chat(const std::string& prompt, 
                     const std::vector<EditorTool>& tools);
};
```
