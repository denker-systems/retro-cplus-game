/**
 * @file CutsceneSystem.cpp
 * @brief Implementation av CutsceneSystem
 */
#include "CutsceneSystem.h"
#include "RoomManager.h"
#include "AISystem.h"
#include "SaveSystem.h"
#include "../graphics/Transition.h"
#include "../graphics/FontManager.h"
#include <SDL.h>
#include <iostream>

CutsceneSystem& CutsceneSystem::instance() {
    static CutsceneSystem inst;
    return inst;
}

void CutsceneSystem::addCutscene(const Cutscene& cutscene) {
    m_cutscenes[cutscene.id] = cutscene;
    std::cout << "Registered cutscene: " << cutscene.id << std::endl;
}

bool CutsceneSystem::play(const std::string& cutsceneId) {
    auto it = m_cutscenes.find(cutsceneId);
    if (it == m_cutscenes.end()) {
        std::cerr << "Cutscene not found: " << cutsceneId << std::endl;
        return false;
    }
    
    m_currentCutscene = &it->second;
    m_currentActionIndex = 0;
    m_actionTimer = 0.0f;
    m_playing = true;
    m_showText = false;
    m_fading = false;
    m_waitingForMovement = false;
    
    std::cout << "Playing cutscene: " << cutsceneId << std::endl;
    
    if (!m_currentCutscene->actions.empty()) {
        executeAction(m_currentCutscene->actions[0]);
    }
    
    return true;
}

void CutsceneSystem::stop() {
    m_playing = false;
    m_currentCutscene = nullptr;
    m_showText = false;
    m_fading = false;
    
    if (m_onComplete) {
        m_onComplete();
    }
}

void CutsceneSystem::skip() {
    if (!m_playing || !m_currentCutscene) return;
    if (!m_currentCutscene->skippable) return;
    
    // Skippa till slutet
    stop();
}

void CutsceneSystem::update(float deltaTime) {
    if (!m_playing || !m_currentCutscene) return;
    
    // Uppdatera fade
    if (m_fading) {
        if (m_fadeAlpha < m_fadeTarget) {
            m_fadeAlpha += m_fadeSpeed * deltaTime;
            if (m_fadeAlpha >= m_fadeTarget) {
                m_fadeAlpha = m_fadeTarget;
                m_fading = false;
            }
        } else if (m_fadeAlpha > m_fadeTarget) {
            m_fadeAlpha -= m_fadeSpeed * deltaTime;
            if (m_fadeAlpha <= m_fadeTarget) {
                m_fadeAlpha = m_fadeTarget;
                m_fading = false;
            }
        }
    }
    
    // Uppdatera timer
    m_actionTimer += deltaTime;
    
    // Kolla om nuvarande action är klar
    if (isCurrentActionComplete()) {
        advanceToNextAction();
    }
}

void CutsceneSystem::render(SDL_Renderer* renderer) {
    if (!m_playing) return;
    
    // Rita fade overlay
    if (m_fadeAlpha > 0.0f) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>(m_fadeAlpha * 255));
        SDL_Rect fullscreen = {0, 0, 640, 400};
        SDL_RenderFillRect(renderer, &fullscreen);
    }
    
    // Rita text om det finns
    if (m_showText && !m_currentText.empty()) {
        // Text bakgrund
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_Rect textBg = {20, 300, 600, 80};
        SDL_RenderFillRect(renderer, &textBg);
        
        // Ram
        SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
        SDL_RenderDrawRect(renderer, &textBg);
        
        // Speaker namn
        if (!m_currentSpeaker.empty()) {
            FontManager::instance().renderText(renderer, "default",
                m_currentSpeaker, 30, 308, {200, 180, 100, 255});
        }
        
        // Text
        int textY = m_currentSpeaker.empty() ? 320 : 330;
        FontManager::instance().renderText(renderer, "default",
            m_currentText, 30, textY, {255, 255, 255, 255});
    }
}

