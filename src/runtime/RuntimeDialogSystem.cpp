/**
 * @file RuntimeDialogSystem.cpp
 * @brief Implementation av dialog system för runtime
 */
#include "RuntimeDialogSystem.h"
#include "engine/data/DataLoader.h"
#include "engine/utils/Logger.h"

RuntimeDialogSystem::RuntimeDialogSystem() = default;

void RuntimeDialogSystem::loadDialogs() {
    auto& dataLoader = DataLoader::instance();
    const auto& dialogData = dataLoader.getDialogs();
    
    m_dialogs.clear();
    
    for (const auto& d : dialogData) {
        Dialog dialog;
        dialog.id = d.id;
        
        for (const auto& n : d.nodes) {
            DialogNode node;
            node.id = n.id;
            node.speaker = n.speaker;
            node.text = n.text;
            node.nextNodeId = n.nextNodeId;
            node.action = n.action;
            
            for (const auto& c : n.choices) {
                DialogChoice choice;
                choice.text = c.text;
                choice.nextNodeId = c.nextNodeId;
                choice.condition = c.condition;
                node.choices.push_back(choice);
            }
            
            dialog.nodes.push_back(node);
        }
        
        m_dialogs.push_back(dialog);
    }
    
    LOG_INFO("[RuntimeDialogSystem] Loaded " + std::to_string(m_dialogs.size()) + " dialogs");
}

bool RuntimeDialogSystem::startDialog(const std::string& dialogId) {
    Dialog* dialog = findDialog(dialogId);
    if (!dialog) {
        LOG_WARNING("[RuntimeDialogSystem] Dialog not found: " + dialogId);
        return false;
    }
    
    m_currentDialog = dialog;
    m_active = true;
    
    // Start at first node (id=0)
    setCurrentNode(0);
    
    LOG_INFO("[RuntimeDialogSystem] Started dialog: " + dialogId);
    return true;
}

void RuntimeDialogSystem::endDialog() {
    m_active = false;
    m_currentDialog = nullptr;
    m_currentNodeId = -1;
    m_currentSpeaker.clear();
    m_currentText.clear();
    m_currentChoices.clear();
    
    LOG_INFO("[RuntimeDialogSystem] Dialog ended");
}

void RuntimeDialogSystem::selectChoice(int choiceIndex) {
    if (!m_active || choiceIndex < 0 || choiceIndex >= (int)m_currentChoices.size()) {
        return;
    }
    
    int nextNodeId = m_currentChoices[choiceIndex].nextNodeId;
    
    if (nextNodeId == -1) {
        endDialog();
    } else {
        setCurrentNode(nextNodeId);
    }
}

void RuntimeDialogSystem::advance() {
    if (!m_active || !m_currentDialog) return;
    
    // Find current node
    for (const auto& node : m_currentDialog->nodes) {
        if (node.id == m_currentNodeId) {
            if (node.nextNodeId == -1) {
                endDialog();
            } else {
                setCurrentNode(node.nextNodeId);
            }
            return;
        }
    }
    
    endDialog();
}

void RuntimeDialogSystem::setCurrentNode(int nodeId) {
    if (!m_currentDialog) return;
    
    // Find node
    for (const auto& node : m_currentDialog->nodes) {
        if (node.id == nodeId) {
            m_currentNodeId = nodeId;
            m_currentSpeaker = node.speaker;
            m_currentText = node.text;
            m_currentChoices = node.choices;
            
            // Trigger action if present
            if (!node.action.empty() && m_actionCallback) {
                m_actionCallback(node.action);
            }
            
            LOG_INFO("[RuntimeDialogSystem] Node " + std::to_string(nodeId) + 
                     ": " + m_currentSpeaker + " says: " + m_currentText);
            return;
        }
    }
    
    LOG_WARNING("[RuntimeDialogSystem] Node not found: " + std::to_string(nodeId));
    endDialog();
}

Dialog* RuntimeDialogSystem::findDialog(const std::string& id) {
    for (auto& dialog : m_dialogs) {
        if (dialog.id == id) {
            return &dialog;
        }
    }
    return nullptr;
}
