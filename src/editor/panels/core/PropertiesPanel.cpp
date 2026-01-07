/**
 * @file PropertiesPanel.cpp
 * @brief Properties panel implementation
 */
#include "PropertiesPanel.h"
#include "editor/core/EditorContext.h"
#include "editor/properties/world/RoomPropertyEditor.h"
#include "editor/properties/world/HotspotPropertyEditor.h"
#include "editor/properties/gameplay/DialogPropertyEditor.h"
#include "editor/properties/gameplay/QuestPropertyEditor.h"
#include "editor/properties/gameplay/ItemPropertyEditor.h"
#include "editor/properties/characters/NPCPropertyEditor.h"
#include "engine/data/DataLoader.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

PropertiesPanel::PropertiesPanel(EditorContext& context)
    : m_context(context) {
    // Skapa alla property editors
    m_roomEditor = std::make_unique<RoomPropertyEditor>(context);
    m_hotspotEditor = std::make_unique<HotspotPropertyEditor>(context);
    m_dialogEditor = std::make_unique<DialogPropertyEditor>(context);
    m_questEditor = std::make_unique<QuestPropertyEditor>(context);
    m_itemEditor = std::make_unique<ItemPropertyEditor>(context);
    m_npcEditor = std::make_unique<NPCPropertyEditor>(context);
}

PropertiesPanel::~PropertiesPanel() = default;

void PropertiesPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        // Uppdatera vilken editor som är aktiv
        updateEditorSelection();
        
        // Rendera breadcrumb navigation
        renderBreadcrumb();
        
        ImGui::Separator();
        
        // Rendera toolbar med Save/Revert buttons
        renderToolbar();
        
        ImGui::Separator();
        ImGui::Spacing();
        
        // Rendera aktuell editor
        IPropertyEditor* editor = getCurrentEditor();
        if (editor) {
            editor->render();
        } else {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                "Select an item to view properties");
        }
    }
    ImGui::End();
#endif
}

void PropertiesPanel::renderBreadcrumb() {
#ifdef HAS_IMGUI
    // Bygg breadcrumb baserat på selection type
    std::vector<std::pair<std::string, std::function<void()>>> breadcrumbItems;
    
    switch (m_context.selectedType) {
        case EditorContext::SelectionType::Room:
            if (!m_context.selectedRoomId.empty()) {
                // Root level
                breadcrumbItems.push_back({"Rooms", [this]() {
                    m_context.selectedType = EditorContext::SelectionType::None;
                    m_context.selectedRoomId.clear();
                }});
                
                // Current room
                RoomData* room = nullptr;
                for (auto& r : m_context.rooms) {
                    if (r.id == m_context.selectedRoomId) {
                        room = &r;
                        break;
                    }
                }
                if (room) {
                    breadcrumbItems.push_back({room->name, nullptr}); // Current item (not clickable)
                }
            }
            break;
            
        case EditorContext::SelectionType::Hotspot:
            if (!m_context.selectedRoomId.empty()) {
                // Root level
                breadcrumbItems.push_back({"Rooms", [this]() {
                    m_context.selectedType = EditorContext::SelectionType::None;
                    m_context.selectedRoomId.clear();
                    m_context.selectedHotspotIndex = -1;
                }});
                
                // Parent room
                RoomData* room = nullptr;
                for (auto& r : m_context.rooms) {
                    if (r.id == m_context.selectedRoomId) {
                        room = &r;
                        break;
                    }
                }
                if (room) {
                    breadcrumbItems.push_back({room->name, [this]() {
                        m_context.selectedType = EditorContext::SelectionType::Room;
                        m_context.selectedHotspotIndex = -1;
                    }});
                    
                    // Current hotspot
                    if (m_context.selectedHotspotIndex >= 0 && 
                        m_context.selectedHotspotIndex < (int)room->hotspots.size()) {
                        breadcrumbItems.push_back({room->hotspots[m_context.selectedHotspotIndex].name, nullptr});
                    }
                }
            }
            break;
            
        case EditorContext::SelectionType::Dialog:
            if (!m_context.selectedDialogId.empty()) {
                breadcrumbItems.push_back({"Dialogs", [this]() {
                    m_context.selectedType = EditorContext::SelectionType::None;
                    m_context.selectedDialogId.clear();
                }});
                
                DialogData* dialog = nullptr;
                for (auto& d : m_context.dialogs) {
                    if (d.id == m_context.selectedDialogId) {
                        dialog = &d;
                        break;
                    }
                }
                if (dialog) {
                    breadcrumbItems.push_back({dialog->id, nullptr});
                }
            }
            break;
            
        case EditorContext::SelectionType::Quest:
            if (!m_context.selectedQuestId.empty()) {
                breadcrumbItems.push_back({"Quests", [this]() {
                    m_context.selectedType = EditorContext::SelectionType::None;
                    m_context.selectedQuestId.clear();
                }});
                
                QuestData* quest = nullptr;
                for (auto& q : m_context.quests) {
                    if (q.id == m_context.selectedQuestId) {
                        quest = &q;
                        break;
                    }
                }
                if (quest) {
                    breadcrumbItems.push_back({quest->title, nullptr});
                }
            }
            break;
            
        case EditorContext::SelectionType::Item:
            if (!m_context.selectedItemId.empty()) {
                breadcrumbItems.push_back({"Items", [this]() {
                    m_context.selectedType = EditorContext::SelectionType::None;
                    m_context.selectedItemId.clear();
                }});
                
                ItemData* item = nullptr;
                for (auto& i : m_context.items) {
                    if (i.id == m_context.selectedItemId) {
                        item = &i;
                        break;
                    }
                }
                if (item) {
                    breadcrumbItems.push_back({item->name, nullptr});
                }
            }
            break;
            
        default:
            break;
    }
    
    // Rendera breadcrumb
    if (!breadcrumbItems.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        
        for (size_t i = 0; i < breadcrumbItems.size(); i++) {
            const auto& item = breadcrumbItems[i];
            
            // Klickbar länk (om inte sista item)
            if (item.second) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.7f, 1.0f, 1.0f));
                if (ImGui::SmallButton(item.first.c_str())) {
                    item.second(); // Anropa navigation callback
                }
                ImGui::PopStyleColor();
            } else {
                // Sista item (current) - inte klickbar
                ImGui::Text("%s", item.first.c_str());
            }
            
            // Separator (om inte sista)
            if (i < breadcrumbItems.size() - 1) {
                ImGui::SameLine();
                ImGui::Text(">");
                ImGui::SameLine();
            }
        }
        
        ImGui::PopStyleColor();
    }
