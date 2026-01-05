# DialogSystem

> Singleton som hanterar dialoger och konversationer med NPCs

## Includes
```cpp
#include "systems/DialogSystem.h"
```

## Översikt

`DialogSystem` hanterar dialogträd med val, villkor och actions. Stödjer branching conversations med dynamiskt innehåll.

## Singleton Access

```cpp
DialogSystem& DialogSystem::instance();
```

## Datastrukturer

### DialogNode
```cpp
struct DialogNode {
    int id;
    std::string speaker;
    std::string text;
    std::vector<DialogChoice> choices;
};
```

### DialogChoice
```cpp
struct DialogChoice {
    std::string text;
    int nextNodeId;
    std::function<bool()> condition;  // Villkor för att visa valet
    std::function<void()> action;     // Körs när valet väljs
};
```

### DialogTree
```cpp
struct DialogTree {
    std::string id;
    std::vector<DialogNode> nodes;
    int currentNodeId = 0;
};
```

## Publika Metoder

### addDialog
```cpp
void addDialog(const DialogTree& tree);
```
**Beskrivning:** Lägger till ett dialogträd i systemet.

**Parametrar:**
- `tree` - Dialogträd att lägga till

**Exempel:**
```cpp
DialogTree bartenderDialog;
bartenderDialog.id = "bartender_intro";
// ... lägg till nodes
DialogSystem::instance().addDialog(bartenderDialog);
```

### startDialog
```cpp
void startDialog(const std::string& dialogId);
```
**Beskrivning:** Startar en dialog.

**Parametrar:**
- `dialogId` - ID på dialog att starta

**Exempel:**
```cpp
DialogSystem::instance().startDialog("bartender_intro");
```

### selectChoice
```cpp
void selectChoice(int choiceIndex);
```
**Beskrivning:** Väljer ett dialogval (0-indexerat).

**Parametrar:**
- `choiceIndex` - Index på valet att välja

**Exempel:**
```cpp
// Användaren klickade på val 1
DialogSystem::instance().selectChoice(0);
```

### endDialog
```cpp
void endDialog();
```
**Beskrivning:** Avslutar nuvarande dialog.

### isActive
```cpp
bool isActive() const;
```
**Beskrivning:** Kontrollerar om en dialog är aktiv.

**Returnerar:** `true` om dialog pågår

### getCurrentNode
```cpp
const DialogNode* getCurrentNode() const;
```
**Beskrivning:** Hämtar nuvarande dialog-nod.

**Returnerar:** Pekare till nuvarande nod, eller `nullptr` om ingen dialog är aktiv

### getAvailableChoices
```cpp
std::vector<const DialogChoice*> getAvailableChoices() const;
```
**Beskrivning:** Hämtar alla val som är tillgängliga (uppfyller sina villkor).

**Returnerar:** Vector med pekare till tillgängliga val

## JSON Format

```json
{
  "id": "bartender_intro",
  "nodes": [
    {
      "id": 0,
      "speaker": "Bartender",
      "text": "Welcome! What can I get ya?",
      "choices": [
        {
          "text": "I'm looking for information.",
          "next": 1
        },
        {
          "text": "Just passing through.",
          "next": 2
        },
        {
          "text": "Goodbye.",
          "next": -1
        }
      ]
    },
    {
      "id": 1,
      "speaker": "Bartender",
      "text": "Information costs coin around here.",
      "choices": [
        {
          "text": "Here's 10 gold.",
          "next": 3,
          "condition": "has_gold_10"
        },
        {
          "text": "Never mind.",
          "next": -1
        }
      ]
    }
  ]
}
```

## Villkor och Actions

Villkor och actions sätts programmatiskt efter laddning:

```cpp
auto* tree = DialogSystem::instance().getDialog("bartender_intro");
auto& node = tree->nodes[1];
node.choices[0].condition = []() {
    return InventorySystem::instance().getGold() >= 10;
};
node.choices[0].action = []() {
    InventorySystem::instance().removeGold(10);
    QuestSystem::instance().updateObjective("find_info", "talked_to_bartender");
};
```

## Integration med DialogState

```cpp
// I PlayState när spelare klickar på NPC
if (hotspot->type == HotspotType::NPC) {
    DialogSystem::instance().startDialog(hotspot->dialogId);
    game->pushState(std::make_unique<DialogState>());
}
```

## Exempel: Komplett Dialog

```cpp
DialogTree createBartenderDialog() {
    DialogTree tree;
    tree.id = "bartender_intro";
    
    DialogNode node0;
    node0.id = 0;
    node0.speaker = "Bartender";
    node0.text = "Welcome to The Rusty Anchor!";
    
    DialogChoice choice1;
    choice1.text = "Tell me about this place.";
    choice1.nextNodeId = 1;
    node0.choices.push_back(choice1);
    
    DialogChoice choice2;
    choice2.text = "Goodbye.";
    choice2.nextNodeId = -1; // Avslutar dialog
    node0.choices.push_back(choice2);
    
    tree.nodes.push_back(node0);
    
    DialogNode node1;
    node1.id = 1;
    node1.speaker = "Bartender";
    node1.text = "Best tavern in town! Been here 20 years.";
    node1.choices.push_back(choice2); // Återanvänd goodbye
    tree.nodes.push_back(node1);
    
    return tree;
}
```

## Se även

- [DialogState](../states/DialogState.md)
- [ConditionSystem](ConditionSystem.md)
- [QuestSystem](QuestSystem.md)
