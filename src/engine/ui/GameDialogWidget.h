/**
 * @file GameDialogWidget.h
 * @brief Delad dialog-UI widget för både editor och runtime
 */
#pragma once

#ifdef HAS_IMGUI

#include <string>
#include <vector>
#include <functional>

/**
 * @brief Dialog UI widget som kan användas i både editor och runtime
 * 
 * Renderar dialog-box med:
 * - Speaker name (gul)
 * - Dialog text (vit)
 * - Choices med nummer (cyan)
 * - [E] Continue om inga val
 */
class GameDialogWidget {
public:
    GameDialogWidget() = default;
    ~GameDialogWidget() = default;
    
    /**
     * @brief Sätt aktuell dialog-data
     */
    void setDialog(const std::string& speaker, const std::string& text,
                   const std::vector<std::pair<int, std::string>>& choices);
    
    /**
     * @brief Rensa dialog (döljer widgeten)
     */
    void clear();
    
    /**
     * @brief Rendera dialog-UI med ImGui
     * @param windowWidth Fönsterbredd
     * @param windowHeight Fönsterhöjd
     * @return Valt choice index (-1 om inget val gjordes)
     */
    int render(int windowWidth, int windowHeight);
    
    /**
     * @brief Kolla om dialog är aktiv
     */
    bool isActive() const { return m_active; }
    
    /**
     * @brief Sätt callback för när ett val görs
     */
    void setOnChoice(std::function<void(int)> callback) { m_onChoice = callback; }
    
    /**
     * @brief Sätt callback för continue (E-tangent)
     */
    void setOnContinue(std::function<void()> callback) { m_onContinue = callback; }

private:
    bool m_active = false;
    std::string m_speaker;
    std::string m_text;
    std::vector<std::pair<int, std::string>> m_choices;
    
    std::function<void(int)> m_onChoice;
    std::function<void()> m_onContinue;
};

#endif // HAS_IMGUI
