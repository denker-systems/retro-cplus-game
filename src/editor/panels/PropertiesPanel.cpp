/**
 * @file PropertiesPanel.cpp
 * @brief Properties panel implementation
 */
#include "PropertiesPanel.h"
#include "editor/EditorContext.h"
#include "engine/data/DataLoader.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

PropertiesPanel::PropertiesPanel(EditorContext& context)
    : m_context(context) {
}

void PropertiesPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        switch (m_context.selectedType) {
            case EditorContext::SelectionType::Room:
                renderRoomProperties();
                break;
            case EditorContext::SelectionType::Dialog:
                renderDialogProperties();
                break;
            case EditorContext::SelectionType::Quest:
                renderQuestProperties();
                break;
            case EditorContext::SelectionType::Item:
                renderItemProperties();
                break;
            case EditorContext::SelectionType::Hotspot:
                renderHotspotProperties();
                break;
            default:
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                    "Select an item to view properties");
                break;
        }
    }
    ImGui::End();
#endif
}

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