void CutsceneSystem::executeAction(const CutsceneAction& action) {
    m_actionTimer = 0.0f;
    
    switch (action.type) {
        case CutsceneActionType::Wait:
            // Bara vänta, timer hanterar det
            break;
            
        case CutsceneActionType::ShowText:
            m_showText = true;
            m_currentText = action.text;
            m_currentSpeaker = action.speaker;
            break;
            
        case CutsceneActionType::HideText:
            m_showText = false;
            m_currentText.clear();
            m_currentSpeaker.clear();
            break;
            
        case CutsceneActionType::FadeIn:
            m_fading = true;
            m_fadeAlpha = 1.0f;
            m_fadeTarget = 0.0f;
            m_fadeSpeed = 1.0f / action.duration;
            break;
            
        case CutsceneActionType::FadeOut:
            m_fading = true;
            m_fadeAlpha = 0.0f;
            m_fadeTarget = 1.0f;
            m_fadeSpeed = 1.0f / action.duration;
            break;
            
        case CutsceneActionType::MoveNPC:
            // Sätt NPC-mål via AISystem
            {
                Behavior behavior;
                behavior.type = BehaviorType::GoTo;
                Waypoint wp;
                wp.x = action.x;
                wp.y = action.y;
                behavior.waypoints.push_back(wp);
                AISystem::instance().setBehavior(action.targetId, behavior);
                m_waitingForMovement = action.waitForCompletion;
            }
            break;
            
        case CutsceneActionType::ChangeRoom:
            RoomManager::instance().changeRoom(action.targetId);
            break;
            
        case CutsceneActionType::SetFlag:
            SaveSystem::instance().setFlag(action.targetId, true);
            break;
            
        case CutsceneActionType::Callback:
            if (action.callback) {
                action.callback();
            }
            break;
            
        default:
            break;
    }
}

void CutsceneSystem::advanceToNextAction() {
    if (!m_currentCutscene) return;
    
    m_currentActionIndex++;
    
    if (m_currentActionIndex >= m_currentCutscene->actions.size()) {
        // Cutscene klar
        std::cout << "Cutscene completed: " << m_currentCutscene->id << std::endl;
        stop();
        return;
    }
    
    executeAction(m_currentCutscene->actions[m_currentActionIndex]);
}

bool CutsceneSystem::isCurrentActionComplete() const {
    if (!m_currentCutscene || m_currentActionIndex >= m_currentCutscene->actions.size()) {
        return true;
    }
    
    const auto& action = m_currentCutscene->actions[m_currentActionIndex];
    
    switch (action.type) {
        case CutsceneActionType::Wait:
            return m_actionTimer >= action.duration;
            
        case CutsceneActionType::ShowText:
            // Text stannar tills nästa action
            return m_actionTimer >= action.duration;
            
        case CutsceneActionType::HideText:
            return true;  // Instant
            
        case CutsceneActionType::FadeIn:
        case CutsceneActionType::FadeOut:
            return !m_fading;
            
        case CutsceneActionType::MoveNPC:
        case CutsceneActionType::MovePlayer:
            if (!action.waitForCompletion) return true;
            // TODO: Kolla om NPC nått målet
            return m_actionTimer >= 2.0f;  // Fallback timeout
            
        case CutsceneActionType::ChangeRoom:
        case CutsceneActionType::SetFlag:
        case CutsceneActionType::Callback:
            return true;  // Instant
            
        default:
            return true;
    }
}

// ============================================================================
// FACTORY METHODS
// ============================================================================

CutsceneAction CutsceneSystem::wait(float seconds) {
    CutsceneAction action;
    action.type = CutsceneActionType::Wait;
    action.duration = seconds;
    return action;
}

CutsceneAction CutsceneSystem::showText(const std::string& text, const std::string& speaker) {
    CutsceneAction action;
    action.type = CutsceneActionType::ShowText;
    action.text = text;
    action.speaker = speaker;
    action.duration = 3.0f;  // Default visning
    return action;
}

CutsceneAction CutsceneSystem::hideText() {
    CutsceneAction action;
    action.type = CutsceneActionType::HideText;
    return action;
}

CutsceneAction CutsceneSystem::fadeIn(float duration) {
    CutsceneAction action;
    action.type = CutsceneActionType::FadeIn;
    action.duration = duration;
    return action;
}

CutsceneAction CutsceneSystem::fadeOut(float duration) {
    CutsceneAction action;
    action.type = CutsceneActionType::FadeOut;
    action.duration = duration;
    return action;
}

CutsceneAction CutsceneSystem::moveNPC(const std::string& npcId, float x, float y) {
    CutsceneAction action;
    action.type = CutsceneActionType::MoveNPC;
    action.targetId = npcId;
    action.x = x;
    action.y = y;
    action.waitForCompletion = true;
    return action;
}

CutsceneAction CutsceneSystem::movePlayer(float x, float y) {
    CutsceneAction action;
    action.type = CutsceneActionType::MovePlayer;
    action.x = x;
    action.y = y;
    action.waitForCompletion = true;
    return action;
}

CutsceneAction CutsceneSystem::changeRoom(const std::string& roomId) {
    CutsceneAction action;
    action.type = CutsceneActionType::ChangeRoom;
    action.targetId = roomId;
    return action;
}

CutsceneAction CutsceneSystem::setFlag(const std::string& flag, bool value) {
    CutsceneAction action;
    action.type = CutsceneActionType::SetFlag;
    action.targetId = flag;
    return action;
}

CutsceneAction CutsceneSystem::runCallback(std::function<void()> cb) {
    CutsceneAction action;
    action.type = CutsceneActionType::Callback;
    action.callback = cb;
    return action;
}
