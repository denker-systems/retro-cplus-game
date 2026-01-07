/**
 * @file NPC.h
 * @brief Non-Player Character using Actor system
 */
#pragma once

#include "CharacterActor.h"
#include <string>

namespace engine {
namespace actors {

/**
 * @brief NPC-karaktär med dialog och beteende
 * 
 * Migrated from legacy Entity system to Actor/Component system.
 * Wraps NPCActor for compatibility with existing Room system.
 */
class NPC {
public:
    NPC(float x, float y, const std::string& name);
    ~NPC() = default;
    
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    
    /** @brief Sätt dialog-ID för denna NPC */
    void setDialogId(const std::string& dialogId) { m_dialogId = dialogId; }
    const std::string& getDialogId() const { return m_dialogId; }
    
    /** @brief Kolla om NPC kan prata */
    bool canTalk() const { return !m_dialogId.empty(); }
    
    /** @brief Get underlying actor */
    engine::NPCActor* getActor() { return m_actor.get(); }
    
    /** @brief Hämta namn */
    const std::string& getName() const;
    
    /** @brief Interagera med NPC */
    void interact();
    
    // Position methods for compatibility
    void setPosition(float x, float y);
    void getPosition(float& x, float& y) const;
    float getX() const;
    float getY() const;
    
    /** @brief Sätt rörelsehastighet */
    void setSpeed(float speed);
    float getSpeed() const;

private:
    std::unique_ptr<engine::NPCActor> m_actor;
    std::string m_dialogId;
};

} // namespace actors
} // namespace engine
