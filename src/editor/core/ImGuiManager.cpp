/**
 * @file ImGuiManager.cpp
 * @brief ImGui integration implementation
 */
#ifdef HAS_IMGUI

#include "ImGuiManager.h"
#include "engine/graphics/GLContext.h"
#include "editor/ui/EditorTheme.h"
#include <imgui.h>
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer2.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <iostream>

ImGuiManager& ImGuiManager::instance() {
    static ImGuiManager instance;
    return instance;
}

bool ImGuiManager::init(SDL_Window* window, SDL_Renderer* renderer) {
    if (m_initialized) return true;
    
    m_window = window;
    m_renderer = renderer;
    
    // Check if window has OpenGL context
    Uint32 windowFlags = SDL_GetWindowFlags(window);
    m_useOpenGL = (windowFlags & SDL_WINDOW_OPENGL) != 0;
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    // Enable docking
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
    // Load fonts and apply theme
    editor::EditorTheme::LoadFonts();
    editor::EditorTheme::Apply(editor::EditorTheme::Theme::RetroEngine);
    
    // Setup Platform/Renderer backends
    if (m_useOpenGL) {
        // Initialize OpenGL context
        m_glContext = new engine::GLContext();
        if (!m_glContext->initialize(window)) {
            std::cerr << "Failed to initialize OpenGL context, falling back to SDL renderer" << std::endl;
            delete m_glContext;
            m_glContext = nullptr;
            m_useOpenGL = false;
        }
    }
    
    if (m_useOpenGL && m_glContext) {
        ImGui_ImplSDL2_InitForOpenGL(window, m_glContext->getContext());
        ImGui_ImplOpenGL3_Init("#version 330");
        std::cout << "ImGuiManager initialized with OpenGL3 backend" << std::endl;
    } else {
        ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
        ImGui_ImplSDLRenderer2_Init(renderer);
        std::cout << "ImGuiManager initialized with SDL_Renderer backend" << std::endl;
    }
    
    m_initialized = true;
    return true;
}

void ImGuiManager::shutdown() {
    if (!m_initialized) return;
    
    if (m_useOpenGL) {
        ImGui_ImplOpenGL3_Shutdown();
    } else {
        ImGui_ImplSDLRenderer2_Shutdown();
    }
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    if (m_glContext) {
        m_glContext->shutdown();
        delete m_glContext;
        m_glContext = nullptr;
    }
    
    m_initialized = false;
    std::cout << "ImGuiManager shutdown" << std::endl;
}

void ImGuiManager::beginFrame() {
    if (!m_initialized) return;
    
    if (m_useOpenGL) {
        ImGui_ImplOpenGL3_NewFrame();
    } else {
        ImGui_ImplSDLRenderer2_NewFrame();
    }
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::endFrame() {
    if (!m_initialized) return;
    
    ImGui::Render();
    if (m_useOpenGL) {
        // Clear and render with OpenGL
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        m_glContext->swapBuffers();
    } else {
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), m_renderer);
    }
}

void ImGuiManager::processEvent(const SDL_Event& event) {
    if (!m_initialized) return;
    
    ImGui_ImplSDL2_ProcessEvent(&event);
}

#endif // HAS_IMGUI
