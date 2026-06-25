#include "elizaos/agentlogger.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <vector>
#include <fstream>
#include <random>
#include <sys/stat.h>
#include <dirent.h>
#include <cstdlib>
#include <cstdio>

namespace elizaos {

// ============================================================================
// LogEntry implementation
// ============================================================================

std::string LogEntry::toJson() const {
    std::ostringstream oss;
    
    // Get timestamp as ISO 8601 string
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    struct tm tm;
    #ifdef _WIN32
        localtime_s(&tm, &time_t);
    #else
        localtime_r(&time_t, &tm);
    #endif
    char timeStr[64];
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%dT%H:%M:%S", &tm);
    
    // Helper to escape JSON strings
    auto escapeJson = [](const std::string& str) -> std::string {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    };
    
    // Convert level to string
    auto levelStr = [](LogLevel l) -> const char* {
        switch (l) {
            case LogLevel::UNKNOWN: return "unknown";
            case LogLevel::SYSTEM: return "system";
            case LogLevel::INFO: return "info";
            case LogLevel::WARNING: return "warning";
            case LogLevel::SUCCESS: return "success";
            case LogLevel::ERROR: return "error";
            case LogLevel::START: return "start";
            case LogLevel::STOP: return "stop";
            case LogLevel::PAUSE: return "pause";
            case LogLevel::EPOCH: return "epoch";
            case LogLevel::SUMMARY: return "summary";
            case LogLevel::REASONING: return "reasoning";
            case LogLevel::ACTION: return "action";
            case LogLevel::PROMPT: return "prompt";
            case LogLevel::DEBUG: return "debug";
            case LogLevel::TRACE: return "trace";
            case LogLevel::AUDIT: return "audit";
            default: return "unknown";
        }
    };
    
    oss << "{";
    oss << "\"timestamp\":\"" << timeStr << "\",";
    oss << "\"level\":\"" << levelStr(level) << "\",";
    oss << "\"source\":\"" << escapeJson(source) << "\",";
    oss << "\"title\":\"" << escapeJson(title) << "\",";
    oss << "\"content\":\"" << escapeJson(content) << "\"";
    
    if (!agentId.empty()) {
        oss << ",\"agentId\":\"" << escapeJson(agentId) << "\"";
    }
    if (!sessionId.empty()) {
        oss << ",\"sessionId\":\"" << escapeJson(sessionId) << "\"";
    }
    
    if (!metadata.empty()) {
        oss << ",\"metadata\":{";
        bool first = true;
        for (const auto& [key, value] : metadata) {
            if (!first) oss << ",";
            oss << "\"" << escapeJson(key) << "\":\"" << escapeJson(value) << "\"";
            first = false;
        }
        oss << "}";
    }
    
    oss << "}";
    return oss.str();
}

std::string LogEntry::toText() const {
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    struct tm tm;
    #ifdef _WIN32
        localtime_s(&tm, &time_t);
    #else
        localtime_r(&time_t, &tm);
    #endif
    char timeStr[64];
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &tm);
    
    std::ostringstream oss;
    oss << "[" << timeStr << "] ";
    if (!source.empty()) {
        oss << "[" << source << "] ";
    }
    oss << content;
    return oss.str();
}

// ============================================================================
// CognitiveTrace implementation
// ============================================================================

double CognitiveTrace::durationMs() const {
    auto duration = endTime - startTime;
    return std::chrono::duration<double, std::milli>(duration).count();
}

