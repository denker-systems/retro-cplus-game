/**
 * @file GameDataLoader.h
 * @brief Hjälpfunktioner för att ladda JSON-data till spelsystemen
 */
#pragma once

#include "DataLoader.h"
#include "../systems/InventorySystem.h"
#include "../systems/QuestSystem.h"
#include "../systems/DialogSystem.h"
#include "../systems/RoomManager.h"
#include "../systems/AISystem.h"
#include "../Room.h"
#include "../entities/NPC.h"
#include <iostream>

/**
 * @brief Laddar all speldata från JSON och registrerar i spelsystemen
 */
class GameDataLoader {
public:
    /** @brief Ladda och registrera all data */
    static bool loadAll() {
        if (!DataLoader::instance().loadAll()) {
            return false;
        }
        
        loadItems();
        loadQuests();
        loadDialogs();
        loadRooms();
        loadNPCs();
        
        return true;
    }
    
    /** @brief Hämta NPC-data för specifikt rum */
    static std::vector<const NPCData*> getNPCsInRoom(const std::string& roomId) {
        std::vector<const NPCData*> result;
        for (const auto& npc : DataLoader::instance().getNPCs()) {
            if (npc.room == roomId) {
                result.push_back(&npc);
            }
        }
        return result;
    }

private:
    static void loadItems() {
        for (const auto& data : DataLoader::instance().getItems()) {
            Item item;
            item.id = data.id;
            item.name = data.name;
            item.description = data.description;
            item.combinable = data.combinable;
            item.combinesWith = data.combinesWith;
            item.combineResult = data.combineResult;
            InventorySystem::instance().registerItem(item);
        }
    }
    
    static void loadQuests() {
        for (const auto& data : DataLoader::instance().getQuests()) {
            Quest quest;
            quest.id = data.id;
            quest.title = data.title;
            quest.description = data.description;
            quest.rewardItem = data.rewardItem;
            quest.rewardXP = data.rewardXP;
            
            for (const auto& objData : data.objectives) {
                Objective obj;
                obj.id = objData.id;
                obj.description = objData.description;
                obj.targetId = objData.targetId;
                obj.requiredCount = objData.requiredCount;
                obj.optional = objData.optional;
                
                // Konvertera typ-sträng till enum
                if (objData.type == "talk") obj.type = ObjectiveType::Talk;
                else if (objData.type == "collect") obj.type = ObjectiveType::Collect;
                else if (objData.type == "deliver") obj.type = ObjectiveType::Deliver;
                else if (objData.type == "goto") obj.type = ObjectiveType::GoTo;
                else if (objData.type == "examine") obj.type = ObjectiveType::Examine;
                
                quest.objectives.push_back(obj);
            }
            
            QuestSystem::instance().addQuest(quest);
            
            if (data.autoStart) {
                QuestSystem::instance().startQuest(quest.id);
            }
        }
    }
    
    static void loadDialogs() {
        for (const auto& data : DataLoader::instance().getDialogs()) {
            DialogTree tree;
            tree.id = data.id;
            tree.npcName = data.npcName;
            tree.startNodeId = data.startNodeId;
            
            for (const auto& nodeData : data.nodes) {
                DialogNode node;
                node.id = nodeData.id;
                node.speaker = nodeData.speaker;
                node.text = nodeData.text;
                node.nextNodeId = nodeData.nextNodeId;
                
                for (const auto& choiceData : nodeData.choices) {
                    node.choices.push_back({choiceData.text, choiceData.nextNodeId});
                }
                
                tree.nodes.push_back(node);
            }
            
            DialogSystem::instance().addDialog(tree);
        }
    }
    
    static void loadRooms() {
        for (const auto& data : DataLoader::instance().getRooms()) {
            auto room = std::make_unique<Room>(data.id, data.name);
            room->setWalkArea(data.walkArea.minX, data.walkArea.maxX,
                             data.walkArea.minY, data.walkArea.maxY);
            
            for (const auto& hs : data.hotspots) {
                HotspotType type = HotspotType::None;
                if (hs.type == "npc") type = HotspotType::NPC;
                else if (hs.type == "item") type = HotspotType::Item;
                else if (hs.type == "exit") type = HotspotType::Exit;
                else if (hs.type == "examine") type = HotspotType::Examine;
                
                if (type == HotspotType::Exit) {
                    room->addExit(hs.id, hs.name, hs.x, hs.y, hs.w, hs.h, hs.targetRoom);
                } else {
                    room->addHotspot(hs.id, hs.name, hs.x, hs.y, hs.w, hs.h, type, 
                                     hs.dialogId, hs.examineText, hs.funnyFails);
                }
            }
            
            RoomManager::instance().addRoom(std::move(room));
        }
    }
    
    static void loadNPCs() {
        for (const auto& data : DataLoader::instance().getNPCs()) {
            Room* room = RoomManager::instance().getRoom(data.room);
            if (!room) {
                std::cerr << "Room not found for NPC: " << data.id << std::endl;
                continue;
            }
            
            auto npc = std::make_unique<NPC>(
                static_cast<float>(data.x),
                static_cast<float>(data.y),
                data.name
            );
            npc->setDialogId(data.dialogId);
            npc->setSpeed(data.moveSpeed);
            
            // Registrera NPC med AISystem
            AISystem::instance().registerNPC(npc.get());
            
            // Sätt beteende baserat på canMove
            if (data.canMove) {
                AISystem::instance().setBehavior(data.name, BehaviorType::Wander);
            } else {
                AISystem::instance().setBehavior(data.name, BehaviorType::Idle);
            }
            
            room->addNPC(std::move(npc));
            std::cout << "Loaded NPC: " << data.name << " in " << data.room << std::endl;
        }
    }
};
