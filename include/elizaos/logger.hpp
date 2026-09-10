#pragma once

#include "elizaos/agentlogger.hpp"

#include <memory>
#include <string>
#include <utility>

// Legacy global Logger compatibility surface. New code should use
// elizaos::AgentLogger directly.
class Logger {
public:
    explicit Logger(std::string loggerPrefix = "ElizaOS")
        : prefix(std::move(loggerPrefix)), delegate_(std::make_shared<elizaos::AgentLogger>()) {}
    virtual ~Logger() = default;

    virtual void log(const std::string& level, const std::string& message) {
        delegate_->log(message, prefix, "logger", parseLevel(level));
    }
    virtual void info(const std::string& message) {
        delegate_->log(message, prefix, "logger", elizaos::LogLevel::INFO);
    }
    virtual void warn(const std::string& message) {
        delegate_->log(message, prefix, "logger", elizaos::LogLevel::WARNING);
    }
    virtual void error(const std::string& message) {
        delegate_->log(message, prefix, "logger", elizaos::LogLevel::ERROR);
    }
    virtual void debug(const std::string& message) {
        delegate_->log(message, prefix, "logger", elizaos::LogLevel::DEBUG);
    }

    std::string prefix;

private:
    static elizaos::LogLevel parseLevel(const std::string& level) noexcept {
        if (level == "error" || level == "ERROR") return elizaos::LogLevel::ERROR;
        if (level == "warn" || level == "warning" || level == "WARN" || level == "WARNING")
            return elizaos::LogLevel::WARNING;
        if (level == "debug" || level == "DEBUG") return elizaos::LogLevel::DEBUG;
        if (level == "trace" || level == "TRACE") return elizaos::LogLevel::TRACE;
        return elizaos::LogLevel::INFO;
    }

    std::shared_ptr<elizaos::AgentLogger> delegate_;
};

inline std::shared_ptr<Logger> createLogger(std::string prefix = "ElizaOS") {
    return std::make_shared<Logger>(std::move(prefix));
}
