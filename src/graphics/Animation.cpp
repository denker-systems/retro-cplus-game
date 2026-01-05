/**
 * @file Animation.cpp
 * @brief Implementation av tidsbaserad sprite-animation
 */
#include "Animation.h"
#include "SpriteSheet.h"

Animation::Animation(SpriteSheet* sheet, int startFrame, int endFrame, float frameTime, bool loop)
    : m_sheet(sheet)
    , m_startFrame(startFrame)
    , m_endFrame(endFrame)
    , m_currentFrame(startFrame)
    , m_frameTime(frameTime)
    , m_loop(loop)
{
}

void Animation::init(SpriteSheet* sheet, int startFrame, int endFrame, float frameTime, bool loop) {
    m_sheet = sheet;
    m_startFrame = startFrame;
    m_endFrame = endFrame;
    m_currentFrame = startFrame;
    m_frameTime = frameTime;
    m_loop = loop;
    m_timer = 0.0f;
    m_playing = true;
    m_finished = false;
}

void Animation::update(float deltaTime) {
    if (!m_playing || m_finished || !m_sheet) return;

    // Ackumulera tid sedan senaste frame-byte
    m_timer += deltaTime;
    
    // Byt frame när tillräckligt med tid passerat
    if (m_timer >= m_frameTime) {
        m_timer -= m_frameTime;
        m_currentFrame++;
        
        // Hantera animation-slut
        if (m_currentFrame > m_endFrame) {
            if (m_loop) {
                m_currentFrame = m_startFrame;  // Börja om
            } else {
                m_currentFrame = m_endFrame;
                m_finished = true;  // Markera som klar
            }
        }
    }
}

void Animation::render(SDL_Renderer* renderer, int x, int y, bool flipH) {
    if (!m_sheet) return;
    m_sheet->render(renderer, m_currentFrame, x, y, flipH);
}

void Animation::renderScaled(SDL_Renderer* renderer, int x, int y, float scale, bool flipH) {
    if (!m_sheet) return;
    m_sheet->renderScaled(renderer, m_currentFrame, x, y, scale, flipH);
}

void Animation::renderScaled(SDL_Renderer* renderer, int x, int y, int w, int h, bool flipH) {
    if (!m_sheet) return;
    m_sheet->renderScaled(renderer, m_currentFrame, x, y, w, h, flipH);
}

void Animation::play() {
    m_playing = true;
}

void Animation::pause() {
    m_playing = false;
}

void Animation::reset() {
    m_currentFrame = m_startFrame;
    m_timer = 0.0f;
    m_finished = false;
    m_playing = true;
}

void Animation::setFrame(int frame) {
    if (frame >= m_startFrame && frame <= m_endFrame) {
        m_currentFrame = frame;
    }
}
