/**
 * @file Logger.h
 * @brief Centralt loggningssystem med fil- och konsol-output
 */
#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <mutex>
#include <chrono>
#include <iomanip>

/**
 * @brief Loggnivåer
 */
enum class LogLevel {
    Debug,      // Detaljerad debug-info
    Info,       // Generell information
    Warning,    // Varningar
    Error,      // Fel
    Critical    // Kritiska fel
};

/**
 * @brief Singleton Logger med fil- och konsol-output
 */
class Logger {
public:
    static Logger& instance();
    
    /** @brief Initiera loggern med filnamn */
    bool init(const std::string& filename = "game.log", bool appendMode = false);
    
    /** @brief Stäng loggfilen */
    void shutdown();
    
    /** @brief Logga meddelande */
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0);
    
    /** @brief Bekvämlighetsmetoder */
    void debug(const std::string& msg, const std::string& file = "", int line = 0);
    void info(const std::string& msg, const std::string& file = "", int line = 0);
    void warning(const std::string& msg, const std::string& file = "", int line = 0);
    void error(const std::string& msg, const std::string& file = "", int line = 0);
    void critical(const std::string& msg, const std::string& file = "", int line = 0);
    
    /** @brief Inställningar */
    void setConsoleOutput(bool enabled) { m_consoleOutput = enabled; }
    void setFileOutput(bool enabled) { m_fileOutput = enabled; }
    void setMinLevel(LogLevel level) { m_minLevel = level; }
    
    /** @brief Hämta loggfilens sökväg */
    const std::string& getLogPath() const { return m_filename; }

private:
    Logger() = default;
    ~Logger();
    
    std::string levelToString(LogLevel level) const;
    std::string getTimestamp() const;
    std::string formatMessage(LogLevel level, const std::string& message,
                             const std::string& file, int line) const;
    
    std::ofstream m_file;
    std::string m_filename;
    std::mutex m_mutex;
    
    bool m_initialized = false;
    bool m_consoleOutput = true;
    bool m_fileOutput = true;
    LogLevel m_minLevel = LogLevel::Debug;
};

// Makron för enkel loggning med fil/rad
#define LOG_DEBUG(msg)    Logger::instance().debug(msg, __FILE__, __LINE__)
#define LOG_INFO(msg)     Logger::instance().info(msg, __FILE__, __LINE__)
#define LOG_WARNING(msg)  Logger::instance().warning(msg, __FILE__, __LINE__)
#define LOG_ERROR(msg)    Logger::instance().error(msg, __FILE__, __LINE__)
#define LOG_CRITICAL(msg) Logger::instance().critical(msg, __FILE__, __LINE__)
