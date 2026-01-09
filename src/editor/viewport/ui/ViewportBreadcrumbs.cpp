/**
 * @file ViewportBreadcrumbs.cpp
 * @brief Implementation of navigation breadcrumbs
 */
#include "ViewportBreadcrumbs.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"

#ifdef HAS_IMGUI
#include <imgui.h>

namespace editor {
namespace viewport {

bool ViewportBreadcrumbs::render(engine::World* world, engine::Level* level, engine::Scene* scene) {
    m_navigateToWorld = false;
    m_navigateToLevel = false;
    
    if (!world) {
        ImGui::Text("No world loaded");
        return false;
    }
    
    // World button
    if (ImGui::SmallButton(("World: " + world->getName()).c_str())) {
        m_navigateToWorld = true;
    }
    
    // Level button
    if (level) {
        ImGui::SameLine();
        ImGui::Text(">");
        ImGui::SameLine();
        if (ImGui::SmallButton(level->getName().c_str())) {
            m_navigateToLevel = true;
        }
    }
    
    // Scene (non-clickable)
    if (scene) {
        ImGui::SameLine();
        ImGui::Text(">");
        ImGui::SameLine();
        ImGui::Text("%s", scene->getName().c_str());
    }
    
    return m_navigateToWorld || m_navigateToLevel;
}

void ViewportBreadcrumbs::resetNavigation() {
    m_navigateToWorld = false;
    m_navigateToLevel = false;
}

} // namespace viewport
} // namespace editor

#endif // HAS_IMGUI
