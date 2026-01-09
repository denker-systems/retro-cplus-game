/**
 * @file GameDataLoader.h
 * @brief DEPRECATED: Hjälpfunktioner för att ladda JSON-data till spelsystemen
 * 
 * MIGRATION NOTE: GameDataLoader contains duplicated logic that should be
 * distributed to individual System classes:
 * - loadItems() → InventorySystem::loadFromData()
 * - loadQuests() → QuestSystem::loadFromData()
 * - loadDialogs() → DialogSystem::loadFromData()
 * - loadRooms() → RoomManager::loadFromData()
 * - loadScenes() → SceneManager::loadFromData()
 * - loadNPCs() → AISystem::loadFromData()
 * 
 * This class will be removed once all logic is moved to respective Systems.
 */
#pragma once

#include "DataLoader.h"
#include "systems/InventorySystem.h"
#include "systems/QuestSystem.h"
#include "systems/DialogSystem.h"
#include "systems/RoomManager.h"
#include "systems/SceneManager.h"
#include "systems/AISystem.h"
#include "Room.h"
#include "world/Scene.h"
#include "actors/NPC.h"
#include "components/SpriteComponent.h"
#include <SDL_image.h>
#include <iostream>

/**
 * @brief DEPRECATED: Laddar all speldata från JSON och registrerar i spelsystemen
 */
class GameDataLoader {
public:
    /** @brief Ladda och registrera all data */
    static bool loadAll(SDL_Renderer* renderer = nullptr) {
        if (!DataLoader::instance().loadAll()) {
            return false;
        }
        
        loadItems();
        loadQuests();
        loadDialogs();
        loadScenes(renderer);  // NPCs skapas nu automatiskt i Scene::createFromData
        
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
    
    /** @brief Reload-metoder för hot reload */
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
                    DialogChoice choice;
                    choice.text = choiceData.text;
                    choice.nextNodeId = choiceData.nextNodeId;
                    choice.condition = choiceData.condition;
                    choice.tone = choiceData.tone;
                    choice.preview = choiceData.preview;
                    node.choices.push_back(choice);
                }
                
