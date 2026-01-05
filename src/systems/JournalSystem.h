/**
 * @file JournalSystem.h
 * @brief System för spelarens journal/loggbok
 * 
 * Automatisk loggning av viktiga händelser, ledtrådar och kunskap
 */
#pragma once

#include <string>
#include <vector>
#include <unordered_map>

/**
 * @brief Typ av journal-entry
 */
enum class JournalEntryType {
    Event,       // Viktig händelse
    Clue,        // Ledtråd
    Character,   // Info om karaktär
    Location,    // Info om plats
    Item,        // Info om föremål
    Quest        // Quest-relaterat
};

/**
 * @brief En journal-entry
 */
struct JournalEntry {
    std::string id;
    JournalEntryType type;
    std::string title;
    std::string content;
    std::string timestamp;  // Game time eller real time
    bool isNew = true;      // Markera som "ny" tills spelaren läst
};

/**
 * @brief Hanterar spelarens journal
 */
class JournalSystem {
public:
    static JournalSystem& instance();
    
    /** @brief Lägg till entry */
    void addEntry(const JournalEntry& entry);
    
    /** @brief Lägg till entry (enkel version) */
    void addEntry(JournalEntryType type, const std::string& title, const std::string& content);
    
    /** @brief Hämta alla entries */
    const std::vector<JournalEntry>& getEntries() const { return m_entries; }
    
    /** @brief Hämta entries av specifik typ */
    std::vector<JournalEntry> getEntriesByType(JournalEntryType type) const;
    
    /** @brief Hämta entry by ID */
    JournalEntry* getEntry(const std::string& id);
    
    /** @brief Markera entry som läst */
    void markAsRead(const std::string& id);
    
    /** @brief Markera alla som lästa */
    void markAllAsRead();
    
    /** @brief Antal nya entries */
    int getNewEntryCount() const;
    
    /** @brief Sök i journal */
    std::vector<JournalEntry> search(const std::string& query) const;
    
    /** @brief Rensa journal */
    void clear();
    
    /** @brief Sammanfattning av vad spelaren vet */
    std::string getKnowledgeSummary() const;
    
    /** @brief Sammanfattning av vad spelaren ska göra */
    std::string getObjectivesSummary() const;

private:
    JournalSystem() = default;
    ~JournalSystem() = default;
    JournalSystem(const JournalSystem&) = delete;
    JournalSystem& operator=(const JournalSystem&) = delete;
    
    std::string generateTimestamp() const;
    
    std::vector<JournalEntry> m_entries;
    int m_nextId = 1;
};