std::string CognitiveTrace::toJson() const {
    std::ostringstream oss;
    
    auto formatTime = [](std::chrono::system_clock::time_point tp) -> std::string {
        auto time_t = std::chrono::system_clock::to_time_t(tp);
        struct tm tm;
        #ifdef _WIN32
            localtime_s(&tm, &time_t);
        #else
            localtime_r(&time_t, &tm);
        #endif
        char timeStr[64];
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%dT%H:%M:%S", &tm);
        return timeStr;
    };
    
    auto escapeJson = [](const std::string& str) -> std::string {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    };
    
    oss << "{";
    oss << "\"traceId\":\"" << escapeJson(traceId) << "\",";
    oss << "\"startTime\":\"" << formatTime(startTime) << "\",";
    oss << "\"endTime\":\"" << formatTime(endTime) << "\",";
    oss << "\"durationMs\":" << std::fixed << std::setprecision(3) << durationMs() << ",";
    oss << "\"agentId\":\"" << escapeJson(agentId) << "\",";
    oss << "\"operationType\":\"" << escapeJson(operationType) << "\",";
    
    oss << "\"events\":[";
    for (size_t i = 0; i < events.size(); ++i) {
        if (i > 0) oss << ",";
        oss << events[i].toJson();
    }
    oss << "],";
    
    oss << "\"context\":{";
    bool first = true;
    for (const auto& [key, value] : context) {
        if (!first) oss << ",";
        oss << "\"" << escapeJson(key) << "\":\"" << escapeJson(value) << "\"";
        first = false;
    }
    oss << "}";
    
    oss << "}";
    return oss.str();
}

// Global logger instance
std::shared_ptr<AgentLogger> globalLogger = std::make_shared<AgentLogger>();

AgentLogger::AgentLogger() : consoleEnabled_(true), fileEnabled_(true), logFormat_(LogFormat::TEXT) {
    // Initialize default type colors
    typeColors_[LogLevel::UNKNOWN] = LogColor::WHITE;
    typeColors_[LogLevel::SYSTEM] = LogColor::MAGENTA;
    typeColors_[LogLevel::INFO] = LogColor::BLUE;
    typeColors_[LogLevel::WARNING] = LogColor::YELLOW;
    typeColors_[LogLevel::SUCCESS] = LogColor::GREEN;
    typeColors_[LogLevel::ERROR] = LogColor::RED;
    typeColors_[LogLevel::START] = LogColor::GREEN;
    typeColors_[LogLevel::STOP] = LogColor::RED;
    typeColors_[LogLevel::PAUSE] = LogColor::YELLOW;
    typeColors_[LogLevel::EPOCH] = LogColor::WHITE;
    typeColors_[LogLevel::SUMMARY] = LogColor::CYAN;
    typeColors_[LogLevel::REASONING] = LogColor::CYAN;
    typeColors_[LogLevel::ACTION] = LogColor::GREEN;
    typeColors_[LogLevel::PROMPT] = LogColor::CYAN;
    typeColors_[LogLevel::DEBUG] = LogColor::WHITE;
    typeColors_[LogLevel::TRACE] = LogColor::WHITE;
    typeColors_[LogLevel::AUDIT] = LogColor::MAGENTA;
}

AgentLogger::~AgentLogger() {
    // Destructor - file handles are automatically closed
}

void AgentLogger::log(
    const std::string& content,
    const std::string& source,
    const std::string& title,
    LogLevel level,
    LogColor color,
    bool /* expand */,
    bool panel,
    bool shouldLog
) {
    if (!shouldLog) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(logMutex_);
    
    // Build title with level and source
    std::string fullTitle = "(" + levelToString(level) + ") " + title;
    if (!source.empty()) {
        fullTitle += ": " + source;
    }
    
    // Use type color if color not overridden
    LogColor finalColor = (color == LogColor::BLUE) ? getDefaultColor(level) : color;
    
    if (consoleEnabled_) {
        if (panel) {
            std::cout << std::endl;
            std::cout << createPanel(content, fullTitle, finalColor) << std::endl;
        } else {
            std::cout << getColorCode(finalColor) << content << "\033[0m" << std::endl;
        }
    }
    
    // Also write to file if enabled
    if (fileEnabled_) {
        writeToFile(content, source, level);
    }
}