#endif
}

void PropertiesPanel::renderToolbar() {
#ifdef HAS_IMGUI
    IPropertyEditor* editor = getCurrentEditor();
    if (!editor) return;
    
    // Save button
    bool canSave = editor->isDirty();
    if (!canSave) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
    }
    
    if (ImGui::Button("Save", ImVec2(100, 0)) && canSave) {
        std::string error;
        if (editor->validate(error)) {
            editor->apply();
            m_context.saveToFiles();
        } else {
            m_context.statusMessage = "Validation error: " + error;
            m_context.statusTimer = 5.0f;
        }
    }
    
    if (!canSave) {
        ImGui::PopStyleVar();
    }
    
    if (ImGui::IsItemHovered()) {
        if (canSave) {
            ImGui::SetTooltip("Save changes (Ctrl+S)");
        } else {
            ImGui::SetTooltip("No changes to save");
        }
    }
    
    // Revert button
    ImGui::SameLine();
    if (!canSave) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
    }
    
    if (ImGui::Button("Revert", ImVec2(100, 0)) && canSave) {
        editor->revert();
        m_context.statusMessage = "Changes reverted";
        m_context.statusTimer = 2.0f;
    }
    
    if (!canSave) {
        ImGui::PopStyleVar();
    }
    
    if (ImGui::IsItemHovered() && canSave) {
        ImGui::SetTooltip("Discard all changes");
    }
    
    // Dirty indicator
    if (canSave) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.3f, 1.0f), "*");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Unsaved changes");
        }
    }
#endif
}

IPropertyEditor* PropertiesPanel::getCurrentEditor() {
    switch (m_context.selectedType) {
        case EditorContext::SelectionType::Room:
            return m_roomEditor.get();
        case EditorContext::SelectionType::Hotspot:
            return m_hotspotEditor.get();
        case EditorContext::SelectionType::Dialog:
            return m_dialogEditor.get();
        case EditorContext::SelectionType::Quest:
            return m_questEditor.get();
        case EditorContext::SelectionType::Item:
            return m_itemEditor.get();
        case EditorContext::SelectionType::NPC:
            return m_npcEditor.get();
        default:
            return nullptr;
    }
}

