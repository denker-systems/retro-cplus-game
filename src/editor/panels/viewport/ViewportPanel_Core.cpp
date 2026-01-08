/**
 * @file ViewportPanel_Core.cpp
 * @brief ViewportPanel core functionality - constructor, render, toolbar, breadcrumbs
 */
#include "ViewportPanel.h"
#include "editor/core/EditorContext.h"
#include "editor/viewport/Viewport3DPanel.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"
#include "engine/data/DataLoader.h"
#include <SDL_image.h>
#include <algorithm>
#include <iostream>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

ViewportPanel::ViewportPanel(EditorContext& context)
    : m_context(context) {
    // Create selection manager
    m_selectionManager = new SelectionManager(context);
}

ViewportPanel::~ViewportPanel() {
    if (m_roomTexture) {
        SDL_DestroyTexture(m_roomTexture);
    }
    delete m_selectionManager;
}

void ViewportPanel::update(float deltaTime) {
    // Load room preview if selection changed
    if (m_context.selectedRoomId != m_loadedRoomId) {
        loadRoomPreview();
    }
}

void ViewportPanel::loadRoomPreview() {
    if (m_roomTexture) {
        SDL_DestroyTexture(m_roomTexture);
        m_roomTexture = nullptr;
    }
    
    m_loadedRoomId = m_context.selectedRoomId;
    
    if (m_loadedRoomId.empty() || !m_renderer) return;
    
    // Find room data
    const auto& rooms = DataLoader::instance().getRooms();
    const RoomData* room = nullptr;
    for (const auto& r : rooms) {
        if (r.id == m_loadedRoomId) {
            room = &r;
            break;
        }
    }
    
    if (!room) return;
    
    // Load background texture
    std::string path = "assets/backgrounds/" + room->background;
    m_roomTexture = IMG_LoadTexture(m_renderer, path.c_str());
}

void ViewportPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        renderBreadcrumbs();
        ImGui::Separator();
        
        // Toolbar with 2D/3D toggle - shown on ALL levels
        renderToolbar();
        
        // Render different views based on breadcrumb level and mode
        if (!m_level) {
            // World level - show all levels
            if (m_viewportMode == ViewportMode::Mode3D) {
                renderWorld3D();
            } else {
                renderWorldView();
            }
        } else if (!m_scene) {
            // Level level - show all scenes
            if (m_viewportMode == ViewportMode::Mode3D) {
                renderLevel3D();
            } else {
                renderLevelView();
            }
        } else {
            // Scene level - show scene content
            if (m_viewportMode == ViewportMode::Mode3D) {
                if (!m_viewport3D) {
                    m_viewport3D = std::make_unique<editor::Viewport3DPanel>();
                    m_viewport3D->setSelectionManager(m_selectionManager);
                }
                // Configure for Scene view
                m_viewport3D->setViewLevel(editor::View3DLevel::Scene);
                m_viewport3D->setScene(m_scene);
                m_viewport3D->render();
            } else {
                renderSceneView();
            }
        }
    }
    ImGui::End();
#endif
}