void AgentLogger::printHeader(const std::string& text, LogColor color) {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    if (!consoleEnabled_) {
        return;
    }
    
    // Simple ASCII header (could be enhanced with figlet-style formatting)
    std::string header = "=== " + text + " ===";
    std::string colorCode = getColorCode(color);
    
    std::cout << std::endl;
    std::cout << colorCode << header << "\033[0m" << std::endl;
    std::cout << std::endl;
}

void AgentLogger::writeToFile(
    const std::string& content,
    const std::string& source,
    LogLevel level,
    const std::string& filename
) {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        return; // Silently fail if file can't be opened
    }
    
    // Build header
    std::string header = "";
    if (!source.empty()) {
        header += source;
    }
    if (level != LogLevel::INFO) {
        if (!header.empty()) {
            header += ": ";
        }
        header += levelToString(level);
    }
    
    if (!header.empty()) {
        header = " " + header + " ";
    }
    
    // Create separator
    int barLength = (SEPARATOR_WIDTH - static_cast<int>(header.length())) / 2;
    std::string separator = std::string(barLength, '=') + header + std::string(barLength, '=');
    std::string footer = std::string(SEPARATOR_WIDTH, '=');
    
    // Get timestamp (cross-platform compatible)
    auto now = std::time(nullptr);
    struct tm tm;
    #ifdef _WIN32
        localtime_s(&tm, &now);  // Windows safe version
    #else
        localtime_r(&now, &tm);  // POSIX version
    #endif
    char timestamp[100];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm);
    
    // Write to file
    file << separator << std::endl << std::endl;
    file << "[" << timestamp << "] " << content << std::endl << std::endl;
    file << footer << std::endl << std::endl;
    
    file.close();
}

void AgentLogger::setTypeColor(LogLevel level, LogColor color) {
    std::lock_guard<std::mutex> lock(logMutex_);
    typeColors_[level] = color;
}

void AgentLogger::setConsoleEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(logMutex_);
    consoleEnabled_ = enabled;
}

void AgentLogger::setFileEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(logMutex_);
    fileEnabled_ = enabled;
}

std::string AgentLogger::getColorCode(LogColor color) const {
    switch (color) {
        case LogColor::WHITE:   return "\033[37m";
        case LogColor::MAGENTA: return "\033[35m";
        case LogColor::BLUE:    return "\033[34m";
        case LogColor::YELLOW:  return "\033[33m";
        case LogColor::GREEN:   return "\033[32m";
        case LogColor::RED:     return "\033[31m";
        case LogColor::CYAN:    return "\033[36m";
        default:                return "\033[37m";
    }
}

LogColor AgentLogger::getDefaultColor(LogLevel level) const {
    auto it = typeColors_.find(level);
    return (it != typeColors_.end()) ? it->second : LogColor::WHITE;
}

std::string AgentLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::UNKNOWN:     return "unknown";
        case LogLevel::SYSTEM:      return "system";
        case LogLevel::INFO:        return "info";
        case LogLevel::WARNING:     return "warning";
        case LogLevel::SUCCESS:     return "success";
        case LogLevel::ERROR:       return "error";
        case LogLevel::START:       return "start";
        case LogLevel::STOP:        return "stop";
        case LogLevel::PAUSE:       return "pause";
        case LogLevel::EPOCH:       return "epoch";
        case LogLevel::SUMMARY:     return "summary";
        case LogLevel::REASONING:   return "reasoning";
        case LogLevel::ACTION:      return "action";
        case LogLevel::PROMPT:      return "prompt";
        case LogLevel::DEBUG:       return "debug";
        case LogLevel::TRACE:       return "trace";
        case LogLevel::AUDIT:       return "audit";
        default:                    return "unknown";
    }
}

