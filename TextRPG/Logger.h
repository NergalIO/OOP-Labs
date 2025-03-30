#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <mutex>
#include <ctime>
#include <typeinfo>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

struct LogRecord {
    LogLevel level;
    std::string message;
    time_t timestamp;
};

template <typename ClassType>
class Logger {
private:
    std::string loggerName;
    std::ofstream logFile;
    bool outputToConsole;
    LogLevel minOutputLevel;
    std::vector<LogRecord> records;
    std::mutex logMutex;

    std::string getClassName() const {
        return typeid(ClassType).name();
    }

    std::string timestampToString(time_t timestamp) const {
        char buffer[20];
        struct tm timeInfo;
        localtime_s(&timeInfo, &timestamp);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);
        return buffer;
    }

    std::string levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::CRITICAL: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }

    std::string colorizeLevel(LogLevel level) const {
        switch (level) {
            case LogLevel::DEBUG: return "\033[0m";
            case LogLevel::INFO: return "\033[34m";
            case LogLevel::WARNING: return "\033[33m";
            case LogLevel::ERROR: return "\033[31m";
            case LogLevel::CRITICAL: return "\033[35m";
            default: return "\033[0m";
        }
    }

    void writeToFile(const LogRecord& record) {
        if (logFile.is_open()) {
            logFile << timestampToString(record.timestamp) << " [" << levelToString(record.level)
                << "] (" << loggerName << ") " << record.message << std::endl;
        }
    }

public:
    Logger(const std::string& filename = "log.txt")
        : loggerName(getClassName()), outputToConsole(false), minOutputLevel(LogLevel::DEBUG) {
        if (!filename.empty()) {
            logFile.open(filename, std::ios::app);
            debug("Logger created");
        }
    }

    ~Logger() {
        debug("Logger destroyed");
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void setLoggerName(const std::string& name) { loggerName = name; }
    void setOutputToConsole(bool value) { outputToConsole = value; }
    void setMinOutputLevel(LogLevel level) { minOutputLevel = level; }

    void log(LogLevel level, const std::string& message) {
        if (level < minOutputLevel) return;

        std::lock_guard<std::mutex> lock(logMutex);
        LogRecord record { level, message, std::time(nullptr) };
        records.push_back(record);

        if (outputToConsole) {
            std::cout << colorizeLevel(level) << timestampToString(record.timestamp) << " [" << levelToString(level)
                << "] (" << loggerName << ") " << message << "\033[0m" << std::endl;
        }

        writeToFile(record);
    }

    void debug(const std::string& message) { log(LogLevel::DEBUG, message); }
    void info(const std::string& message) { log(LogLevel::INFO, message); }
    void warning(const std::string& message) { log(LogLevel::WARNING, message); }
    void error(const std::string& message) { log(LogLevel::ERROR, message); }
    void critical(const std::string& message) { log(LogLevel::CRITICAL, message); }
};