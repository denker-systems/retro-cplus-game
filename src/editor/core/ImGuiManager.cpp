/**
 * @file ImGuiManager.cpp
 * @brief ImGui integration implementation
 */
#ifdef HAS_IMGUI

#include "ImGuiManager.h"
#include <imgui.h>
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer2.h"
#include <iostream>

ImGuiManager& ImGuiManager::instance() {
    static ImGuiManager instance;
    return instance;
}

bool ImGuiManager::init(SDL_Window* window, SDL_Renderer* renderer) {
    if (m_initialized) return true;
    
    m_window = window;
    m_renderer = renderer;
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    // Enable docking
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
    // Setup style
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 4.0f;
    style.FrameRounding = 2.0f;
    style.ScrollbarRounding = 2.0f;
    
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    
    m_initialized = true;
    std::cout << "ImGuiManager initialized with docking support" << std::endl;
    return true;
}

void ImGuiManager::shutdown() {
    if (!m_initialized) return;
    
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    m_initialized = false;
    std::cout << "ImGuiManager shutdown" << std::endl;
}

void ImGuiManager::beginFrame() {
    if (!m_initialized) return;
    
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::endFrame() {
    if (!m_initialized) return;
    
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), m_renderer);
}

void ImGuiManager::processEvent(const SDL_Event& event) {
    if (!m_initialized) return;
    
    ImGui_ImplSDL2_ProcessEvent(&event);
}

#endif // HAS_IMGUI
