/**
 * @file HierarchyPanel.cpp
 * @brief Hierarki-panel implementation
 */
#include "HierarchyPanel.h"
#include "editor/EditorContext.h"
#include "engine/data/DataLoader.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

HierarchyPanel::HierarchyPanel(EditorContext& context)
    : m_context(context) {
}

void HierarchyPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        const auto& rooms = DataLoader::instance().getRooms();
        const auto& dialogs = DataLoader::instance().getDialogs();
        const auto& quests = DataLoader::instance().getQuests();
        const auto& items = DataLoader::instance().getItems();
        const auto& npcs = DataLoader::instance().getNPCs();
        
        // Rooms
        if (ImGui::CollapsingHeader("Rooms", ImGuiTreeNodeFlags_DefaultOpen)) {
            for (const auto& room : rooms) {
                bool selected = (m_context.selectedRoomId == room.id);
                if (ImGui::Selectable(room.name.c_str(), selected)) {
                    m_context.selectedRoomId = room.id;
                    m_context.selectedType = EditorContext::SelectionType::Room;
                    if (m_onRoomSelected) m_onRoomSelected(room.id);
                }
            }
        }
        
        // Dialogs
        if (ImGui::CollapsingHeader("Dialogs")) {
            for (const auto& dialog : dialogs) {
                bool selected = (m_context.selectedDialogId == dialog.id);
                if (ImGui::Selectable(dialog.id.c_str(), selected)) {
                    m_context.selectedDialogId = dialog.id;
                    m_context.selectedType = EditorContext::SelectionType::Dialog;
                    if (m_onDialogSelected) m_onDialogSelected(dialog.id);
                }
            }
        }
        
        // Quests
        if (ImGui::CollapsingHeader("Quests")) {
            for (const auto& quest : quests) {
                bool selected = (m_context.selectedQuestId == quest.id);
                if (ImGui::Selectable(quest.title.c_str(), selected)) {
                    m_context.selectedQuestId = quest.id;
                    m_context.selectedType = EditorContext::SelectionType::Quest;
                    if (m_onQuestSelected) m_onQuestSelected(quest.id);
                }
            }
        }
        
        // Items
        if (ImGui::CollapsingHeader("Items")) {
            for (const auto& item : items) {
                bool selected = (m_context.selectedItemId == item.id);
                if (ImGui::Selectable(item.name.c_str(), selected)) {
                    m_context.selectedItemId = item.id;
                    m_context.selectedType = EditorContext::SelectionType::Item;
                    if (m_onItemSelected) m_onItemSelected(item.id);
                }
            }
        }
        
        // NPCs
        if (ImGui::CollapsingHeader("NPCs")) {
            for (const auto& npc : npcs) {
                if (ImGui::Selectable(npc.name.c_str())) {
                    // TODO: NPC selection
                }
            }
        }
    }
    ImGui::End();
#endif
}
