/**
 * @file DialogState.cpp
 * @brief Implementation av dialog overlay state
 */
#include "DialogState.h"
#include "engine/systems/DialogSystem.h"
#include "engine/systems/ConditionSystem.h"
#include "engine/graphics/FontManager.h"
#include "engine/audio/AudioManager.h"
#include "game/Game.h"
#include <iostream>

DialogState::DialogState() {
    std::cout << "DialogState created" << std::endl;
}

void DialogState::enter() {
    std::cout << "DialogState::enter()" << std::endl;
    m_selectedChoice = 0;
    m_textRevealTimer = 0.0f;
    m_revealedChars = 0;
    m_textFullyRevealed = false;
    m_skipRequested = false;
}

void DialogState::exit() {
    std::cout << "DialogState::exit()" << std::endl;
}

void DialogState::update(float deltaTime) {
    // Avsluta om dialogen är klar
    if (!DialogSystem::instance().isActive()) {
        if (m_game) {
            m_game->popState();
        }
        return;
    }
    
    // Text reveal animation (snabbare om skipRequested)
    const std::string& text = DialogSystem::instance().getCurrentText();
    float revealSpeed = m_skipRequested ? 0.01f : 0.03f;  // Sekunder per tecken
    
    if (!m_textFullyRevealed && m_revealedChars < static_cast<int>(text.length())) {
        m_textRevealTimer += deltaTime;
        
        while (m_textRevealTimer >= revealSpeed && m_revealedChars < static_cast<int>(text.length())) {
            m_textRevealTimer -= revealSpeed;
            m_revealedChars++;
        }
        
        if (m_revealedChars >= static_cast<int>(text.length())) {
            m_textFullyRevealed = true;
        }
    }
}

void DialogState::render(SDL_Renderer* renderer) {
    // Semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
    SDL_Rect overlay = {0, 0, 640, 400};
    SDL_RenderFillRect(renderer, &overlay);
    
    renderDialogBox(renderer);
    renderChoices(renderer);
}

void DialogState::renderDialogBox(SDL_Renderer* renderer) {
    // Dialog box längst ner
    SDL_Rect dialogBox = {20, 280, 600, 90};
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 240);
    SDL_RenderFillRect(renderer, &dialogBox);
    
    // Ram
    SDL_SetRenderDrawColor(renderer, 100, 100, 180, 255);
    SDL_RenderDrawRect(renderer, &dialogBox);
    
    // Speaker namn
    const std::string& speaker = DialogSystem::instance().getCurrentSpeaker();
    if (!speaker.empty()) {
        SDL_Rect nameBox = {30, 268, 100, 20};
        SDL_SetRenderDrawColor(renderer, 40, 40, 80, 255);
        SDL_RenderFillRect(renderer, &nameBox);
        SDL_SetRenderDrawColor(renderer, 100, 100, 180, 255);
        SDL_RenderDrawRect(renderer, &nameBox);
        
        FontManager::instance().renderText(renderer, "default", speaker, 35, 270, {255, 220, 100, 255});
    }
    
    // Dialog text (visa bara revealed chars)
    const std::string& text = DialogSystem::instance().getCurrentText();
    std::string displayText = text.substr(0, m_revealedChars);
    FontManager::instance().renderText(renderer, "default", displayText, 35, 295, {255, 255, 255, 255});
}

void DialogState::renderChoices(SDL_Renderer* renderer) {
    const auto& choices = DialogSystem::instance().getCurrentChoices();
    if (choices.empty()) {
        // Visa "Tryck SPACE för att fortsätta"
        FontManager::instance().renderText(renderer, "default", 
            "[SPACE to continue]", 450, 355, {150, 150, 180, 255});
        return;
    }
    
    // Rita valmöjligheter
    int y = 320;
    for (size_t i = 0; i < choices.size(); i++) {
        bool selected = (static_cast<int>(i) == m_selectedChoice);
        
        if (selected) {
            SDL_Rect highlight = {30, y - 2, 580, 18};
            SDL_SetRenderDrawColor(renderer, 80, 80, 120, 200);
            SDL_RenderFillRect(renderer, &highlight);
        }
        
        std::string prefix = selected ? "> " : "  ";
        SDL_Color color = selected ? SDL_Color{255, 255, 100, 255} : SDL_Color{200, 200, 220, 255};
        
        // Visa preview om det finns, annars full text
        std::string displayText = choices[i].preview.empty() ? choices[i].text : choices[i].preview;
        
        // Lägg till ton-ikon
        std::string toneIcon = "";
        if (!choices[i].tone.empty()) {
            if (choices[i].tone == "friendly") toneIcon = "[😊] ";
            else if (choices[i].tone == "aggressive") toneIcon = "[🔥] ";
            else if (choices[i].tone == "sarcastic") toneIcon = "[😏] ";
            else if (choices[i].tone == "neutral") toneIcon = "[💬] ";
        }
        
        FontManager::instance().renderText(renderer, "default", prefix + toneIcon + displayText, 35, y, color);
        
        y += 20;
    }
}

void DialogState::handleEvent(const SDL_Event& event) {
    if (event.type != SDL_KEYDOWN) return;
    
    const auto& choices = DialogSystem::instance().getCurrentChoices();
    
    if (choices.empty()) {
        // Ingen val - SPACE fortsätter eller skippar text
        if (event.key.keysym.scancode == SDL_SCANCODE_SPACE ||
            event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
            
            if (!m_textFullyRevealed) {
                // Skip till slutet av texten
                m_skipRequested = true;
            } else {
                // Fortsätt till nästa nod
                DialogSystem::instance().advance();
                // Återställ för nästa text
                m_textRevealTimer = 0.0f;
                m_revealedChars = 0;
                m_textFullyRevealed = false;
                m_skipRequested = false;
            }
        }
    } else {
        // Navigera val
        switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_UP:
            case SDL_SCANCODE_W:
                m_selectedChoice = (m_selectedChoice - 1 + choices.size()) % choices.size();
                AudioManager::instance().playSound("navigate");
                break;
                
            case SDL_SCANCODE_DOWN:
            case SDL_SCANCODE_S:
                m_selectedChoice = (m_selectedChoice + 1) % choices.size();
                AudioManager::instance().playSound("navigate");
                break;
                
            case SDL_SCANCODE_SPACE:
            case SDL_SCANCODE_RETURN:
                {
                    AudioManager::instance().playSound("select");
                    
                    // Kör actions för valt val
                    if (m_selectedChoice < static_cast<int>(choices.size())) {
                        const auto& selectedChoice = choices[m_selectedChoice];
                        if (!selectedChoice.actions.empty()) {
                            ActionExecutor::instance().executeAll(selectedChoice.actions);
                        }
                    }
                    
                    DialogSystem::instance().selectChoice(m_selectedChoice);
                    m_selectedChoice = 0;
                    
                    // Återställ text reveal för nästa nod
                    m_textRevealTimer = 0.0f;
                    m_revealedChars = 0;
                    m_textFullyRevealed = false;
                    m_skipRequested = false;
                }
                break;
                
            default:
                break;
        }
    }
}
