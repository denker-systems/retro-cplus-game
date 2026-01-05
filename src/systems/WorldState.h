/**
 * @file WorldState.h
 * @brief Central sanningskälla för all narrativ state (flags, counters, variabler)
 * 
 * Ersätter spridda globala singletons för story state.
 * Alla system läser/skriver via WorldState istället för direkt åtkomst.
 */
#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <functional>
#include <vector>

/**
 * @brief Värdetyp för variabler (bool, int, float, string)
 */
using WorldValue = std::variant<bool, int, float, std::string>;

/**
 * @brief Central state för all narrativ data
 */
class WorldState {
public:
    static WorldState& instance();
    
    // === Flags (bool) ===
    void setFlag(const std::string& key, bool value);
    bool getFlag(const std::string& key) const;
    bool hasFlag(const std::string& key) const;
    
    // === Counters (int) ===
    void setCounter(const std::string& key, int value);
    int getCounter(const std::string& key) const;
    void incrementCounter(const std::string& key, int amount = 1);
    void decrementCounter(const std::string& key, int amount = 1);
    
    // === Variables (any type) ===
    void setVar(const std::string& key, const WorldValue& value);
    WorldValue getVar(const std::string& key) const;
    bool hasVar(const std::string& key) const;
    
    // === String variables (convenience) ===
    void setString(const std::string& key, const std::string& value);
    std::string getString(const std::string& key) const;
    
    // === Serialization ===
    std::unordered_map<std::string, bool> getAllFlags() const { return m_flags; }
    std::unordered_map<std::string, int> getAllCounters() const { return m_counters; }
    std::unordered_map<std::string, WorldValue> getAllVars() const { return m_vars; }
    
    void loadFlags(const std::unordered_map<std::string, bool>& flags);
    void loadCounters(const std::unordered_map<std::string, int>& counters);
    
    // === Reset ===
    void clear();
    
    // === Change listeners ===
    using ChangeCallback = std::function<void(const std::string& key)>;
    void addFlagListener(ChangeCallback callback) { m_flagListeners.push_back(callback); }
    void addCounterListener(ChangeCallback callback) { m_counterListeners.push_back(callback); }

private:
    WorldState() = default;
    ~WorldState() = default;
    WorldState(const WorldState&) = delete;
    WorldState& operator=(const WorldState&) = delete;
    
    void notifyFlagChange(const std::string& key);
    void notifyCounterChange(const std::string& key);
    
    std::unordered_map<std::string, bool> m_flags;
    std::unordered_map<std::string, int> m_counters;
    std::unordered_map<std::string, WorldValue> m_vars;
    
    std::vector<ChangeCallback> m_flagListeners;
    std::vector<ChangeCallback> m_counterListeners;
};