std::string AgentLogger::createPanel(
    const std::string& content,
    const std::string& title,
    LogColor color,
    int width
) const {
    std::string colorCode = getColorCode(color);
    std::string reset = "\033[0m";
    
    // Create top border with title (using simple ASCII characters)
    std::string topBorder = colorCode + "+";
    if (!title.empty()) {
        int titleSpace = width - 4 - static_cast<int>(title.length());
        if (titleSpace > 0) {
            topBorder += "- " + title + " " + std::string(titleSpace, '-');
        } else {
            topBorder += std::string(width - 2, '-');
        }
    } else {
        topBorder += std::string(width - 2, '-');
    }
    topBorder += "+" + reset;
    
    // Create content lines with proper wrapping
    std::vector<std::string> lines;
    std::istringstream iss(content);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.length() <= static_cast<size_t>(width - 4)) {
            lines.push_back(line);
        } else {
            // Simple word wrapping
            size_t pos = 0;
            while (pos < line.length()) {
                size_t end = std::min(pos + width - 4, line.length());
                lines.push_back(line.substr(pos, end - pos));
                pos = end;
            }
        }
    }
    
    // Create content with side borders
    std::string result = topBorder + "\n";
    for (const auto& contentLine : lines) {
        result += colorCode + "| " + reset + contentLine;
        // Pad to width
        int padding = width - 4 - static_cast<int>(contentLine.length());
        if (padding > 0) {
            result += std::string(padding, ' ');
        }
        result += colorCode + " |" + reset + "\n";
    }
    
    // Create bottom border (using simple ASCII characters)
    std::string bottomBorder = colorCode + "+" + std::string(width - 2, '-') + "+" + reset;
    result += bottomBorder;
    
    return result;
}

// Convenience functions
void logInfo(const std::string& content, const std::string& source) {
    globalLogger->log(content, source, "agentlogger", LogLevel::INFO);
}

void logWarning(const std::string& content, const std::string& source) {
    globalLogger->log(content, source, "agentlogger", LogLevel::WARNING);
}

void logError(const std::string& content, const std::string& source) {
    globalLogger->log(content, source, "agentlogger", LogLevel::ERROR);
}

void logSuccess(const std::string& content, const std::string& source) {
    globalLogger->log(content, source, "agentlogger", LogLevel::SUCCESS);
}

void logSystem(const std::string& content, const std::string& source) {
    globalLogger->log(content, source, "agentlogger", LogLevel::SYSTEM);
}

void logDebug(const std::string& content, const std::string& source) {
    globalLogger->log(content, source, "agentlogger", LogLevel::DEBUG);
}

void logTrace(const std::string& content, const std::string& source) {
    globalLogger->log(content, source, "agentlogger", LogLevel::TRACE);
}

void logJsonInfo(const std::string& content, 
                 const std::unordered_map<std::string, std::string>& metadata) {
    globalLogger->logJson(content, "", LogLevel::INFO, metadata);
}

void logJsonError(const std::string& content,
                  const std::unordered_map<std::string, std::string>& metadata) {
    globalLogger->logJson(content, "", LogLevel::ERROR, metadata);
}

// ============================================================================
// Task 1.4.1: Structured JSON Logging
// ============================================================================

void AgentLogger::logStructured(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    if (consoleEnabled_) {
        if (logFormat_ == LogFormat::JSON) {
            std::cout << entry.toJson() << std::endl;
        } else {
            LogColor color = getDefaultColor(entry.level);
            std::cout << getColorCode(color) << entry.toText() << "\033[0m" << std::endl;
        }
    }
    
    if (fileEnabled_) {
        writeJsonToFile(entry);
    }
}

void AgentLogger::logJson(const std::string& content,
                          const std::string& source,
                          LogLevel level,
                          const std::unordered_map<std::string, std::string>& metadata) {
    LogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = level;
    entry.source = source;
    entry.title = "agentlogger";
    entry.content = content;
    entry.agentId = agentId_;
    entry.sessionId = sessionId_;
    entry.metadata = metadata;
    
    logStructured(entry);
}

