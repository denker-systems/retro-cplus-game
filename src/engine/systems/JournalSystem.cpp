/**
 * @file JournalSystem.cpp
 * @brief Implementation av journal system
 */
#include "JournalSystem.h"
#include "QuestSystem.h"
#include "utils/Logger.h"
#include <algorithm>
#include <sstream>

JournalSystem& JournalSystem::instance() {
    static JournalSystem inst;
    return inst;
}

void JournalSystem::addEntry(const JournalEntry& entry) {
    m_entries.push_back(entry);
    LOG_INFO("Journal entry added: " + entry.title);
}

void JournalSystem::addEntry(JournalEntryType type, const std::string& title, const std::string& content) {
    JournalEntry entry;
    entry.id = "entry_" + std::to_string(m_nextId++);
    entry.type = type;
    entry.title = title;
    entry.content = content;
    entry.timestamp = generateTimestamp();
    entry.isNew = true;
    
    addEntry(entry);
}

std::vector<JournalEntry> JournalSystem::getEntriesByType(JournalEntryType type) const {
    std::vector<JournalEntry> filtered;
    
    for (const auto& entry : m_entries) {
        if (entry.type == type) {
            filtered.push_back(entry);
        }
    }
    
    return filtered;
}

JournalEntry* JournalSystem::getEntry(const std::string& id) {
    for (auto& entry : m_entries) {
        if (entry.id == id) {
            return &entry;
        }
    }
    return nullptr;
}

void JournalSystem::markAsRead(const std::string& id) {
    JournalEntry* entry = getEntry(id);
    if (entry) {
        entry->isNew = false;
    }
}

void JournalSystem::markAllAsRead() {
    for (auto& entry : m_entries) {
        entry.isNew = false;
    }
}

int JournalSystem::getNewEntryCount() const {
    int count = 0;
    for (const auto& entry : m_entries) {
        if (entry.isNew) count++;
    }
    return count;
}

std::vector<JournalEntry> JournalSystem::search(const std::string& query) const {
    std::vector<JournalEntry> results;
    
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    for (const auto& entry : m_entries) {
        std::string lowerTitle = entry.title;
        std::string lowerContent = entry.content;
        std::transform(lowerTitle.begin(), lowerTitle.end(), lowerTitle.begin(), ::tolower);
        std::transform(lowerContent.begin(), lowerContent.end(), lowerContent.begin(), ::tolower);
        
        if (lowerTitle.find(lowerQuery) != std::string::npos ||
            lowerContent.find(lowerQuery) != std::string::npos) {
            results.push_back(entry);
        }
    }
    
    return results;
}

void JournalSystem::clear() {
    m_entries.clear();
    m_nextId = 1;
}

std::string JournalSystem::getKnowledgeSummary() const {
    std::stringstream ss;
    ss << "=== Vad jag vet ===\n\n";
    
    // Samla ledtrådar
    auto clues = getEntriesByType(JournalEntryType::Clue);
    if (!clues.empty()) {
        ss << "Ledtrådar:\n";
        for (const auto& clue : clues) {
            ss << "- " << clue.title << "\n";
        }
        ss << "\n";
    }
    
    // Samla karaktärsinformation
    auto characters = getEntriesByType(JournalEntryType::Character);
    if (!characters.empty()) {
        ss << "Karaktärer:\n";
        for (const auto& character : characters) {
            ss << "- " << character.title << "\n";
        }
        ss << "\n";
    }
    
    return ss.str();
}

std::string JournalSystem::getObjectivesSummary() const {
    std::stringstream ss;
    ss << "=== Vad jag ska göra ===\n\n";
    
    // Hämta aktiva quests från QuestSystem
    const auto& activeQuests = QuestSystem::instance().getActiveQuests();
    
    for (const auto& quest : activeQuests) {
        ss << quest.title << ":\n";
        for (const auto& obj : quest.objectives) {
            if (obj.status != QuestStatus::Completed) {
                ss << "  - " << obj.description << "\n";
            }
        }
        ss << "\n";
    }
    
    return ss.str();
}

std::string JournalSystem::generateTimestamp() const {
    // Enkel timestamp - kan utökas med game time senare
    return "Day 1, 10:00";
}
