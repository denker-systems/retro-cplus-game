/**
 * @file WorldState.cpp
 * @brief Implementation av central narrativ state
 */
#include "WorldState.h"
#include "utils/Logger.h"

WorldState& WorldState::instance() {
    static WorldState inst;
    return inst;
}

// === Flags ===

void WorldState::setFlag(const std::string& key, bool value) {
    bool changed = (m_flags.find(key) == m_flags.end() || m_flags[key] != value);
    m_flags[key] = value;
    
    if (changed) {
        LOG_DEBUG("Flag set: " + key + " = " + (value ? "true" : "false"));
        notifyFlagChange(key);
    }
}

bool WorldState::getFlag(const std::string& key) const {
    auto it = m_flags.find(key);
    return it != m_flags.end() ? it->second : false;
}

bool WorldState::hasFlag(const std::string& key) const {
    return m_flags.find(key) != m_flags.end();
}

// === Counters ===

void WorldState::setCounter(const std::string& key, int value) {
    bool changed = (m_counters.find(key) == m_counters.end() || m_counters[key] != value);
    m_counters[key] = value;
    
    if (changed) {
        LOG_DEBUG("Counter set: " + key + " = " + std::to_string(value));
        notifyCounterChange(key);
    }
}

int WorldState::getCounter(const std::string& key) const {
    auto it = m_counters.find(key);
    return it != m_counters.end() ? it->second : 0;
}

void WorldState::incrementCounter(const std::string& key, int amount) {
    setCounter(key, getCounter(key) + amount);
}

void WorldState::decrementCounter(const std::string& key, int amount) {
    setCounter(key, getCounter(key) - amount);
}

// === Variables ===

void WorldState::setVar(const std::string& key, const WorldValue& value) {
    m_vars[key] = value;
}

WorldValue WorldState::getVar(const std::string& key) const {
    auto it = m_vars.find(key);
    if (it != m_vars.end()) {
        return it->second;
    }
    return false; // Default
}

bool WorldState::hasVar(const std::string& key) const {
    return m_vars.find(key) != m_vars.end();
}

void WorldState::setString(const std::string& key, const std::string& value) {
    setVar(key, value);
}

std::string WorldState::getString(const std::string& key) const {
    auto val = getVar(key);
    if (std::holds_alternative<std::string>(val)) {
        return std::get<std::string>(val);
    }
    return "";
}

// === Serialization ===

void WorldState::loadFlags(const std::unordered_map<std::string, bool>& flags) {
    m_flags = flags;
    LOG_INFO("Loaded " + std::to_string(flags.size()) + " flags");
}

void WorldState::loadCounters(const std::unordered_map<std::string, int>& counters) {
    m_counters = counters;
    LOG_INFO("Loaded " + std::to_string(counters.size()) + " counters");
}

// === Reset ===

void WorldState::clear() {
    m_flags.clear();
    m_counters.clear();
    m_vars.clear();
    LOG_INFO("WorldState cleared");
}

// === Notifications ===

void WorldState::notifyFlagChange(const std::string& key) {
    for (auto& callback : m_flagListeners) {
        callback(key);
    }
}

void WorldState::notifyCounterChange(const std::string& key) {
    for (auto& callback : m_counterListeners) {
        callback(key);
    }
}
