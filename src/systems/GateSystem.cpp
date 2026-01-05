/**
 * @file GateSystem.cpp
 * @brief Implementation av gate system med alternativa lösningar
 */
#include "GateSystem.h"
#include "QuestSystem.h"
#include "InventorySystem.h"
#include "DialogSystem.h"
#include "../utils/Logger.h"

GateSystem& GateSystem::instance() {
    static GateSystem inst;
    return inst;
}

void GateSystem::addGate(const Gate& gate) {
    m_gates[gate.id] = gate;
    LOG_INFO("Added gate: " + gate.id + " with " + std::to_string(gate.approaches.size()) + " approaches");
}

Gate* GateSystem::getGate(const std::string& gateId) {
    auto it = m_gates.find(gateId);
    if (it != m_gates.end()) {
        return &it->second;
    }
    return nullptr;
}

bool GateSystem::isGateOpen(const std::string& gateId) const {
    auto it = m_gates.find(gateId);
    return it != m_gates.end() && it->second.isOpen;
}

bool GateSystem::tryOpenGate(const std::string& gateId, ApproachType approach) {
    Gate* gate = getGate(gateId);
    if (!gate || gate->isOpen) {
        return false;
    }
    
    // Hitta approach
    for (auto& app : gate->approaches) {
        if (app.type == approach && checkApproachRequirements(app)) {
            gate->isOpen = true;
            app.completed = true;
            
            LOG_INFO("Gate opened: " + gateId + " via approach " + std::to_string(static_cast<int>(approach)));
            
            if (m_onGateOpened) {
                m_onGateOpened(gateId, approach);
            }
            
            return true;
        }
    }
    
    return false;
}

void GateSystem::completeApproach(const std::string& gateId, ApproachType approach) {
    Gate* gate = getGate(gateId);
    if (!gate) return;
    
    for (auto& app : gate->approaches) {
        if (app.type == approach) {
            app.completed = true;
            LOG_DEBUG("Approach completed: " + gateId + " type=" + std::to_string(static_cast<int>(approach)));
            
            // Kolla om gate kan öppnas nu
            tryOpenGate(gateId, approach);
            break;
        }
    }
}

std::vector<GateApproach> GateSystem::getAvailableApproaches(const std::string& gateId) const {
    std::vector<GateApproach> available;
    
    auto it = m_gates.find(gateId);
    if (it == m_gates.end()) {
        return available;
    }
    
    for (const auto& app : it->second.approaches) {
        if (checkApproachRequirements(app)) {
            available.push_back(app);
        }
    }
    
    return available;
}

void GateSystem::clear() {
    m_gates.clear();
}

bool GateSystem::checkApproachRequirements(const GateApproach& approach) const {
    // Kolla dialog-krav
    if (!approach.requiredDialog.empty()) {
        // TODO: Kolla om dialog är slutförd
        // För nu: anta att det är ok
    }
    
    // Kolla item-krav
    if (!approach.requiredItem.empty()) {
        if (!InventorySystem::instance().hasItem(approach.requiredItem)) {
            return false;
        }
    }
    
    // Kolla quest-krav
    if (!approach.requiredQuest.empty()) {
        // TODO: Kolla quest status
        // För nu: anta att det är ok
    }
    
    // Kolla flag-krav
    if (!approach.requiredFlag.empty()) {
        // TODO: Kolla flags i SaveSystem
        // För nu: anta att det är ok
    }
    
    return true;
}