void PropertiesPanel::updateEditorSelection() {
    // Uppdatera room editor
    if (m_context.selectedType == EditorContext::SelectionType::Room && !m_context.selectedRoomId.empty()) {
        RoomData* room = nullptr;
        for (auto& r : m_context.rooms) {
            if (r.id == m_context.selectedRoomId) {
                room = &r;
                break;
            }
        }
        if (room && m_roomEditor->getRoom() != room) {
            m_roomEditor->setRoom(room);
        }
    }
    
    // Uppdatera hotspot editor
    if (m_context.selectedType == EditorContext::SelectionType::Hotspot && 
        !m_context.selectedRoomId.empty() && m_context.selectedHotspotIndex >= 0) {
        RoomData* room = nullptr;
        for (auto& r : m_context.rooms) {
            if (r.id == m_context.selectedRoomId) {
                room = &r;
                break;
            }
        }
        if (room && m_context.selectedHotspotIndex < (int)room->hotspots.size()) {
            HotspotData* hotspot = &room->hotspots[m_context.selectedHotspotIndex];
            if (m_hotspotEditor->getHotspot() != hotspot) {
                m_hotspotEditor->setHotspot(hotspot, room);
            }
        }
    }
    
    // Uppdatera dialog editor
    if (m_context.selectedType == EditorContext::SelectionType::Dialog && !m_context.selectedDialogId.empty()) {
        DialogData* dialog = nullptr;
        for (auto& d : m_context.dialogs) {
            if (d.id == m_context.selectedDialogId) {
                dialog = &d;
                break;
            }
        }
        if (dialog && m_dialogEditor->getDialog() != dialog) {
            m_dialogEditor->setDialog(dialog);
        }
    }
    
    // Uppdatera quest editor
    if (m_context.selectedType == EditorContext::SelectionType::Quest && !m_context.selectedQuestId.empty()) {
        QuestData* quest = nullptr;
        for (auto& q : m_context.quests) {
            if (q.id == m_context.selectedQuestId) {
                quest = &q;
                break;
            }
        }
        if (quest && m_questEditor->getQuest() != quest) {
            m_questEditor->setQuest(quest);
        }
    }
    
    // Uppdatera item editor
    if (m_context.selectedType == EditorContext::SelectionType::Item && !m_context.selectedItemId.empty()) {
        ItemData* item = nullptr;
        for (auto& i : m_context.items) {
            if (i.id == m_context.selectedItemId) {
                item = &i;
                break;
            }
        }
        if (item && m_itemEditor->getItem() != item) {
            m_itemEditor->setItem(item);
        }
    }
    
    // Uppdatera NPC editor
    if (m_context.selectedType == EditorContext::SelectionType::NPC) {
        NPCData* npc = nullptr;
        for (auto& n : m_context.npcs) {
            if (n.id == m_context.selectedNPCId) {
                npc = &n;
                break;
            }
        }
        if (npc && m_npcEditor->getNPC() != npc) {
            m_npcEditor->setNPC(npc);
        }
    }
}

