/**
 * @file WorldQuery.cpp
 * @brief Implementation av WorldBridge - delegerar till singletons
 */
#include "WorldQuery.h"
#include "WorldState.h"
#include "InventorySystem.h"
#include "QuestSystem.h"
#include "RoomManager.h"
#include "DialogSystem.h"
#include "CutsceneSystem.h"
#include "audio/AudioManager.h"

WorldBridge& WorldBridge::instance() {
    static WorldBridge inst;
    return inst;
}

// === IWorldQuery ===

bool WorldBridge::getFlag(std::string_view id) const {
    return WorldState::instance().getFlag(std::string(id));
}

bool WorldBridge::hasFlag(std::string_view id) const {
    return WorldState::instance().hasFlag(std::string(id));
}

int WorldBridge::getCounter(std::string_view id) const {
    return WorldState::instance().getCounter(std::string(id));
}

bool WorldBridge::hasItem(std::string_view id) const {
    return InventorySystem::instance().hasItem(std::string(id));
}

QuestStatus WorldBridge::getQuestStatus(std::string_view questId) const {
    auto* quest = QuestSystem::instance().getQuest(std::string(questId));
    return quest ? quest->status : QuestStatus::Inactive;
}

bool WorldBridge::isObjectiveComplete(std::string_view questId, std::string_view objId) const {
    auto* quest = QuestSystem::instance().getQuest(std::string(questId));
    if (!quest) return false;
    
    for (const auto& obj : quest->objectives) {
        if (obj.id == objId) {
            return obj.status == QuestStatus::Completed;
        }
    }
    return false;
}

std::string WorldBridge::getCurrentRoomId() const {
    auto* room = RoomManager::instance().getCurrentRoom();
    return room ? room->getId() : "";
}

// === IWorldMutator ===

void WorldBridge::setFlag(std::string_view id, bool value) {
    WorldState::instance().setFlag(std::string(id), value);
}

void WorldBridge::setCounter(std::string_view id, int value) {
    WorldState::instance().setCounter(std::string(id), value);
}

void WorldBridge::incrementCounter(std::string_view id, int amount) {
    WorldState::instance().incrementCounter(std::string(id), amount);
}

void WorldBridge::addItem(std::string_view id) {
    InventorySystem::instance().addItem(std::string(id));
}

void WorldBridge::removeItem(std::string_view id) {
    InventorySystem::instance().removeItem(std::string(id));
}

void WorldBridge::startQuest(std::string_view questId) {
    QuestSystem::instance().startQuest(std::string(questId));
}

void WorldBridge::completeObjective(std::string_view questId, std::string_view objId) {
    QuestSystem::instance().completeObjectiveById(std::string(questId), std::string(objId));
}

void WorldBridge::completeQuest(std::string_view questId) {
    QuestSystem::instance().completeQuest(std::string(questId));
}

void WorldBridge::changeRoom(std::string_view roomId) {
    RoomManager::instance().changeRoom(std::string(roomId));
}

void WorldBridge::startDialog(std::string_view dialogId) {
    DialogSystem::instance().startDialog(std::string(dialogId));
}

void WorldBridge::startCutscene(std::string_view cutsceneId) {
    CutsceneSystem::instance().play(std::string(cutsceneId));
}

void WorldBridge::playSound(std::string_view soundId) {
    AudioManager::instance().playSound(std::string(soundId));
}

void WorldBridge::playMusic(std::string_view musicId) {
    AudioManager::instance().crossfadeToMusic(std::string(musicId), 1000);
}
