/**
 * @file Logger.cpp
 * @brief Implementation av loggningssystem
 */
#include "Logger.h"
#include <iostream>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

Logger::~Logger() {
    shutdown();
}

bool Logger::init(const std::string& filename, bool appendMode) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_initialized) {
        return true;
    }
    
    m_filename = filename;
    
    // Skapa mappen om den inte finns
    fs::path filepath(filename);
    if (filepath.has_parent_path()) {
        fs::create_directories(filepath.parent_path());
    }
    
    auto mode = std::ios::out;
    if (appendMode) {
        mode |= std::ios::app;
    }
    
    m_file.open(filename, mode);
    if (!m_file.is_open()) {
        std::cerr << "Logger: Failed to open log file: " << filename << std::endl;
        return false;
    }
    
    m_initialized = true;
    
    // Skriv header
    m_file << "========================================\n";
    m_file << "  Game Log - " << getTimestamp() << "\n";
    m_file << "========================================\n\n";
    m_file.flush();
    
    return true;
}

void Logger::shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_file.is_open()) {
        m_file << "\n========================================\n";
        m_file << "  Log ended - " << getTimestamp() << "\n";
        m_file << "========================================\n";
        m_file.close();
    }
    
    m_initialized = false;
}

void Logger::log(LogLevel level, const std::string& message,
                 const std::string& file, int line) {
    if (level < m_minLevel) return;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::string formatted = formatMessage(level, message, file, line);
    
    // Fil-output
    if (m_fileOutput && m_file.is_open()) {
        m_file << formatted << "\n";
        m_file.flush();
    }
    
    // Konsol-output med färger (Windows)
    if (m_consoleOutput) {
#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        WORD color = 7; // Default white
        
        switch (level) {
            case LogLevel::Debug:    color = 8;  break; // Dark gray
            case LogLevel::Info:     color = 7;  break; // White
            case LogLevel::Warning:  color = 14; break; // Yellow
            case LogLevel::Error:    color = 12; break; // Red
            case LogLevel::Critical: color = 79; break; // White on red
        }
        
        SetConsoleTextAttribute(hConsole, color);
        std::cout << formatted << std::endl;
        SetConsoleTextAttribute(hConsole, 7); // Reset
#else
        std::cout << formatted << std::endl;
#endif
    }
}

void Logger::debug(const std::string& msg, const std::string& file, int line) {
    log(LogLevel::Debug, msg, file, line);
}

void Logger::info(const std::string& msg, const std::string& file, int line) {
    log(LogLevel::Info, msg, file, line);
}

void Logger::warning(const std::string& msg, const std::string& file, int line) {
    log(LogLevel::Warning, msg, file, line);
}

void Logger::error(const std::string& msg, const std::string& file, int line) {
    log(LogLevel::Error, msg, file, line);
}

void Logger::critical(const std::string& msg, const std::string& file, int line) {
    log(LogLevel::Critical, msg, file, line);
}

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO ";
        case LogLevel::Warning:  return "WARN ";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRIT ";
        default:                 return "?????";
    }
}

std::string Logger::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::tm tm_buf;
#ifdef _WIN32
    localtime_s(&tm_buf, &time);
#else
    localtime_r(&time, &tm_buf);
#endif
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

std::string Logger::formatMessage(LogLevel level, const std::string& message,
                                  const std::string& file, int line) const {
    std::ostringstream oss;
    oss << "[" << getTimestamp() << "] ";
    oss << "[" << levelToString(level) << "] ";
    
    // Lägg till fil:rad om tillgängligt (förkorta sökvägen)
    if (!file.empty()) {
        std::string shortFile = file;
        size_t pos = file.find_last_of("/\\");
        if (pos != std::string::npos) {
            shortFile = file.substr(pos + 1);
        }
        oss << "[" << shortFile << ":" << line << "] ";
    }
    
    oss << message;
    return oss.str();
}
