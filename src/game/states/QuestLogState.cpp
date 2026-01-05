/**
 * @file QuestLogState.cpp
 * @brief Implementation av Quest Log UI
 */
#include "QuestLogState.h"
#include "game/Game.h"
#include "engine/systems/QuestSystem.h"
#include "engine/graphics/FontManager.h"
#include "engine/audio/AudioManager.h"
#include <iostream>

QuestLogState::QuestLogState() {
    std::cout << "QuestLogState created" << std::endl;
}

void QuestLogState::enter() {
    std::cout << "QuestLogState::enter()" << std::endl;
    m_selectedQuest = 0;
}

void QuestLogState::exit() {
    std::cout << "QuestLogState::exit()" << std::endl;
}

void QuestLogState::update(float deltaTime) {
    (void)deltaTime;
}

void QuestLogState::render(SDL_Renderer* renderer) {
    // Semi-transparent bakgrund
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect overlay = {0, 0, 640, 400};
    SDL_RenderFillRect(renderer, &overlay);
    
    // Quest log panel
    SDL_Rect panel = {80, 40, 480, 320};
    SDL_SetRenderDrawColor(renderer, 35, 40, 50, 250);
    SDL_RenderFillRect(renderer, &panel);
    
    // Ram
    SDL_SetRenderDrawColor(renderer, 100, 120, 140, 255);
    SDL_RenderDrawRect(renderer, &panel);
    
    // Titel
    FontManager::instance().renderTextCentered(renderer, "title",
        "QUEST LOG", 320, 55, {140, 160, 200, 255});
    
    auto quests = QuestSystem::instance().getAllQuests();
    
    if (quests.empty()) {
        FontManager::instance().renderTextCentered(renderer, "default",
            "No active quests", 320, 180, {120, 120, 140, 255});
    } else {
        int y = 100;
        int index = 0;
        
        for (const Quest* quest : quests) {
            bool selected = (index == m_selectedQuest);
            
            // Quest titel
            SDL_Color titleColor;
            std::string statusIcon;
            
            switch (quest->status) {
                case QuestStatus::Active:
                    titleColor = {200, 200, 100, 255};
                    statusIcon = "[*] ";
                    break;
                case QuestStatus::Completed:
                    titleColor = {100, 200, 100, 255};
                    statusIcon = "[v] ";
                    break;
                case QuestStatus::Failed:
                    titleColor = {200, 100, 100, 255};
                    statusIcon = "[x] ";
                    break;
                default:
                    titleColor = {150, 150, 150, 255};
                    statusIcon = "[ ] ";
            }
            
            if (selected) {
                // Highlight bakgrund
                SDL_Rect highlight = {100, y - 2, 440, 22};
                SDL_SetRenderDrawColor(renderer, 60, 70, 90, 255);
                SDL_RenderFillRect(renderer, &highlight);
            }
            
            FontManager::instance().renderText(renderer, "default",
                statusIcon + quest->title, 110, y, titleColor);
            
            // Visa objectives om vald
            if (selected && quest->status == QuestStatus::Active) {
                y += 25;
                for (const auto& obj : quest->objectives) {
                    std::string objText = "  - " + obj.description;
                    if (obj.requiredCount > 1) {
                        objText += " (" + std::to_string(obj.currentCount) + "/" 
                                 + std::to_string(obj.requiredCount) + ")";
                    }
                    
                    SDL_Color objColor = (obj.status == QuestStatus::Completed) 
                        ? SDL_Color{100, 180, 100, 255} 
                        : SDL_Color{160, 160, 180, 255};
                    
                    FontManager::instance().renderText(renderer, "default",
                        objText, 120, y, objColor);
                    y += 18;
                }
            }
            
            y += 28;
            index++;
        }
    }
    
    // Instruktioner
    FontManager::instance().renderTextCentered(renderer, "default",
        "UP/DOWN to select, ESC/J to close", 320, 345, {100, 100, 120, 255});
}

void QuestLogState::handleEvent(const SDL_Event& event) {
    if (event.type != SDL_KEYDOWN) return;
    
    auto quests = QuestSystem::instance().getAllQuests();
    int questCount = static_cast<int>(quests.size());
    
    switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_UP:
        case SDL_SCANCODE_W:
            if (m_selectedQuest > 0) {
                m_selectedQuest--;
                AudioManager::instance().playSound("navigate");
            }
            break;
            
        case SDL_SCANCODE_DOWN:
        case SDL_SCANCODE_S:
            if (m_selectedQuest < questCount - 1) {
                m_selectedQuest++;
                AudioManager::instance().playSound("navigate");
            }
            break;
            
        case SDL_SCANCODE_ESCAPE:
        case SDL_SCANCODE_J:
            AudioManager::instance().playSound("select");
            if (m_game) {
                m_game->popState();
            }
            break;
            
        default:
            break;
    }
}
