/**
 * @file NPC.h
 * @brief Non-Player Character klass
 */
#pragma once

#include "Character.h"
#include <string>

/**
 * @brief NPC-karaktär med dialog och beteende
 */
class NPC : public Character {
public:
    NPC(float x, float y, const std::string& name);
    ~NPC() override = default;
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    
    /** @brief Sätt dialog-ID för denna NPC */
    void setDialogId(const std::string& dialogId) { m_dialogId = dialogId; }
    const std::string& getDialogId() const { return m_dialogId; }
    
    /** @brief Kolla om NPC kan prata */
    bool canTalk() const { return !m_dialogId.empty(); }

private:
    std::string m_dialogId;
};
