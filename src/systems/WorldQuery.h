/**
 * @file WorldQuery.h
 * @brief Interfaces för att query/mutera världstillstånd utan singleton-beroenden
 * 
 * DSL-lagret (ConditionEvaluator/ActionExecutor) använder dessa interfaces
 * istället för att direkt anropa singletons. Gör testbarhet och framtida
 * refaktorering mycket enklare.
 */
#pragma once

#include <string>
#include <string_view>

// Forward declarations
enum class QuestStatus;

/**
 * @brief Interface för att läsa världstillstånd (read-only)
 */
struct IWorldQuery {
    virtual ~IWorldQuery() = default;
    
    // Flags
    virtual bool getFlag(std::string_view id) const = 0;
    virtual bool hasFlag(std::string_view id) const = 0;
    
    // Counters
    virtual int getCounter(std::string_view id) const = 0;
    
    // Inventory
    virtual bool hasItem(std::string_view id) const = 0;
    
    // Quests
    virtual QuestStatus getQuestStatus(std::string_view questId) const = 0;
    virtual bool isObjectiveComplete(std::string_view questId, std::string_view objId) const = 0;
    
    // Room
    virtual std::string getCurrentRoomId() const = 0;
};

/**
 * @brief Interface för att mutera världstillstånd (write)
 */
struct IWorldMutator {
    virtual ~IWorldMutator() = default;
    
    // Flags
    virtual void setFlag(std::string_view id, bool value) = 0;
    
    // Counters
    virtual void setCounter(std::string_view id, int value) = 0;
    virtual void incrementCounter(std::string_view id, int amount = 1) = 0;
    
    // Inventory
    virtual void addItem(std::string_view id) = 0;
    virtual void removeItem(std::string_view id) = 0;
    
    // Quests
    virtual void startQuest(std::string_view questId) = 0;
    virtual void completeObjective(std::string_view questId, std::string_view objId) = 0;
    virtual void completeQuest(std::string_view questId) = 0;
    
    // Room
    virtual void changeRoom(std::string_view roomId) = 0;
    
    // Dialog
    virtual void startDialog(std::string_view dialogId) = 0;
    
    // Cutscene
    virtual void startCutscene(std::string_view cutsceneId) = 0;
    
    // Audio
    virtual void playSound(std::string_view soundId) = 0;
    virtual void playMusic(std::string_view musicId) = 0;
};

/**
 * @brief Konkret implementation som delegerar till singletons
 */
class WorldBridge : public IWorldQuery, public IWorldMutator {
public:
    static WorldBridge& instance();
    
    // === IWorldQuery ===
    bool getFlag(std::string_view id) const override;
    bool hasFlag(std::string_view id) const override;
    int getCounter(std::string_view id) const override;
    bool hasItem(std::string_view id) const override;
    QuestStatus getQuestStatus(std::string_view questId) const override;
    bool isObjectiveComplete(std::string_view questId, std::string_view objId) const override;
    std::string getCurrentRoomId() const override;
    
    // === IWorldMutator ===
    void setFlag(std::string_view id, bool value) override;
    void setCounter(std::string_view id, int value) override;
    void incrementCounter(std::string_view id, int amount = 1) override;
    void addItem(std::string_view id) override;
    void removeItem(std::string_view id) override;
    void startQuest(std::string_view questId) override;
    void completeObjective(std::string_view questId, std::string_view objId) override;
    void completeQuest(std::string_view questId) override;
    void changeRoom(std::string_view roomId) override;
    void startDialog(std::string_view dialogId) override;
    void startCutscene(std::string_view cutsceneId) override;
    void playSound(std::string_view soundId) override;
    void playMusic(std::string_view musicId) override;

private:
    WorldBridge() = default;
};
