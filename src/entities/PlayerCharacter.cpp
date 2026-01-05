/**
 * @file PlayerCharacter.cpp
 * @brief Implementation av spelarkaraktär
 */
#include "PlayerCharacter.h"
#include "../graphics/SpriteSheet.h"
#include "../graphics/Animation.h"
#include <iostream>

PlayerCharacter::PlayerCharacter(float x, float y)
    : Character(x, y, 32, 48, "Player")
    , m_walkMinX(0)
    , m_walkMaxX(640)
    , m_walkMinY(0)
    , m_walkMaxY(400) {}

PlayerCharacter::~PlayerCharacter() = default;

void PlayerCharacter::setWalkArea(int minX, int maxX, int minY, int maxY) {
    m_walkMinX = minX;
    m_walkMaxX = maxX;
    m_walkMinY = minY;
    m_walkMaxY = maxY;
}

void PlayerCharacter::clampToWalkArea() {
    // Använd rummets walk area
    if (m_x < m_walkMinX) m_x = static_cast<float>(m_walkMinX);
    if (m_x > m_walkMaxX - m_width) m_x = static_cast<float>(m_walkMaxX - m_width);
    if (m_y < m_walkMinY) m_y = static_cast<float>(m_walkMinY);
    if (m_y > m_walkMaxY - m_height) m_y = static_cast<float>(m_walkMaxY - m_height);
}

bool PlayerCharacter::loadSprite(SDL_Renderer* renderer, const std::string& path) {
    m_spriteSheet = std::make_unique<SpriteSheet>();
    if (!m_spriteSheet->load(path, 32, 48)) {
        std::cerr << "Failed to load player sprite: " << path << std::endl;
        m_spriteSheet.reset();
        return false;
    }
    
    // Skapa animationer (anta 4 frames walk, 1 frame idle)
    m_walkAnim = std::make_unique<Animation>(m_spriteSheet.get(), 0, 3, 0.15f, true);
    m_idleAnim = std::make_unique<Animation>(m_spriteSheet.get(), 0, 0, 1.0f, true);
    
    std::cout << "Loaded player sprite: " << path << std::endl;
    return true;
}

void PlayerCharacter::move(int dx, int dy, float deltaTime) {
    if (dx != 0 || dy != 0) {
        m_hasTarget = false;
        m_moving = true;
        
        if (dx != 0) {
            m_facingRight = (dx > 0);
        }
        
        m_x += dx * m_speed * deltaTime;
        m_y += dy * m_speed * deltaTime;
        clampToWalkArea();
    }
}

void PlayerCharacter::update(float deltaTime) {
    updateMovement(deltaTime);
    updateAnimation(deltaTime);
}

void PlayerCharacter::updateAnimation(float deltaTime) {
    // Uppdatera Animation-objekt om de finns
    if (m_walkAnim && m_moving) {
        m_walkAnim->update(deltaTime);
    }
    if (m_idleAnim && !m_moving) {
        m_idleAnim->update(deltaTime);
    }
    
    // Fallback animation för placeholder
    if (m_moving) {
        m_animTimer += deltaTime;
        if (m_animTimer >= m_animSpeed) {
            m_animTimer = 0;
            m_currentFrame = (m_currentFrame + 1) % 4;
        }
    } else {
        m_currentFrame = 0;
        m_animTimer = 0;
    }
}

void PlayerCharacter::render(SDL_Renderer* renderer) {
    renderScaled(renderer, 1.0f);
}

void PlayerCharacter::renderScaled(SDL_Renderer* renderer, float scale) {
    int scaledW = static_cast<int>(m_width * scale);
    int scaledH = static_cast<int>(m_height * scale);
    
    // Justera Y så att fötterna stannar på samma plats
    int drawX = static_cast<int>(m_x) - (scaledW - m_width) / 2;
    int drawY = static_cast<int>(m_y) - (scaledH - m_height);
    
    if (m_spriteSheet && m_spriteSheet->isLoaded()) {
        Animation* currentAnim = m_moving ? m_walkAnim.get() : m_idleAnim.get();
        if (currentAnim) {
            currentAnim->renderScaled(renderer, drawX, drawY, scaledW, scaledH, !m_facingRight);
        }
    } else {
        // Placeholder med skalning
        SDL_Rect rect = {drawX, drawY, scaledW, scaledH};
        
        // Kropp
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderFillRect(renderer, &rect);
        
        // Huvud (skalad)
        int headW = static_cast<int>(16 * scale);
        int headH = static_cast<int>(14 * scale);
        SDL_Rect head = {
            drawX + static_cast<int>(8 * scale),
            drawY + static_cast<int>(2 * scale),
            headW, headH
        };
        SDL_SetRenderDrawColor(renderer, 240, 200, 160, 255);
        SDL_RenderFillRect(renderer, &head);
        
        // Byxor (skalad)
        SDL_Rect pants = {
            drawX + static_cast<int>(4 * scale),
            drawY + static_cast<int>(24 * scale),
            static_cast<int>(24 * scale),
            static_cast<int>(16 * scale)
        };
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
        SDL_RenderFillRect(renderer, &pants);
    }
}

void PlayerCharacter::renderPlaceholder(SDL_Renderer* renderer) {
    // TODO: Ersätt med SpriteSheet
    SDL_Rect rect = {
        static_cast<int>(m_x),
        static_cast<int>(m_y),
        m_width,
        m_height
    };

    // Kropp
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &rect);

    // Huvud
    SDL_Rect head = {
        static_cast<int>(m_x) + 8,
        static_cast<int>(m_y) + 2,
        16, 14
    };
    SDL_SetRenderDrawColor(renderer, 240, 200, 160, 255);
    SDL_RenderFillRect(renderer, &head);

    // Byxor
    SDL_Rect pants = {
        static_cast<int>(m_x) + 4,
        static_cast<int>(m_y) + 24,
        24, 16
    };
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderFillRect(renderer, &pants);
}
