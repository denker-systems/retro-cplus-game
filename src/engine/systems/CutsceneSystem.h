/**
 * @file CutsceneSystem.h
 * @brief System för att spela sekventiella cutscenes
 */
#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>

/**
 * @brief Typ av cutscene-action
 */
enum class CutsceneActionType {
    Wait,           // Vänta X sekunder
    ShowText,       // Visa text på skärmen
    HideText,       // Dölj text
    FadeIn,         // Fade in från svart
    FadeOut,        // Fade out till svart
    MoveNPC,        // Flytta NPC till position
    MovePlayer,     // Flytta spelaren till position
    PlaySound,      // Spela ljudeffekt
    PlayMusic,      // Byt musik
    ChangeRoom,     // Byt rum
    SetFlag,        // Sätt game flag
    Callback        // Kör custom funktion
};

/**
 * @brief En enskild action i en cutscene
 */
struct CutsceneAction {
    CutsceneActionType type;
    
    // Parametrar (används olika beroende på typ)
    float duration = 0.0f;
    std::string text;
    std::string speaker;
    std::string targetId;       // NPC-id, room-id, flag-namn
    float x = 0.0f, y = 0.0f;   // Målposition
    bool waitForCompletion = true;
    
    std::function<void()> callback;
};

/**
 * @brief En komplett cutscene med flera actions
 */
struct Cutscene {
    std::string id;
    std::string name;
    std::vector<CutsceneAction> actions;
    bool skippable = true;
};

/**
 * @brief Singleton för cutscene-hantering
 */
class CutsceneSystem {
public:
    static CutsceneSystem& instance();
    
    /** @brief Registrera en cutscene */
    void addCutscene(const Cutscene& cutscene);
    
    /** @brief Spela en cutscene */
    bool play(const std::string& cutsceneId);
    
    /** @brief Stoppa pågående cutscene */
    void stop();
    
    /** @brief Skippa till nästa action */
    void skip();
    
    /** @brief Uppdatera cutscene (kalla varje frame) */
    void update(float deltaTime);
    
    /** @brief Rendera cutscene overlay (text etc) */
    void render(struct SDL_Renderer* renderer);
    
    /** @brief Kolla om cutscene spelas */
    bool isPlaying() const { return m_playing; }
    
    /** @brief Hämta aktuell text att visa */
    const std::string& getCurrentText() const { return m_currentText; }
    const std::string& getCurrentSpeaker() const { return m_currentSpeaker; }
    bool hasTextToShow() const { return m_showText; }
    
    /** @brief Sätt callback för när cutscene är klar */
    void setOnComplete(std::function<void()> callback) {
        m_onComplete = callback;
    }
    
    // Hjälpmetoder för att skapa actions
    static CutsceneAction wait(float seconds);
    static CutsceneAction showText(const std::string& text, const std::string& speaker = "");
    static CutsceneAction hideText();
    static CutsceneAction fadeIn(float duration = 0.5f);
    static CutsceneAction fadeOut(float duration = 0.5f);
    static CutsceneAction moveNPC(const std::string& npcId, float x, float y);
    static CutsceneAction movePlayer(float x, float y);
    static CutsceneAction changeRoom(const std::string& roomId);
    static CutsceneAction setFlag(const std::string& flag, bool value = true);
    static CutsceneAction runCallback(std::function<void()> cb);

private:
    CutsceneSystem() = default;
    
    void executeAction(const CutsceneAction& action);
    void advanceToNextAction();
    bool isCurrentActionComplete() const;
    
    std::unordered_map<std::string, Cutscene> m_cutscenes;
    
    bool m_playing = false;
    const Cutscene* m_currentCutscene = nullptr;
    size_t m_currentActionIndex = 0;
    float m_actionTimer = 0.0f;
    
    // Text display
    bool m_showText = false;
    std::string m_currentText;
    std::string m_currentSpeaker;
    
    // Fade
    bool m_fading = false;
    float m_fadeAlpha = 0.0f;
    float m_fadeTarget = 0.0f;
    float m_fadeSpeed = 0.0f;
    
    // Movement tracking
    bool m_waitingForMovement = false;
    
    std::function<void()> m_onComplete;
};
