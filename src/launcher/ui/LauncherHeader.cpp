/**
 * @file LauncherHeader.cpp
 * @brief Launcher header implementation
 */
#include "LauncherHeader.h"
#include "editor/ui/EditorTheme.h"
#include <SDL_image.h>
#include <iostream>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

namespace launcher {

LauncherHeader::LauncherHeader() {
}

LauncherHeader::~LauncherHeader() {
    if (m_logoTexture) {
        SDL_DestroyTexture(m_logoTexture);
        m_logoTexture = nullptr;
    }
}

bool LauncherHeader::initialize(SDL_Renderer* renderer) {
    m_renderer = renderer;
    
    std::cout << "[LauncherHeader] Initializing..." << std::endl;
    
    // Try multiple paths for logo
    const char* logoPaths[] = {
        "retroengine.png",
        "src/launcher/retroengine.png",
        "../src/launcher/retroengine.png",
        "assets/launcher/retroengine.png",
        nullptr
    };
    
    for (int i = 0; logoPaths[i] != nullptr; i++) {
        std::cout << "[LauncherHeader] Trying to load logo from: " << logoPaths[i] << std::endl;
        m_logoTexture = IMG_LoadTexture(renderer, logoPaths[i]);
        if (m_logoTexture) {
            std::cout << "[LauncherHeader] ✓ Logo loaded successfully from: " << logoPaths[i] << std::endl;
            
            int w, h;
            SDL_QueryTexture(m_logoTexture, nullptr, nullptr, &w, &h);
            std::cout << "[LauncherHeader] Logo size: " << w << "x" << h << std::endl;
            break;
        } else {
            std::cout << "[LauncherHeader] ✗ Failed: " << IMG_GetError() << std::endl;
        }
    }
    
    if (!m_logoTexture) {
        std::cout << "[LauncherHeader] WARNING: Logo not found, using text fallback" << std::endl;
    }
    
    return true;
}

int LauncherHeader::render(float screenWidth) {
#ifdef HAS_IMGUI
    int action = 0;
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(screenWidth, getHeight()));
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;
    
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.09f, 1.0f));
    ImGui::Begin("Header", nullptr, flags);
    
    // Logo
    ImGui::SetCursorPosY(20);
    ImGui::SetCursorPosX(50);
    
    if (m_logoTexture) {
        int texW, texH;
        SDL_QueryTexture(m_logoTexture, nullptr, nullptr, &texW, &texH);
        
        float scale = 110.0f / texH;
        float displayW = texW * scale;
        float displayH = texH * scale;
        
        ImGui::Image(reinterpret_cast<ImTextureID>(m_logoTexture), ImVec2(displayW, displayH));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, editor::EditorTheme::BrandColors::Primary);
        ImGui::SetWindowFontScale(2.0f);
        ImGui::Text("RETRO ENGINE");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();
    }
    
    // Buttons
    ImGui::SameLine(screenWidth - 250);
    if (ImGui::Button("New Project", ImVec2(120, 40))) {
        action = 1;
    }
    
    ImGui::SameLine();
    if (ImGui::Button("About", ImVec2(80, 40))) {
        action = 2;
    }
    
    ImGui::End();
    ImGui::PopStyleColor();
    
    return action;
#else
    return 0;
#endif
}

} // namespace launcher
