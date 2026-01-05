/**
 * @file DialogSystem.cpp
 * @brief Implementation av dialog system
 */
#include "DialogSystem.h"
#include "../utils/Logger.h"
#include <iostream>

DialogSystem& DialogSystem::instance() {
    static DialogSystem instance;
    return instance;
}

void DialogSystem::addDialog(const DialogTree& tree) {
    m_dialogs[tree.id] = tree;
    LOG_INFO("Added dialog: " + tree.id + " with " + std::to_string(tree.nodes.size()) + " nodes");
}

bool DialogSystem::startDialog(const std::string& dialogId) {
    LOG_INFO("startDialog called: " + dialogId);
    auto it = m_dialogs.find(dialogId);
    if (it == m_dialogs.end()) {
        LOG_ERROR("Dialog not found: " + dialogId);
        return false;
    }
    
    m_currentDialog = &it->second;
    m_currentNodeId = m_currentDialog->startNodeId;
    m_active = true;
    
    LOG_INFO("Started dialog: " + dialogId + ", startNode=" + std::to_string(m_currentNodeId));
    return true;
}

void DialogSystem::endDialog() {
    m_active = false;
    m_currentDialog = nullptr;
    m_currentNodeId = 0;
    
    if (m_onDialogEnd) {
        m_onDialogEnd();
    }
    
    std::cout << "Dialog ended" << std::endl;
}

void DialogSystem::selectChoice(int index) {
    const DialogNode* node = getCurrentNode();
    if (!node || index < 0 || index >= static_cast<int>(node->choices.size())) {
        return;
    }
    
    int nextId = node->choices[index].nextNodeId;
    if (nextId < 0) {
        endDialog();
    } else {
        m_currentNodeId = nextId;
    }
}

void DialogSystem::advance() {
    const DialogNode* node = getCurrentNode();
    if (!node) {
        endDialog();
        return;
    }
    
    // Om det finns val, gör inget (vänta på selectChoice)
    if (!node->choices.empty()) {
        return;
    }
    
    // Fortsätt till nästa nod
    if (node->nextNodeId < 0) {
        endDialog();
    } else {
        m_currentNodeId = node->nextNodeId;
    }
}

const DialogNode* DialogSystem::getCurrentNode() const {
    return findNode(m_currentNodeId);
}

const std::string& DialogSystem::getCurrentSpeaker() const {
    static std::string empty;
    const DialogNode* node = getCurrentNode();
    return node ? node->speaker : empty;
}

const std::string& DialogSystem::getCurrentText() const {
    static std::string empty;
    const DialogNode* node = getCurrentNode();
    return node ? node->text : empty;
}

const std::vector<DialogChoice>& DialogSystem::getCurrentChoices() const {
    static std::vector<DialogChoice> empty;
    const DialogNode* node = getCurrentNode();
    return node ? node->choices : empty;
}

const DialogNode* DialogSystem::findNode(int id) const {
    if (!m_currentDialog) return nullptr;
    
    for (const auto& node : m_currentDialog->nodes) {
        if (node.id == id) {
            return &node;
        }
    }
    return nullptr;
}
