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
            // New Room button
            if (ImGui::SmallButton("+ New Room")) {
                RoomData newRoom;
                newRoom.id = "room_" + std::to_string(m_context.rooms.size());
                newRoom.name = "New Room";
                newRoom.background = "assets/backgrounds/placeholder.png";
                newRoom.playerSpawnX = 320.0f;
                newRoom.playerSpawnY = 300.0f;
                newRoom.walkArea = {0, 640, 260, 400, 0.5f, 1.0f};
                m_context.rooms.push_back(newRoom);
                m_context.markDirty();
                
                // Select the new room
                m_context.selectedRoomId = newRoom.id;
                m_context.selectedDialogId.clear();
                m_context.selectedQuestId.clear();
                m_context.selectedItemId.clear();
                m_context.selectedHotspotIndex = -1;
                m_context.selectedType = EditorContext::SelectionType::Room;
            }
            ImGui::Spacing();
            
            for (const auto& room : rooms) {
                bool selected = (m_context.selectedType == EditorContext::SelectionType::Room && 
                                m_context.selectedRoomId == room.id);
                if (ImGui::Selectable(room.name.c_str(), selected)) {
                    // Deselect om samma item klickas igen
                    if (selected) {
                        m_context.selectedType = EditorContext::SelectionType::None;
                        m_context.selectedRoomId.clear();
                    } else {
                        // Rensa alla andra selections
                        m_context.selectedRoomId = room.id;
                        m_context.selectedDialogId.clear();
                        m_context.selectedQuestId.clear();
                        m_context.selectedItemId.clear();
                        m_context.selectedHotspotIndex = -1;
                        m_context.selectedType = EditorContext::SelectionType::Room;
                        if (m_onRoomSelected) m_onRoomSelected(room.id);
                    }
                }
            }
        }
        
        // Dialogs
        if (ImGui::CollapsingHeader("Dialogs")) {
            // New Dialog button
            if (ImGui::SmallButton("+ New Dialog")) {
                DialogData newDialog;
                newDialog.id = "dialog_" + std::to_string(m_context.dialogs.size());
                newDialog.npcName = "NPC";
                newDialog.startNodeId = 0;
                
                // Add default node
                DialogNodeData defaultNode;
                defaultNode.id = 0;
                defaultNode.speaker = "NPC";
                defaultNode.text = "Hello!";
                defaultNode.nextNodeId = -1;
                newDialog.nodes.push_back(defaultNode);
                
                m_context.dialogs.push_back(newDialog);
                m_context.markDirty();
                
                // Select the new dialog
                m_context.selectedRoomId.clear();
                m_context.selectedDialogId = newDialog.id;
                m_context.selectedQuestId.clear();
                m_context.selectedItemId.clear();
                m_context.selectedHotspotIndex = -1;
                m_context.selectedType = EditorContext::SelectionType::Dialog;
            }
            ImGui::Spacing();
            
            for (const auto& dialog : dialogs) {
                bool selected = (m_context.selectedType == EditorContext::SelectionType::Dialog && 
                                m_context.selectedDialogId == dialog.id);
                if (ImGui::Selectable(dialog.id.c_str(), selected)) {
                    // Deselect om samma item klickas igen
                    if (selected) {
                        m_context.selectedType = EditorContext::SelectionType::None;
                        m_context.selectedDialogId.clear();
                    } else {
                        // Rensa alla andra selections
                        m_context.selectedRoomId.clear();
                        m_context.selectedDialogId = dialog.id;
                        m_context.selectedQuestId.clear();
                        m_context.selectedItemId.clear();
                        m_context.selectedHotspotIndex = -1;
                        m_context.selectedType = EditorContext::SelectionType::Dialog;
                        if (m_onDialogSelected) m_onDialogSelected(dialog.id);
                    }
                }
            }
        }
        
        // Quests
        if (ImGui::CollapsingHeader("Quests")) {
            // New Quest button
            if (ImGui::SmallButton("+ New Quest")) {
                QuestData newQuest;
                newQuest.id = "quest_" + std::to_string(m_context.quests.size());
                newQuest.title = "New Quest";
                newQuest.description = "Quest description";
                newQuest.autoStart = false;
                m_context.quests.push_back(newQuest);
                m_context.markDirty();
                
                // Select the new quest
                m_context.selectedRoomId.clear();
                m_context.selectedDialogId.clear();
                m_context.selectedQuestId = newQuest.id;
                m_context.selectedItemId.clear();
                m_context.selectedHotspotIndex = -1;
                m_context.selectedType = EditorContext::SelectionType::Quest;
            }
            ImGui::Spacing();
            
            for (const auto& quest : quests) {
                bool selected = (m_context.selectedType == EditorContext::SelectionType::Quest && 
                                m_context.selectedQuestId == quest.id);
                if (ImGui::Selectable(quest.title.c_str(), selected)) {
                    // Deselect om samma item klickas igen
                    if (selected) {
                        m_context.selectedType = EditorContext::SelectionType::None;
                        m_context.selectedQuestId.clear();
                    } else {
                        // Rensa alla andra selections
                        m_context.selectedRoomId.clear();
                        m_context.selectedDialogId.clear();
                        m_context.selectedQuestId = quest.id;
                        m_context.selectedItemId.clear();
                        m_context.selectedHotspotIndex = -1;
                        m_context.selectedType = EditorContext::SelectionType::Quest;
                        if (m_onQuestSelected) m_onQuestSelected(quest.id);
                    }
                }
            }
        }
        
        // Items
        if (ImGui::CollapsingHeader("Items")) {
            // New Item button
            if (ImGui::SmallButton("+ New Item")) {
                ItemData newItem;
                newItem.id = "item_" + std::to_string(m_context.items.size());
                newItem.name = "New Item";
                newItem.description = "Item description";
                newItem.icon = "assets/sprites/items/placeholder.png";
                newItem.combinable = false;
                m_context.items.push_back(newItem);
                m_context.markDirty();
                
                // Select the new item
                m_context.selectedRoomId.clear();
                m_context.selectedDialogId.clear();
                m_context.selectedQuestId.clear();
                m_context.selectedItemId = newItem.id;
                m_context.selectedHotspotIndex = -1;
                m_context.selectedType = EditorContext::SelectionType::Item;
            }
            ImGui::Spacing();
            
            for (const auto& item : items) {
                bool selected = (m_context.selectedType == EditorContext::SelectionType::Item && 
                                m_context.selectedItemId == item.id);
                if (ImGui::Selectable(item.name.c_str(), selected)) {
                    // Deselect om samma item klickas igen
                    if (selected) {
                        m_context.selectedType = EditorContext::SelectionType::None;
                        m_context.selectedItemId.clear();
                    } else {
                        // Rensa alla andra selections
                        m_context.selectedRoomId.clear();
                        m_context.selectedSceneId.clear();
                        m_context.selectedDialogId.clear();
                        m_context.selectedQuestId.clear();
                        m_context.selectedItemId = item.id;
                        m_context.selectedHotspotIndex = -1;
                        m_context.selectedType = EditorContext::SelectionType::Item;
                        if (m_onItemSelected) m_onItemSelected(item.id);
                    }
                }
            }
        }
        
        // NPCs
        if (ImGui::CollapsingHeader("NPCs")) {
            for (const auto& npc : npcs) {
                bool isSelected = (m_context.selectedType == EditorContext::SelectionType::NPC &&
                                   m_context.selectedNPCId == npc.id);
                if (ImGui::Selectable(npc.name.c_str(), isSelected)) {
                    m_context.selectedType = EditorContext::SelectionType::NPC;
                    m_context.selectedNPCId = npc.id;
                }
            }
        }
    }
    ImGui::End();
#endif
}
