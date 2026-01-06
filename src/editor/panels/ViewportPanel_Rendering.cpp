// Helper method for ViewportPanel.cpp - add to ViewportPanel class

void ViewportPanel::renderSceneActors(ImDrawList* drawList, ImVec2 offset) {
#ifdef HAS_IMGUI
    if (!m_scene || !drawList) return;
    
    const auto& actors = m_scene->getActors();
    
    for (const auto& actor : actors) {
        if (!actor || !actor->isActive()) continue;
        
        // Get actor transform
        Vec2 pos = actor->getPosition();
        float worldX = offset.x + pos.x * m_zoom;
        float worldY = offset.y + pos.y * m_zoom;
        
        // Try to get SpriteComponent
        auto* spriteComp = actor->getComponent<SpriteComponent>();
        if (spriteComp) {
            SDL_Texture* tex = spriteComp->getTexture();
            if (tex) {
                int w = spriteComp->getWidth();
                int h = spriteComp->getHeight();
                
                // Render texture via ImGui
                drawList->AddImage(
                    (ImTextureID)(intptr_t)tex,
                    ImVec2(worldX, worldY),
                    ImVec2(worldX + w * m_zoom, worldY + h * m_zoom)
                );
            }
        }
        
        // Debug visualization for actors without sprites
        if (!spriteComp) {
            std::string name = actor->getName();
            
            // Different colors for different actor types
            ImU32 color = IM_COL32(200, 200, 200, 255);
            
            if (name == "PlayerSpawn") {
                color = IM_COL32(255, 0, 255, 255);
                drawList->AddCircleFilled(ImVec2(worldX, worldY), 8.0f * m_zoom, color);
                drawList->AddText(ImVec2(worldX + 10, worldY - 8), color, "Spawn");
            }
            else if (name == "WalkArea") {
                color = IM_COL32(100, 255, 100, 200);
                // Draw box around typical walk area size
                drawList->AddRect(
                    ImVec2(worldX, worldY),
                    ImVec2(worldX + 640 * m_zoom, worldY + 400 * m_zoom),
                    color, 0, 0, 2.0f
                );
            }
            else {
                // Interactive actors (hotspots)
                color = IM_COL32(100, 255, 255, 255);
                drawList->AddRect(
                    ImVec2(worldX, worldY),
                    ImVec2(worldX + 64 * m_zoom, worldY + 64 * m_zoom),
                    color, 0, 0, 2.0f
                );
                drawList->AddText(ImVec2(worldX + 5, worldY + 5), 
                                IM_COL32(255, 255, 255, 255), 
                                name.c_str());
            }
        }
    }
#endif
}
