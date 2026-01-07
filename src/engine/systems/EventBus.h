/**
 * @file EventBus.h
 * @brief Central event bus för löskopplad kommunikation mellan system
 * 
 * System publicerar events, andra system prenumererar på dem.
 * Eliminerar direkta beroenden mellan system.
 */
#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <any>
#include <typeindex>

/**
 * @brief Base event för alla game events
 */
struct GameEvent {
    virtual ~GameEvent() = default;
};

// ============================================================================
// KONKRETA EVENTS
// ============================================================================

struct FlagChangedEvent : GameEvent {
    std::string flag;
    bool value;
    FlagChangedEvent(const std::string& f, bool v) : flag(f), value(v) {}
};

struct CounterChangedEvent : GameEvent {
    std::string counter;
    int value;
    CounterChangedEvent(const std::string& c, int v) : counter(c), value(v) {}
};

struct ItemPickedUpEvent : GameEvent {
    std::string itemId;
    ItemPickedUpEvent(const std::string& id) : itemId(id) {}
};

struct ItemRemovedEvent : GameEvent {
    std::string itemId;
    ItemRemovedEvent(const std::string& id) : itemId(id) {}
};

struct QuestStartedEvent : GameEvent {
    std::string questId;
    QuestStartedEvent(const std::string& id) : questId(id) {}
};

struct QuestCompletedEvent : GameEvent {
    std::string questId;
    QuestCompletedEvent(const std::string& id) : questId(id) {}
};

struct ObjectiveCompletedEvent : GameEvent {
    std::string questId;
    std::string objectiveId;
    ObjectiveCompletedEvent(const std::string& q, const std::string& o) 
        : questId(q), objectiveId(o) {}
};

struct RoomChangedEvent : GameEvent {
    std::string fromRoom;
    std::string toRoom;
    RoomChangedEvent(const std::string& from, const std::string& to) 
        : fromRoom(from), toRoom(to) {}
};

struct DialogStartedEvent : GameEvent {
    std::string dialogId;
    DialogStartedEvent(const std::string& id) : dialogId(id) {}
};

struct DialogEndedEvent : GameEvent {
    std::string dialogId;
    DialogEndedEvent(const std::string& id) : dialogId(id) {}
};

struct DialogChoiceEvent : GameEvent {
    std::string dialogId;
    int choiceIndex;
    std::string choiceText;
    DialogChoiceEvent(const std::string& d, int i, const std::string& t) 
        : dialogId(d), choiceIndex(i), choiceText(t) {}
};

struct HotspotInteractedEvent : GameEvent {
    std::string hotspotId;
    std::string roomId;
    HotspotInteractedEvent(const std::string& h, const std::string& r) 
        : hotspotId(h), roomId(r) {}
};

struct GateOpenedEvent : GameEvent {
    std::string gateId;
    int approachType;
    GateOpenedEvent(const std::string& g, int a) : gateId(g), approachType(a) {}
};

struct CutsceneStartedEvent : GameEvent {
    std::string cutsceneId;
    CutsceneStartedEvent(const std::string& id) : cutsceneId(id) {}
};

struct CutsceneEndedEvent : GameEvent {
    std::string cutsceneId;
    CutsceneEndedEvent(const std::string& id) : cutsceneId(id) {}
};

struct MusicStateChangedEvent : GameEvent {
    int oldState;
    int newState;
    MusicStateChangedEvent(int o, int n) : oldState(o), newState(n) {}
};

/**
 * @brief Central event bus singleton
 */
class EventBus {
public:
    static EventBus& instance();
    
    /** @brief Subscribe till ett event-typ */
    template<typename T>
    void subscribe(std::function<void(const T&)> handler) {
        auto& handlers = m_handlers[std::type_index(typeid(T))];
        handlers.push_back([handler](const GameEvent& e) {
            handler(static_cast<const T&>(e));
        });
    }
    
    /** @brief Publicera ett event */
    template<typename T>
    void publish(const T& event) {
        auto it = m_handlers.find(std::type_index(typeid(T)));
        if (it != m_handlers.end()) {
            for (auto& handler : it->second) {
                handler(event);
            }
        }
        
        // Logga event för debugging
        logEvent(typeid(T).name());
    }
    
    /** @brief Rensa alla subscribers */
    void clear() { m_handlers.clear(); }
    
    /** @brief Aktivera/inaktivera event-loggning */
    void setLogging(bool enabled) { m_loggingEnabled = enabled; }

private:
    EventBus() = default;
    ~EventBus() = default;
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
    
    void logEvent(const char* eventName);
    
    using Handler = std::function<void(const GameEvent&)>;
    std::unordered_map<std::type_index, std::vector<Handler>> m_handlers;
    bool m_loggingEnabled = false;
};