void AgentLogger::writeJsonToFile(const LogEntry& entry, const std::string& filename) {
    checkAndRotate(filename);
    
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        return;
    }
    
    file << entry.toJson() << std::endl;
    file.close();
}

// ============================================================================
// Task 1.4.3: Log Rotation
// ============================================================================

void AgentLogger::setRotationConfig(const LogRotationConfig& config) {
    std::lock_guard<std::mutex> lock(logMutex_);
    rotationConfig_ = config;
}

size_t AgentLogger::getFileSize(const std::string& filename) const {
    struct stat st;
    if (stat(filename.c_str(), &st) == 0) {
        return static_cast<size_t>(st.st_size);
    }
    return 0;
}

void AgentLogger::checkAndRotate(const std::string& filename) {
    if (getFileSize(filename) >= rotationConfig_.maxFileSize) {
        rotateLogFile(filename);
    }
}

void AgentLogger::rotateLogFile(const std::string& filename) {
    // Get current timestamp for rotation suffix
    auto now = std::time(nullptr);
    struct tm tm;
    #ifdef _WIN32
        localtime_s(&tm, &now);
    #else
        localtime_r(&now, &tm);
    #endif
    char suffix[64];
    std::strftime(suffix, sizeof(suffix), rotationConfig_.rotationPattern.c_str(), &tm);
    
    std::string rotatedName = filename + suffix;
    
    // Rename current log file
    std::rename(filename.c_str(), rotatedName.c_str());

    // Optionally compress the freshly-rotated file. We shell out to gzip when
    // available; on success the rotated artifact becomes <rotatedName>.gz.
    if (rotationConfig_.compressRotated) {
        const std::string cmd = "gzip -f \"" + rotatedName + "\" 2>/dev/null";
        if (std::system(cmd.c_str()) == 0) {
            rotatedName += ".gz";
        }
    }

    // Enforce the retention policy: keep at most maxFiles rotated artifacts for
    // this base log, deleting the oldest first. Rotated names share the base
    // filename prefix; we enumerate the containing directory, select matching
    // siblings, sort by last-modified time, and prune the excess.
    enforceRotationRetention(filename);
}

