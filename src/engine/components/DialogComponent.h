/**
 * @file DialogComponent.h
 * @brief Dialog Component
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include <string>

namespace engine {

/**
 * @brief Component for dialog functionality
 */
class DialogComponent : public ActorComponent {
public:
    DialogComponent(const std::string& name = "DialogComponent");
    virtual ~DialogComponent() = default;
    
    void setDialogId(const std::string& dialogId) { m_dialogId = dialogId; }
    const std::string& getDialogId() const { return m_dialogId; }
    
    void setGreeting(const std::string& greeting) { m_greeting = greeting; }
    const std::string& getGreeting() const { return m_greeting; }
    
    bool canTalk() const { return !m_dialogId.empty(); }
    
private:
    std::string m_dialogId;
    std::string m_greeting;
};

} // namespace engine
