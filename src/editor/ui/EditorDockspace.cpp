/**
 * @file EditorDockspace.cpp
 * @brief Implementation of EditorDockspace
 */
#include "EditorDockspace.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#include <imgui_internal.h>
#endif

void EditorDockspace::render(float menuBarHeight, float screenW, float screenH) {
#ifdef HAS_IMGUI
    ImGuiWindowFlags dockspaceFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
    
    ImGui::SetNextWindowPos(ImVec2(0, menuBarHeight));
    ImGui::SetNextWindowSize(ImVec2(screenW, screenH - menuBarHeight));
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("DockSpaceWindow", nullptr, dockspaceFlags);
    ImGui::PopStyleVar();
    
    m_dockspaceId = ImGui::GetID("EditorDockspace");
    ImGui::DockSpace(m_dockspaceId, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);
    
    if (m_firstTime) {
        setupDefaultLayout(screenW, screenH, menuBarHeight);
        m_firstTime = false;
    }
    
    ImGui::End();
#endif
}

void EditorDockspace::setupDefaultLayout(float screenW, float screenH, float menuBarHeight) {
#ifdef HAS_IMGUI
    ImGui::DockBuilderRemoveNode(m_dockspaceId);
    ImGui::DockBuilderAddNode(m_dockspaceId, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(m_dockspaceId, ImVec2(screenW, screenH - menuBarHeight));
    
    ImGuiID dockLeft, dockRight, dockBottom, dockCenter;
    ImGui::DockBuilderSplitNode(m_dockspaceId, ImGuiDir_Left, 0.2f, &dockLeft, &dockCenter);
    ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Right, 0.3f, &dockRight, &dockCenter);
    ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Down, 0.25f, &dockBottom, &dockCenter);
    
    ImGuiID dockRightTop, dockRightBottom;
    ImGui::DockBuilderSplitNode(dockRight, ImGuiDir_Down, 0.5f, &dockRightBottom, &dockRightTop);
    
    ImGui::DockBuilderDockWindow("Hierarchy", dockLeft);
    ImGui::DockBuilderDockWindow("Viewport", dockCenter);
    ImGui::DockBuilderDockWindow("3D Viewport", dockCenter);
    ImGui::DockBuilderDockWindow("Dialog Graph", dockCenter);
    ImGui::DockBuilderDockWindow("Quest Graph", dockCenter);
    ImGui::DockBuilderDockWindow("NPC Behavior", dockCenter);
    ImGui::DockBuilderDockWindow("Scene Graph", dockCenter);
    ImGui::DockBuilderDockWindow("Layer Editor", dockCenter);
    ImGui::DockBuilderDockWindow("TileMap Editor", dockCenter);
    
    // Right side: Properties + AI Assistant (tabbed top), Asset Browser + Place Actors (tabbed bottom)
    ImGui::DockBuilderDockWindow("Properties", dockRightTop);
    ImGui::DockBuilderDockWindow("AI Assistant", dockRightTop);
    ImGui::DockBuilderDockWindow("Asset Browser", dockRightBottom);
    ImGui::DockBuilderDockWindow("Place Actors", dockRightBottom);
    
    ImGui::DockBuilderDockWindow("Console", dockBottom);
    ImGui::DockBuilderFinish(m_dockspaceId);
#endif
}