void ViewportPanel::renderToolbar() {
#ifdef HAS_IMGUI
    // 2D/3D Mode Toggle - prominent at start
    ImGui::PushStyleColor(ImGuiCol_Button, m_viewportMode == ViewportMode::Mode2D ? 
        ImVec4(0.2f, 0.5f, 0.8f, 1.0f) : ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    if (ImGui::Button("2D")) {
        m_viewportMode = ViewportMode::Mode2D;
    }
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    
    ImGui::PushStyleColor(ImGuiCol_Button, m_viewportMode == ViewportMode::Mode3D ? 
        ImVec4(0.2f, 0.5f, 0.8f, 1.0f) : ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    if (ImGui::Button("3D")) {
        m_viewportMode = ViewportMode::Mode3D;
    }
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    ImGui::Text("|");
    ImGui::SameLine();
    
    // Tool buttons
    m_toolManager.renderToolbar();
    ImGui::SameLine();
    ImGui::Text("|");
    ImGui::SameLine();
    
    // Zoom controls
    ImGui::Text("Zoom: %.0f%%", m_zoom * 100);
    ImGui::SameLine();
    if (ImGui::Button("-")) m_zoom = std::max(0.25f, m_zoom - 0.25f);
    ImGui::SameLine();
    if (ImGui::Button("+")) m_zoom = std::min(4.0f, m_zoom + 0.25f);
    ImGui::SameLine();
    if (ImGui::Button("100%")) { m_zoom = 1.0f; m_panX = 0; m_panY = 0; }
    
    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();
    
    // View toggles
    ImGui::Checkbox("Grid", &m_showGrid);
    ImGui::SameLine();
    ImGui::Checkbox("Hotspots", &m_showHotspots);
    ImGui::SameLine();
    ImGui::Checkbox("Walk Area", &m_showWalkArea);
    ImGui::SameLine();
    ImGui::Checkbox("Physics", &m_showPhysicsDebug);
#endif
}

void ViewportPanel::renderBreadcrumbs() {
#ifdef HAS_IMGUI
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.4f, 0.5f, 1.0f));
    
    // World level
    if (m_world) {
        if (ImGui::Button("🌍 World")) {
            std::cout << "[ViewportPanel] Breadcrumb click: World" << std::endl;
            setLevel(nullptr);
        }
        
        // Level level
        if (m_level) {
            ImGui::SameLine();
            ImGui::TextDisabled(">");
            ImGui::SameLine();
            
            if (ImGui::Button(("📁 " + m_level->getName()).c_str())) {
                std::cout << "[ViewportPanel] Breadcrumb click: Level " << m_level->getName() << std::endl;
                setScene(nullptr);
            }
            
            // Scene level
            if (m_scene) {
                ImGui::SameLine();
                ImGui::TextDisabled(">");
                ImGui::SameLine();
                
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.3f, 1.0f));
                ImGui::Button(("🎬 " + m_scene->getName()).c_str());
                ImGui::PopStyleColor();
            }
        }
    } else {
        ImGui::TextDisabled("No World loaded");
    }
    
    ImGui::PopStyleColor(2);
#endif
}

void ViewportPanel::setWorld(engine::World* world) {
    std::cout << "[ViewportPanel] setWorld: " << (world ? world->getName() : "null") << std::endl;
    m_world = world;
    if (m_selectionManager) {
        m_selectionManager->setWorld(world);
    }
}

void ViewportPanel::setLevel(engine::Level* level) {
    std::cout << "[ViewportPanel] setLevel: " << (level ? level->getName() : "null") << std::endl;
    m_level = level;
    m_scene = nullptr;
    if (m_selectionManager) {
        m_selectionManager->setActiveLevel(level);
        m_selectionManager->setActiveScene(nullptr);
    }
}

void ViewportPanel::setScene(engine::Scene* scene) {
    std::cout << "[ViewportPanel] setScene: " << (scene ? scene->getName() : "null") << std::endl;
    m_scene = scene;
    if (m_selectionManager) {
        m_selectionManager->setActiveScene(scene);
    }
}

void ViewportPanel::syncFromSelectionManager() {
    if (!m_selectionManager) return;
    
    std::cout << "[ViewportPanel] syncFromSelectionManager - Level: " 
              << (m_selectionManager->getActiveLevel() ? m_selectionManager->getActiveLevel()->getName() : "null")
              << ", Scene: " << (m_selectionManager->getActiveScene() ? m_selectionManager->getActiveScene()->getName() : "null") 
              << std::endl;
    
    m_world = m_selectionManager->getWorld();
    m_level = m_selectionManager->getActiveLevel();
    m_scene = m_selectionManager->getActiveScene();
    m_selectedActor = m_selectionManager->getSelectedActor();
}
