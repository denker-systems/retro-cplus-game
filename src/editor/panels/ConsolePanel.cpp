/**
 * @file ConsolePanel.cpp
 * @brief Console panel implementation
 */
#include "ConsolePanel.h"
#include "editor/EditorContext.h"
#include "engine/data/DataLoader.h"
#include <ctime>
#include <iomanip>
#include <sstream>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

ConsolePanel::ConsolePanel(EditorContext& context)
    : m_context(context) {
    log("Editor initialized");
}

void ConsolePanel::addEntry(LogEntry::Level level, const std::string& message) {
    // Get timestamp
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.timestamp = oss.str();
    
    m_entries.push_back(entry);
    
    // Limit size
    while (m_entries.size() > MAX_ENTRIES) {
        m_entries.pop_front();
    }
}

void ConsolePanel::log(const std::string& message) {
    addEntry(LogEntry::Level::Info, message);
}

void ConsolePanel::logWarning(const std::string& message) {
    addEntry(LogEntry::Level::Warning, message);
}

void ConsolePanel::logError(const std::string& message) {
    addEntry(LogEntry::Level::Error, message);
}

void ConsolePanel::clear() {
    m_entries.clear();
}

void ConsolePanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        // Toolbar
        if (ImGui::Button("Clear")) {
            clear();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &m_autoScroll);
        ImGui::SameLine();
        ImGui::Checkbox("Info", &m_showInfo);
        ImGui::SameLine();
        ImGui::Checkbox("Warnings", &m_showWarnings);
        ImGui::SameLine();
        ImGui::Checkbox("Errors", &m_showErrors);
        
        ImGui::Separator();
        
        // Status from context
        if (!m_context.statusMessage.empty()) {
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), 
                "Status: %s", m_context.statusMessage.c_str());
            ImGui::Separator();
        }
        
        // Stats
        const auto& rooms = DataLoader::instance().getRooms();
        const auto& dialogs = DataLoader::instance().getDialogs();
        const auto& quests = DataLoader::instance().getQuests();
        const auto& items = DataLoader::instance().getItems();
        const auto& npcs = DataLoader::instance().getNPCs();
        
        ImGui::Text("Loaded: %d rooms, %d dialogs, %d quests, %d items, %d npcs",
            (int)rooms.size(), (int)dialogs.size(), (int)quests.size(), 
            (int)items.size(), (int)npcs.size());
        
        ImGui::Separator();
        
        // Log entries
        if (ImGui::BeginChild("LogScroll", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar)) {
            for (const auto& entry : m_entries) {
                // Filter by level
                if (entry.level == LogEntry::Level::Info && !m_showInfo) continue;
                if (entry.level == LogEntry::Level::Warning && !m_showWarnings) continue;
                if (entry.level == LogEntry::Level::Error && !m_showErrors) continue;
                
                // Color based on level
                ImVec4 color;
                const char* prefix;
                switch (entry.level) {
                    case LogEntry::Level::Warning:
                        color = ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
                        prefix = "[WARN]";
                        break;
                    case LogEntry::Level::Error:
                        color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
                        prefix = "[ERR]";
                        break;
                    default:
                        color = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
                        prefix = "[INFO]";
                        break;
                }
                
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[%s]", entry.timestamp.c_str());
                ImGui::SameLine();
                ImGui::TextColored(color, "%s %s", prefix, entry.message.c_str());
            }
            
            if (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.0f);
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
#endif
}
