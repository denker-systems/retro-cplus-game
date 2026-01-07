/**
 * @file GameSettingsPanel.cpp
 * @brief Editor panel for game mode and physics settings
 */
#include "GameSettingsPanel.h"
#include "engine/data/GameSettings.h"
#include <imgui.h>

namespace editor {

GameSettingsPanel::GameSettingsPanel() = default;

void GameSettingsPanel::render() {
    if (!ImGui::Begin(m_title.c_str())) {
        ImGui::End();
        return;
    }
    
    renderGameModeSection();
    ImGui::Separator();
    renderPhysicsSection();
    
    ImGui::End();
}

void GameSettingsPanel::renderGameModeSection() {
    auto& settings = engine::GameSettings::instance();
    
    ImGui::Text("Game Mode");
    ImGui::Spacing();
    
    // Radio buttons for game mode
    int currentMode = static_cast<int>(settings.getGameMode());
    
    if (ImGui::RadioButton("Adventure (Point & Click)", &currentMode, 0)) {
        settings.setGameMode(engine::GameMode::Adventure);
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Classic adventure game style:\n- Point-and-click movement\n- Depth scaling (perspective)\n- No physics simulation");
    }
    
    if (ImGui::RadioButton("Platformer (Physics)", &currentMode, 1)) {
        settings.setGameMode(engine::GameMode::Platformer);
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Side-scrolling platformer style:\n- WASD/Arrow movement\n- Space to jump\n- Shift to run\n- Box2D physics");
    }
    
    ImGui::Spacing();
    
    // Show current mode
    ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "Current: %s", settings.getGameModeName());
}

void GameSettingsPanel::renderPhysicsSection() {
    auto& settings = engine::GameSettings::instance();
    
    ImGui::Text("Physics Settings");
    ImGui::Spacing();
    
    // Only show physics settings in platformer mode
    bool isPlatformer = settings.isPlatformerMode();
    
    if (!isPlatformer) {
        ImGui::TextDisabled("(Physics settings only available in Platformer mode)");
        return;
    }
    
    // Gravity
    float gravity = settings.getGravity();
    if (ImGui::SliderFloat("Gravity", &gravity, 100.0f, 2000.0f, "%.0f px/s²")) {
        settings.setGravity(gravity);
    }
    
    // Jump force
    float jumpForce = settings.getJumpForce();
    if (ImGui::SliderFloat("Jump Force", &jumpForce, 100.0f, 800.0f, "%.0f")) {
        settings.setJumpForce(jumpForce);
    }
    
    // Walk speed
    float walkSpeed = settings.getWalkSpeed();
    if (ImGui::SliderFloat("Walk Speed", &walkSpeed, 50.0f, 400.0f, "%.0f px/s")) {
        settings.setWalkSpeed(walkSpeed);
    }
    
    // Run speed
    float runSpeed = settings.getRunSpeed();
    if (ImGui::SliderFloat("Run Speed", &runSpeed, 100.0f, 600.0f, "%.0f px/s")) {
        settings.setRunSpeed(runSpeed);
    }
    
    ImGui::Spacing();
    
    // Debug draw toggle
    bool debugDraw = settings.isPhysicsDebugEnabled();
    if (ImGui::Checkbox("Show Physics Debug", &debugDraw)) {
        settings.setPhysicsDebug(debugDraw);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Draw collision shapes and physics bodies");
    }
}

} // namespace editor
