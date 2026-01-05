#pragma once

#include <SDL.h>

class Player {
public:
    Player(float x, float y);

    void move(int dx, int dy, float deltaTime);
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);

    float getX() const { return m_x; }
    float getY() const { return m_y; }

private:
    float m_x, m_y;
    float m_speed = 200.0f;
    int m_width = 32;
    int m_height = 48;
    
    // Animation
    int m_currentFrame = 0;
    float m_animTimer = 0.0f;
    float m_animSpeed = 0.1f;
    bool m_moving = false;
    bool m_facingRight = true;
};