                tree.nodes.push_back(node);
            }
            
            DialogSystem::instance().addDialog(tree);
        }
    }
    
    static void loadRooms(SDL_Renderer* renderer) {
        for (const auto& data : DataLoader::instance().getRooms()) {
            auto room = std::make_unique<Room>(data.id, data.name);
            room->setWalkArea(data.walkArea.minX, data.walkArea.maxX,
                             data.walkArea.minY, data.walkArea.maxY,
                             data.walkArea.scaleTop, data.walkArea.scaleBottom);
            room->setPlayerSpawn(data.playerSpawnX, data.playerSpawnY);
            
            // Ladda layers om de finns
            if (!data.layers.empty()) {
                for (const auto& layer : data.layers) {
                    room->loadLayer(renderer, layer.image, layer.zIndex, 
                                   layer.parallaxX, layer.parallaxY, layer.opacity);
                }
            }
            // Background loaded via SpriteComponent in Scene::createFromData
            
            for (const auto& hs : data.hotspots) {
                HotspotType type = HotspotType::None;
                if (hs.type == "npc") type = HotspotType::NPC;
                else if (hs.type == "item") type = HotspotType::Item;
                else if (hs.type == "exit") type = HotspotType::Exit;
                else if (hs.type == "examine") type = HotspotType::Examine;
                
                if (type == HotspotType::Exit) {
                    room->addExit(hs.id, hs.name, hs.x, hs.y, hs.w, hs.h, hs.targetScene);
                } else {
                    room->addHotspot(hs.id, hs.name, hs.x, hs.y, hs.w, hs.h, type, 
                                     hs.dialogId, hs.examineText, hs.funnyFails);
                }
            }
            
            RoomManager::instance().addRoom(std::move(room));
        }
    }
    
    static void loadScenes(SDL_Renderer* renderer) {
        for (const auto& data : DataLoader::instance().getRooms()) {
            // Use Scene::createFromData to create actors automatically
            auto scene = engine::Scene::createFromData(data);
            
            // Set grid data if available
            if (data.gridPosition) {
                scene->setGridPosition(*data.gridPosition);
                std::cout << "  Grid position: (" << data.gridPosition->gridX 
                         << "," << data.gridPosition->gridY << ") "
                         << data.gridPosition->pixelWidth << "x" 
                         << data.gridPosition->pixelHeight << std::endl;
            }
            
            if (data.camera) {
                scene->setCameraConfig(*data.camera);
                std::cout << "  Camera: zoom=" << data.camera->zoom << std::endl;
            }
            
            // Ladda layers om de finns
            if (!data.layers.empty()) {
                for (const auto& layer : data.layers) {
                    scene->loadLayer(renderer, layer.image, layer.zIndex, 
                                   layer.parallaxX, layer.parallaxY, layer.opacity);
                }
            }
            
            // Load background texture to Background actor's SpriteComponent
            if (!data.background.empty() && renderer) {
                std::cout << "[DEBUG] Loading background for scene: " << data.name << " -> " << data.background << std::endl;
                std::cout << "[DEBUG] Scene has " << scene->getActors().size() << " actors" << std::endl;
                
                // List all actors in scene
                for (const auto& actor : scene->getActors()) {
                    std::cout << "[DEBUG]   Actor: " << actor->getName() << std::endl;
                }
                
                auto* bgActor = scene->findActor("Background");
                if (bgActor) {
                    std::cout << "[DEBUG] Found Background actor" << std::endl;
                    auto* spriteComp = bgActor->getComponent<engine::SpriteComponent>();
                    if (spriteComp) {
                        std::cout << "[DEBUG] Found SpriteComponent on Background" << std::endl;
                        // data.background already contains full path like "assets/backgrounds/tavern-inside.png"
                        SDL_Texture* tex = IMG_LoadTexture(renderer, data.background.c_str());
                        if (tex) {
                            std::cout << "[DEBUG] Successfully loaded background texture: " << data.background << std::endl;
                            spriteComp->setTexture(tex);
                            // Query texture size
                            int w, h;
                            SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
                            spriteComp->setSize(w, h);
                            spriteComp->setSourceRect({0, 0, w, h});
                            std::cout << "[DEBUG] Background size: " << w << "x" << h << std::endl;
                        } else {
                            std::cout << "[ERROR] Failed to load background: " << data.background << std::endl;
                        }
                    } else {
                        std::cout << "[ERROR] No SpriteComponent on Background actor" << std::endl;
                    }
                } else {
                    std::cout << "[ERROR] Background actor not found in scene!" << std::endl;
                }
            }
            
            // Load NPC sprites for all NPCs in this scene
            if (renderer) {
                const auto& npcList = DataLoader::instance().getNPCs();
                for (const auto& npcData : npcList) {
                    if (npcData.room == data.name) {
                        std::cout << "[DEBUG] Loading NPC sprite for: " << npcData.name << " (sprite: " << npcData.sprite << ")" << std::endl;
                        auto* npcActor = scene->findActor(npcData.name);
                        if (npcActor) {
                            std::cout << "[DEBUG] Found NPC actor: " << npcData.name << std::endl;
                            auto* spriteComp = npcActor->getComponent<engine::SpriteComponent>();
                            if (spriteComp && !npcData.sprite.empty()) {
                                std::string spritePath = "assets/sprites/" + npcData.sprite + ".png";
                                SDL_Texture* tex = IMG_LoadTexture(renderer, spritePath.c_str());
                                if (tex) {
                                    std::cout << "[DEBUG] Successfully loaded NPC texture: " << spritePath << std::endl;
                                    spriteComp->setTexture(tex);
                                    int w, h;
                                    SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
                                    spriteComp->setSize(w, h);
                                    spriteComp->setSourceRect({0, 0, w, h});
                                    std::cout << "[DEBUG] NPC sprite size: " << w << "x" << h << std::endl;
                                } else {
                                    std::cout << "[ERROR] Failed to load NPC texture: " << spritePath << std::endl;
                                }
                            } else {
                                std::cout << "[ERROR] No SpriteComponent on NPC actor or empty sprite name" << std::endl;
                            }
                        } else {
                            std::cout << "[ERROR] NPC actor not found: " << npcData.name << std::endl;
                        }
                    }
                }
            }
            
            for (const auto& hs : data.hotspots) {
                HotspotType type = HotspotType::None;
                if (hs.type == "npc") type = HotspotType::NPC;
                else if (hs.type == "item") type = HotspotType::Item;
                else if (hs.type == "exit") type = HotspotType::Exit;
                else if (hs.type == "examine") type = HotspotType::Examine;
                
                if (type == HotspotType::Exit) {
                    scene->addExit(hs.id, hs.name, hs.x, hs.y, hs.w, hs.h, hs.targetScene);
                } else {
                    scene->addHotspot(hs.id, hs.name, hs.x, hs.y, hs.w, hs.h, type, 
                                     hs.dialogId, hs.examineText, hs.funnyFails);
                }
            }
            
            SceneManager::instance().addScene(std::move(scene));
        }
    }
    
    static void loadNPCsToScenes() {
        for (const auto& data : DataLoader::instance().getNPCs()) {
            engine::Scene* scene = SceneManager::instance().getScene(data.room);
            if (!scene) {
                std::cerr << "Scene not found for NPC: " << data.id << std::endl;
                continue;
            }
            
            // Hitta hotspot med samma ID för att få position
            const auto& hotspots = scene->getHotspots();
            float npcX = static_cast<float>(data.x);
            float npcY = static_cast<float>(data.y);
            
            for (const auto& hs : hotspots) {
                if (hs.id == data.id && hs.type == HotspotType::NPC) {
                    // Använd hotspot-position (centrera NPC i hotspot)
                    npcX = static_cast<float>(hs.rect.x + hs.rect.w / 2 - 16);
                    npcY = static_cast<float>(hs.rect.y + hs.rect.h - 48);
                    break;
                }
            }
            
            auto npc = std::make_unique<engine::actors::NPC>(npcX, npcY, data.name);
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
            
            scene->addNPC(std::move(npc));
            std::cout << "Loaded NPC: " << data.name << " in " << data.room << std::endl;
        }
    }
    
    // Legacy - behålls för bakåtkompatibilitet med RoomManager
    static void loadNPCs() {
        for (const auto& data : DataLoader::instance().getNPCs()) {
            Room* room = RoomManager::instance().getRoom(data.room);
            if (!room) {
                std::cerr << "Room not found for NPC: " << data.id << std::endl;
                continue;
            }
            
            // Hitta hotspot med samma ID för att få position
            const auto& hotspots = room->getHotspots();
            float npcX = static_cast<float>(data.x);
            float npcY = static_cast<float>(data.y);
            
            for (const auto& hs : hotspots) {
                if (hs.id == data.id && hs.type == HotspotType::NPC) {
                    // Använd hotspot-position (centrera NPC i hotspot)
                    npcX = static_cast<float>(hs.rect.x + hs.rect.w / 2 - 16);  // 16 = halva NPC-bredden
                    npcY = static_cast<float>(hs.rect.y + hs.rect.h - 48);      // 48 = NPC-höjd (fötter vid botten)
                    break;
                }
            }
            
            auto npc = std::make_unique<engine::actors::NPC>(npcX, npcY, data.name);
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
