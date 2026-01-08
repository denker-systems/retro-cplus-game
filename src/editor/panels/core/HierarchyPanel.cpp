/**
 * @file HierarchyPanel.cpp
 * @brief Hierarki-panel implementation
 */
#include "HierarchyPanel.h"
#include "editor/core/EditorContext.h"
#include "editor/core/SelectionManager.h"
#include "engine/data/DataLoader.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"
#include "engine/core/ActorObjectExtended.h"
#include <iostream>

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
        // Show hierarchy based on current navigation level
        if (m_selectionManager) {
            auto* world = m_selectionManager->getWorld();
            auto* level = m_selectionManager->getActiveLevel();
            auto* scene = m_selectionManager->getActiveScene();
            
            // Breadcrumb navigation
            if (world) {
                if (ImGui::SmallButton(world->getName().c_str())) {
                    std::cout << "[HierarchyPanel] Breadcrumb click: World" << std::endl;
                    m_selectionManager->setActiveLevel(nullptr);
                    m_selectionManager->setActiveScene(nullptr);
                }
                
                if (level) {
                    ImGui::SameLine();
                    ImGui::Text(">");
                    ImGui::SameLine();
                    if (ImGui::SmallButton(level->getName().c_str())) {
                        std::cout << "[HierarchyPanel] Breadcrumb click: Level " << level->getName() << std::endl;
                        m_selectionManager->setActiveScene(nullptr);
                    }
                    
                    if (scene) {
                        ImGui::SameLine();
                        ImGui::Text(">");
                        ImGui::SameLine();
                        ImGui::Text("%s", scene->getName().c_str());
                    }
                }
            }
            ImGui::Separator();
            
            // Render appropriate hierarchy view
            if (scene) {
                renderSceneHierarchy();
            } else if (level) {
                renderLevelHierarchy();
            } else if (world) {
                renderWorldHierarchy();
            }
        }
        
        ImGui::Separator();
        
        // Always show data categories below
        const auto& dialogs = DataLoader::instance().getDialogs();
        const auto& quests = DataLoader::instance().getQuests();
        const auto& items = DataLoader::instance().getItems();
        const auto& npcs = DataLoader::instance().getNPCs();
        
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

void HierarchyPanel::setSelectionManager(SelectionManager* selectionManager) {
    m_selectionManager = selectionManager;
    
    // Register callback to refresh when selection changes from viewport
    if (m_selectionManager) {
        m_selectionManager->registerSelectionChangedCallback([this]() {
            // HierarchyPanel will automatically show correct selection 
            // since it reads from SelectionManager each frame
            std::cout << "[HierarchyPanel] Selection changed notification received" << std::endl;
        });
    }
}

void HierarchyPanel::renderWorldHierarchy() {
#ifdef HAS_IMGUI
    if (!m_selectionManager) return;
    auto* world = m_selectionManager->getWorld();
    if (!world) return;
    
    ImGui::Text("Levels:");
    ImGui::Spacing();
    
    const auto& levels = world->getLevels();
    for (const auto& level : levels) {
        if (!level) continue;
        
        std::string label = "[L] " + level->getName();
        if (ImGui::Selectable(label.c_str())) {
            m_selectionManager->setActiveLevel(level.get());
        }
    }
#endif
}

void HierarchyPanel::renderLevelHierarchy() {
#ifdef HAS_IMGUI
    if (!m_selectionManager) return;
    auto* level = m_selectionManager->getActiveLevel();
    if (!level) return;
    
    ImGui::Text("Scenes:");
    ImGui::Spacing();
    
    const auto& scenes = level->getScenes();
    for (const auto& scene : scenes) {
        if (!scene) continue;
        
        const char* icon = getSceneIcon(scene->getName());
        std::string label = std::string(icon) + " " + scene->getName();
        if (ImGui::Selectable(label.c_str())) {
            m_selectionManager->setActiveScene(scene.get());
        }
    }
#endif
}

void HierarchyPanel::renderSceneHierarchy() {
#ifdef HAS_IMGUI
    if (!m_selectionManager) return;
    auto* scene = m_selectionManager->getActiveScene();
    if (!scene) return;
    
    ImGui::Text("Actors:");
    ImGui::Spacing();
    
    renderActorsForScene();
#endif
}

void HierarchyPanel::renderActorsForScene() {
#ifdef HAS_IMGUI
    if (!m_selectionManager) return;
    auto* scene = m_selectionManager->getActiveScene();
    if (!scene) {
        ImGui::TextDisabled("No scene selected");
        return;
    }
    
    const auto& actors = scene->getActors();
    auto* selectedActor = m_selectionManager->getSelectedActor();
    
    for (const auto& actorPtr : actors) {
        if (!actorPtr) continue;
        
        bool selected = (selectedActor == actorPtr.get());
        const char* icon = getActorIcon(actorPtr->getName());
        std::string label = std::string(icon) + " " + actorPtr->getName();
        
        if (ImGui::Selectable(label.c_str(), selected)) {
            std::cout << "[HierarchyPanel] Actor click: " << actorPtr->getName() << std::endl;
            m_selectionManager->selectActor(actorPtr.get());
        }
    }
    
    if (actors.empty()) {
        ImGui::TextDisabled("No actors in scene");
    }
#endif
}

const char* HierarchyPanel::getActorIcon(const std::string& actorName) const {
    if (actorName.find("Background") != std::string::npos) {
        return "[BG]";
    } else if (actorName.find("Player") != std::string::npos) {
        return "[P]";
    } else if (actorName.find("NPC") != std::string::npos) {
        return "[NPC]";
    } else if (actorName.find("Item") != std::string::npos) {
        return "[I]";
    } else if (actorName.find("Hotspot") != std::string::npos) {
        return "[H]";
    }
    return "[A]";
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
