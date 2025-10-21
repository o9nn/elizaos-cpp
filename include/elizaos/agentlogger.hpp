#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <fstream>
#include <memory>

namespace elizaos {

/**
 * Log levels for AgentLogger
 */
enum class LogLevel {
    UNKNOWN,
    SYSTEM,
    INFO,
    WARNING,
    SUCCESS,
    ERROR,
    START,
    STOP,
    PAUSE,
    EPOCH,
    SUMMARY,
    REASONING,
    ACTION,
    PROMPT
};

/**
 * Color codes for console output
 */
enum class LogColor {
    WHITE,
    MAGENTA,
    BLUE,
    YELLOW,
    GREEN,
    RED,
    CYAN
};

/**
 * AgentLogger - C++ implementation of the Python agentlogger functionality
 * 
 * Provides colored console logging with file output capabilities,
 * similar to the Python version using Rich library
 */
class AgentLogger {
public:
    /**
     * Constructor
     */
    AgentLogger();
    
    /**
     * Destructor - ensures file handles are closed
     */
    ~AgentLogger();
    
    /**
     * Main logging function
     * @param content Content to log
     * @param source Optional source identifier (function/file name)
     * @param title Title for the log entry
     * @param level Log level
     * @param color Override color (optional)
     * @param expand Whether to expand panel width
     * @param panel Whether to use panel formatting
     * @param shouldLog Whether to actually log (can be disabled)
     */
    void log(
        const std::string& content,
        const std::string& source = "",
        const std::string& title = "agentlogger",
        LogLevel level = LogLevel::INFO,
        LogColor color = LogColor::BLUE,
        bool expand = true,
        bool panel = true,
        bool shouldLog = true
    );
    
    /**
     * Print ASCII header
     * @param text Text to display in ASCII art
     * @param color Color for the header
     */
    void printHeader(
        const std::string& text = "agentlogger",
        LogColor color = LogColor::YELLOW
    );
    
    /**
     * Write content to log file
     * @param content Content to write
     * @param source Optional source identifier
     * @param level Log level
     * @param filename Log filename
     */
    void writeToFile(
        const std::string& content,
        const std::string& source = "",
        LogLevel level = LogLevel::INFO,
        const std::string& filename = "events.log"
    );
    
    /**
     * Set custom type colors
     * @param level Log level
     * @param color Color to use for this level
     */
    void setTypeColor(LogLevel level, LogColor color);
    
    /**
     * Enable/disable console output
     * @param enabled Whether console output is enabled
     */
    void setConsoleEnabled(bool enabled);
    
    /**
     * Enable/disable file output
     * @param enabled Whether file output is enabled
     */
    void setFileEnabled(bool enabled);
    
    /**
     * Convenience logging methods (member functions)
     */
    void logInfo(const std::string& content, const std::string& source = "");
    void logWarning(const std::string& content, const std::string& source = "");
    void logError(const std::string& content, const std::string& source = "");
    void logSuccess(const std::string& content, const std::string& source = "");
    void logSystem(const std::string& content, const std::string& source = "");
    
    /**
     * Panel display method for formatted output
     * @param title Panel title
     * @param content Panel content (can be multi-line)
     * @param color Panel color (optional)
     */
    void panel(const std::string& title, const std::string& content, LogColor color = LogColor::BLUE);
    
private:
    /**
     * Get color code string for console output
     * @param color Color enum
     * @return ANSI color code string
     */
    std::string getColorCode(LogColor color) const;
    
    /**
     * Get default color for log level
     * @param level Log level
     * @return Default color for this level
     */
    LogColor getDefaultColor(LogLevel level) const;
    
    /**
     * Convert log level to string
     * @param level Log level
     * @return String representation
     */
    std::string levelToString(LogLevel level) const;
    
    /**
     * Create formatted panel border
     * @param title Panel title
     * @param color Panel color
     * @param width Panel width
     * @return Formatted border string
     */
    std::string createPanel(
        const std::string& content,
        const std::string& title,
        LogColor color,
        int width = 80
    ) const;
    
    // Member variables
    std::unordered_map<LogLevel, LogColor> typeColors_;
    std::mutex logMutex_;
    bool consoleEnabled_;
    bool fileEnabled_;
    static const int SEPARATOR_WIDTH = 80;
};

/**
 * Global logger instance for convenience
 */
extern std::shared_ptr<AgentLogger> globalLogger;

/**
 * Convenience functions for common log operations
 */
void logInfo(const std::string& content, const std::string& source = "");
void logWarning(const std::string& content, const std::string& source = "");
void logError(const std::string& content, const std::string& source = "");
void logSuccess(const std::string& content, const std::string& source = "");
void logSystem(const std::string& content, const std::string& source = "");

} // namespace elizaos