void AgentLogger::enforceRotationRetention(const std::string& baseFilename) {
    if (rotationConfig_.maxFiles == 0) {
        return; // 0 => unlimited retention
    }

    // Split baseFilename into directory + leaf prefix.
    std::string dir = ".";
    std::string prefix = baseFilename;
    const auto slash = baseFilename.find_last_of('/');
    if (slash != std::string::npos) {
        dir = baseFilename.substr(0, slash);
        prefix = baseFilename.substr(slash + 1);
    }

    // Collect rotated siblings: leaf starts with "<prefix>" but is not the
    // active log file itself (which has leaf == prefix exactly).
    struct Rotated { std::string path; time_t mtime; };
    std::vector<Rotated> rotated;

    if (DIR* d = ::opendir(dir.c_str())) {
        while (struct dirent* ent = ::readdir(d)) {
            const std::string leaf = ent->d_name;
            if (leaf == prefix) continue;                       // active log
            if (leaf.rfind(prefix, 0) != 0) continue;           // not our family
            const std::string full = dir + "/" + leaf;
            struct stat st;
            if (::stat(full.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
                rotated.push_back({full, st.st_mtime});
            }
        }
        ::closedir(d);
    }

    if (rotated.size() <= rotationConfig_.maxFiles) {
        return;
    }

    // Oldest first.
    std::sort(rotated.begin(), rotated.end(),
              [](const Rotated& a, const Rotated& b) { return a.mtime < b.mtime; });

    const size_t toRemove = rotated.size() - rotationConfig_.maxFiles;
    for (size_t i = 0; i < toRemove; ++i) {
        std::remove(rotated[i].path.c_str());
    }
}

void AgentLogger::setLogFormat(LogFormat format) {
    std::lock_guard<std::mutex> lock(logMutex_);
    logFormat_ = format;
}

void AgentLogger::setAgentId(const std::string& agentId) {
    std::lock_guard<std::mutex> lock(logMutex_);
    agentId_ = agentId;
}

void AgentLogger::setSessionId(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(logMutex_);
    sessionId_ = sessionId;
}

// ============================================================================
// Task 1.4.4: Cognitive Introspection Trace
// ============================================================================

std::string AgentLogger::startTrace(const std::string& operationType) {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    // Generate unique trace ID
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static const char* hexChars = "0123456789abcdef";
    
    std::string traceId;
    for (int i = 0; i < 16; ++i) {
        traceId += hexChars[dis(gen)];
    }
    
    CognitiveTrace trace;
    trace.traceId = traceId;
    trace.startTime = std::chrono::system_clock::now();
    trace.agentId = agentId_;
    trace.operationType = operationType;
    
    activeTraces_[traceId] = trace;
    
    return traceId;
}

void AgentLogger::addTraceEvent(const std::string& traceId, const LogEntry& event) {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    auto it = activeTraces_.find(traceId);
    if (it != activeTraces_.end()) {
        it->second.events.push_back(event);
    }
}

CognitiveTrace AgentLogger::endTrace(const std::string& traceId) {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    auto it = activeTraces_.find(traceId);
    if (it == activeTraces_.end()) {
        return CognitiveTrace{};
    }
    
    CognitiveTrace trace = std::move(it->second);
    trace.endTime = std::chrono::system_clock::now();
    activeTraces_.erase(it);
    
    // Store in completed traces (bounded)
    if (completedTraces_.size() >= MAX_COMPLETED_TRACES) {
        completedTraces_.pop_front();
    }
    completedTraces_.push_back(trace);
    
    return completedTraces_.back();
}

void AgentLogger::exportTraces(const std::string& filename) {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "[" << std::endl;
    bool first = true;
    for (const auto& trace : completedTraces_) {
        if (!first) file << "," << std::endl;
        file << trace.toJson();
        first = false;
    }
    file << std::endl << "]" << std::endl;
    
    file.close();
}

// ============================================================================
// Task 1.4.5: Audit Trail
// ============================================================================

void AgentLogger::logAudit(const std::string& action,
                           const std::string& subject,
                           const std::string& outcome,
                           const std::unordered_map<std::string, std::string>& details) {
    LogEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.level = LogLevel::AUDIT;
    entry.source = "audit";
    entry.title = "audit";
    entry.content = action;
    entry.agentId = agentId_;
    entry.sessionId = sessionId_;
    entry.metadata = details;
    entry.metadata["action"] = action;
    entry.metadata["subject"] = subject;
    entry.metadata["outcome"] = outcome;
    
    {
        std::lock_guard<std::mutex> lock(logMutex_);
        if (auditTrail_.size() >= MAX_AUDIT_ENTRIES) {
            auditTrail_.pop_front();
        }
        auditTrail_.push_back(entry);
    }
    
    // Also log it
    logStructured(entry);
}

void AgentLogger::exportAuditTrail(const std::string& filename) {
    std::lock_guard<std::mutex> lock(logMutex_);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "[" << std::endl;
    bool first = true;
    for (const auto& entry : auditTrail_) {
        if (!first) file << "," << std::endl;
        file << entry.toJson();
        first = false;
    }
    file << std::endl << "]" << std::endl;
    
    file.close();
}

std::string AgentLogger::getTimestamp() const {
    auto now = std::time(nullptr);
    struct tm tm;
    #ifdef _WIN32
        localtime_s(&tm, &now);
    #else
        localtime_r(&now, &tm);
    #endif
    char timestamp[100];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm);
    return timestamp;
}

std::string AgentLogger::escapeJson(const std::string& str) const {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

} // namespace elizaos