// Gamla metoder borttagna - ersatta av PropertyEditor-systemet
/*
void PropertiesPanel::renderRoomProperties() {
#ifdef HAS_IMGUI
    if (m_context.selectedRoomId.empty()) return;
    
    const auto& rooms = DataLoader::instance().getRooms();
    const RoomData* room = nullptr;
    for (const auto& r : rooms) {
        if (r.id == m_context.selectedRoomId) {
            room = &r;
            break;
        }
    }
    
    if (!room) return;
    
    ImGui::Text("Room Properties");
    ImGui::Separator();
    
    ImGui::Text("ID: %s", room->id.c_str());
    ImGui::Text("Name: %s", room->name.c_str());
    ImGui::Text("Background: %s", room->background.c_str());
    
    ImGui::Spacing();
    ImGui::Text("Player Spawn: (%.0f, %.0f)", room->playerSpawnX, room->playerSpawnY);
    
    ImGui::Spacing();
    ImGui::Text("Walk Area:");
    ImGui::Text("  Min: (%d, %d)", room->walkArea.minX, room->walkArea.minY);
    ImGui::Text("  Max: (%d, %d)", room->walkArea.maxX, room->walkArea.maxY);
    
    ImGui::Spacing();
    ImGui::Text("Depth Scale: %.2f - %.2f", room->walkArea.scaleTop, room->walkArea.scaleBottom);
    
    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Hotspots", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Count: %d", (int)room->hotspots.size());
        for (size_t i = 0; i < room->hotspots.size(); i++) {
            const auto& hs = room->hotspots[i];
            ImGui::PushID((int)i);
            if (ImGui::Selectable(hs.name.c_str(), m_context.selectedHotspotIndex == (int)i)) {
                m_context.selectedHotspotIndex = (int)i;
                m_context.selectedType = EditorContext::SelectionType::Hotspot;
            }
            ImGui::PopID();
        }
    }
    
    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Exits")) {
        for (const auto& hs : room->hotspots) {
            if (!hs.targetRoom.empty()) {
                ImGui::BulletText("%s -> %s", hs.name.c_str(), hs.targetRoom.c_str());
            }
        }
    }
#endif
}

void PropertiesPanel::renderDialogProperties() {
#ifdef HAS_IMGUI
    if (m_context.selectedDialogId.empty()) return;
    
    const auto& dialogs = DataLoader::instance().getDialogs();
    const DialogData* dialog = nullptr;
    for (const auto& d : dialogs) {
        if (d.id == m_context.selectedDialogId) {
            dialog = &d;
            break;
        }
    }
    
    if (!dialog) return;
    
    ImGui::Text("Dialog Properties");
    ImGui::Separator();
    
    ImGui::Text("ID: %s", dialog->id.c_str());
    ImGui::Text("Start Node: %d", dialog->startNodeId);
    ImGui::Text("Nodes: %d", (int)dialog->nodes.size());
    
    if (ImGui::CollapsingHeader("Nodes")) {
        for (const auto& node : dialog->nodes) {
            std::string preview = node.text.length() > 30 ? node.text.substr(0, 30) + "..." : node.text;
            ImGui::BulletText("[%d] %s: %s", 
                node.id, 
                node.speaker.c_str(),
                preview.c_str());
        }
    }
#endif
}

void PropertiesPanel::renderQuestProperties() {
#ifdef HAS_IMGUI
    if (m_context.selectedQuestId.empty()) return;
    
    const auto& quests = DataLoader::instance().getQuests();
    const QuestData* quest = nullptr;
    for (const auto& q : quests) {
        if (q.id == m_context.selectedQuestId) {
            quest = &q;
            break;
        }
    }
    
    if (!quest) return;
    
    ImGui::Text("Quest Properties");
    ImGui::Separator();
    
    ImGui::Text("ID: %s", quest->id.c_str());
    ImGui::Text("Title: %s", quest->title.c_str());
    ImGui::TextWrapped("Description: %s", quest->description.c_str());
    ImGui::Text("Objectives: %d", (int)quest->objectives.size());
    
    if (ImGui::CollapsingHeader("Objectives")) {
        for (const auto& obj : quest->objectives) {
            ImGui::BulletText("%s", obj.description.c_str());
        }
    }
#endif
}

void PropertiesPanel::renderItemProperties() {
#ifdef HAS_IMGUI
    if (m_context.selectedItemId.empty()) return;
    
    const auto& items = DataLoader::instance().getItems();
    const ItemData* item = nullptr;
    for (const auto& i : items) {
        if (i.id == m_context.selectedItemId) {
            item = &i;
            break;
        }
    }
    
    if (!item) return;
    
    ImGui::Text("Item Properties");
    ImGui::Separator();
    
    ImGui::Text("ID: %s", item->id.c_str());
    ImGui::Text("Name: %s", item->name.c_str());
    ImGui::TextWrapped("Description: %s", item->description.c_str());
    ImGui::Text("Icon: %s", item->icon.c_str());
    ImGui::Text("Combinable: %s", item->combinable ? "Yes" : "No");
#endif
}

void PropertiesPanel::renderHotspotProperties() {
#ifdef HAS_IMGUI
    if (m_context.selectedRoomId.empty() || m_context.selectedHotspotIndex < 0) return;
    
    const auto& rooms = DataLoader::instance().getRooms();
    const RoomData* room = nullptr;
    for (const auto& r : rooms) {
        if (r.id == m_context.selectedRoomId) {
            room = &r;
            break;
        }
    }
    
    if (!room || m_context.selectedHotspotIndex >= (int)room->hotspots.size()) return;
    
    const auto& hs = room->hotspots[m_context.selectedHotspotIndex];
    
    ImGui::Text("Hotspot Properties");
    ImGui::Separator();
    
    ImGui::Text("ID: %s", hs.id.c_str());
    ImGui::Text("Name: %s", hs.name.c_str());
    ImGui::Text("Position: (%d, %d)", hs.x, hs.y);
    ImGui::Text("Size: %d x %d", hs.w, hs.h);
    
    if (!hs.targetRoom.empty()) {
        ImGui::Spacing();
        ImGui::Text("Exit to: %s", hs.targetRoom.c_str());
    }
    
    if (!hs.dialogId.empty()) {
        ImGui::Spacing();
        ImGui::Text("Dialog: %s", hs.dialogId.c_str());
    }
#endif
}
*/
