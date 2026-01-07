/**
 * @file HierarchyPanel.cpp
 * @brief Hierarki-panel implementation
 */
#include "HierarchyPanel.h"
#include "editor/core/EditorContext.h"
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
        
        // Scenes (formerly Rooms)
        if (ImGui::CollapsingHeader("Scenes", ImGuiTreeNodeFlags_DefaultOpen)) {
            // New Scene button
            if (ImGui::SmallButton("+ New Scene")) {
                RoomData newScene;
                newScene.id = "scene_" + std::to_string(m_context.rooms.size());
                newScene.name = "New Scene";
                newScene.background = "assets/backgrounds/placeholder.png";
                newScene.playerSpawnX = 320.0f;
                newScene.playerSpawnY = 300.0f;
                newScene.walkArea = {0, 640, 260, 400, 0.5f, 1.0f};
                m_context.rooms.push_back(newScene);
                m_context.markDirty();
                
                // Select the new scene
                m_context.selectedRoomId = newScene.id;
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
                
                // Get scene type icon based on room id/name
                const char* icon = getSceneIcon(room.id);
                std::string label = std::string(icon) + " " + room.name;
                
                if (ImGui::Selectable(label.c_str(), selected)) {
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

const char* HierarchyPanel::getSceneIcon(const std::string& sceneId) const {
    // Determine scene type based on id/name patterns
    if (sceneId.find("cave") != std::string::npos) {
        return "[C]";  // Cave
    } else if (sceneId.find("forest") != std::string::npos) {
        return "[F]";  // Forest
    } else if (sceneId.find("cellar") != std::string::npos) {
        return "[B]";  // Basement/Cellar
    } else if (sceneId.find("street") != std::string::npos || 
               sceneId.find("town") != std::string::npos ||
               sceneId.find("square") != std::string::npos) {
        return "[T]";  // Town
    } else if (sceneId.find("underwater") != std::string::npos) {
        return "[U]";  // Underwater
    } else if (sceneId.find("dream") != std::string::npos) {
        return "[D]";  // Dream
    } else {
        return "[I]";  // Interior (default)
    }
}
