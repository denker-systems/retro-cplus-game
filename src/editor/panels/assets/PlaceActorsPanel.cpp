/**
 * @file PlaceActorsPanel.cpp
 * @brief Place Actors panel implementation
 */
#include "PlaceActorsPanel.h"
#include "editor/core/EditorContext.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

PlaceActorsPanel::PlaceActorsPanel(EditorContext& context)
    : m_context(context) {
    initializeTemplates();
}

void PlaceActorsPanel::initializeTemplates() {
    // Basic actors
    m_templates.push_back({"Empty Actor", "◯", ActorCategory::Basic, "Empty actor with no components", "", false});
    m_templates.push_back({"Sprite Actor", "🖼", ActorCategory::Basic, "Actor with sprite component", "", true});
    m_templates.push_back({"Hotspot", "🔲", ActorCategory::Basic, "Interactive hotspot area", "", false});
    
    // Characters
    m_templates.push_back({"NPC", "👤", ActorCategory::Characters, "Non-player character", "", true});
    m_templates.push_back({"Player Spawn", "⚑", ActorCategory::Characters, "Player spawn point", "", false});
    m_templates.push_back({"Enemy", "👾", ActorCategory::Characters, "Enemy character", "", true});
    
    // Props
    m_templates.push_back({"Static Prop", "📦", ActorCategory::Props, "Non-interactive decoration", "", true});
    m_templates.push_back({"Interactive Prop", "🎁", ActorCategory::Props, "Clickable/usable object", "", true});
    m_templates.push_back({"Door", "🚪", ActorCategory::Props, "Door/exit to another scene", "", false});
    m_templates.push_back({"Container", "📥", ActorCategory::Props, "Container with items", "", true});
    
    // 3D Physics Props
    m_templates.push_back({"Static Mesh", "🧊", ActorCategory::Props, "3D mesh with optional physics", "StaticMeshActor", true});
    m_templates.push_back({"Physics Cube", "⬜", ActorCategory::Props, "Falling physics cube", "StaticMeshActor:Cube:Dynamic", true});
    m_templates.push_back({"Physics Sphere", "⚪", ActorCategory::Props, "Rolling physics sphere", "StaticMeshActor:Sphere:Dynamic", true});
    m_templates.push_back({"Static Platform", "▬", ActorCategory::Props, "Static physics platform", "StaticMeshActor:Cube:Static", true});
    
    // Triggers
    m_templates.push_back({"Trigger Zone", "⬜", ActorCategory::Triggers, "Invisible trigger area", "", false});
    m_templates.push_back({"Walk Area", "🟩", ActorCategory::Triggers, "Walkable area bounds", "", false});
    m_templates.push_back({"Cutscene Trigger", "🎬", ActorCategory::Triggers, "Triggers cutscene", "", false});
    
    // Lights
    m_templates.push_back({"Point Light", "💡", ActorCategory::Lights, "Point light source", "", false});
    m_templates.push_back({"Ambient Light", "☀", ActorCategory::Lights, "Ambient lighting", "", false});
    
    // Audio
    m_templates.push_back({"Sound Emitter", "🔊", ActorCategory::Audio, "Plays sound effects", "", false});
    m_templates.push_back({"Music Zone", "🎵", ActorCategory::Audio, "Changes background music", "", false});
}

void PlaceActorsPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        // Search bar
        ImGui::SetNextItemWidth(-1);
        char searchBuf[128] = {0};
        strncpy(searchBuf, m_searchFilter.c_str(), sizeof(searchBuf) - 1);
        if (ImGui::InputTextWithHint("##search", "Search actors...", searchBuf, sizeof(searchBuf))) {
            m_searchFilter = searchBuf;
        }
        
        ImGui::Separator();
        
        // Category tabs
        if (ImGui::BeginTabBar("ActorCategories")) {
            if (ImGui::BeginTabItem("All")) {
                m_selectedCategory = ActorCategory::Basic;
                ImGui::EndTabItem();
                
                // Show all categories
                renderCategory(ActorCategory::Basic, "Basic");
                renderCategory(ActorCategory::Characters, "Characters");
                renderCategory(ActorCategory::Props, "Props");
                renderCategory(ActorCategory::Triggers, "Triggers");
                renderCategory(ActorCategory::Lights, "Lights");
                renderCategory(ActorCategory::Audio, "Audio");
            }
            
            if (ImGui::BeginTabItem("Basic")) {
                renderCategory(ActorCategory::Basic, nullptr);
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Characters")) {
                renderCategory(ActorCategory::Characters, nullptr);
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Props")) {
                renderCategory(ActorCategory::Props, nullptr);
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Triggers")) {
                renderCategory(ActorCategory::Triggers, nullptr);
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
#endif
}

void PlaceActorsPanel::renderCategory(ActorCategory category, const char* headerName) {
#ifdef HAS_IMGUI
    bool hasHeader = headerName != nullptr;
    
    if (hasHeader) {
        if (!ImGui::CollapsingHeader(headerName, ImGuiTreeNodeFlags_DefaultOpen)) {
            return;
        }
    }
    
    float panelWidth = ImGui::GetContentRegionAvail().x;
    float buttonSize = 70.0f;
    int columns = std::max(1, (int)(panelWidth / (buttonSize + 8)));
    
    int col = 0;
    for (const auto& templ : m_templates) {
        if (templ.category != category) continue;
        
        // Apply search filter
        if (!m_searchFilter.empty()) {
            std::string lowerName = templ.name;
            std::string lowerFilter = m_searchFilter;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            std::transform(lowerFilter.begin(), lowerFilter.end(), lowerFilter.begin(), ::tolower);
            if (lowerName.find(lowerFilter) == std::string::npos) continue;
        }
        
        renderActorButton(templ);
        
        col++;
        if (col < columns) {
            ImGui::SameLine();
        } else {
            col = 0;
        }
    }
    
    if (col != 0) {
        ImGui::NewLine();
    }
#endif
}

void PlaceActorsPanel::renderActorButton(const ActorTemplate& templ) {
#ifdef HAS_IMGUI
    ImVec2 buttonSize(65, 65);
    ImVec2 buttonPos = ImGui::GetCursorScreenPos();
    
    ImGui::PushID(templ.name.c_str());
    
    // Button
    if (ImGui::Button("##actor", buttonSize)) {
        // Click to select (optional)
    }
    
    // Drag source
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        // Store template name as payload
        ImGui::SetDragDropPayload("ACTOR_TEMPLATE", templ.name.c_str(), templ.name.size() + 1);
        
        // Preview
        ImGui::Text("%s", templ.icon.c_str());
        ImGui::Text("%s", templ.name.c_str());
        ImGui::EndDragDropSource();
    }
    
    // Draw content
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Icon
    ImVec2 iconPos(buttonPos.x + buttonSize.x / 2 - 10, buttonPos.y + 8);
    drawList->AddText(iconPos, IM_COL32(255, 255, 255, 255), templ.icon.c_str());
    
    // Name (truncated)
    std::string displayName = templ.name;
    if (displayName.length() > 8) {
        displayName = displayName.substr(0, 6) + "..";
    }
    ImVec2 textSize = ImGui::CalcTextSize(displayName.c_str());
    ImVec2 textPos(buttonPos.x + (buttonSize.x - textSize.x) / 2, buttonPos.y + buttonSize.y - 18);
    drawList->AddText(textPos, IM_COL32(200, 200, 200, 255), displayName.c_str());
    
    // Tooltip
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s\n\n%s", templ.name.c_str(), templ.description.c_str());
    }
    
    ImGui::PopID();
#endif
}
