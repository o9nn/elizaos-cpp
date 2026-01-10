#include "/home/ubuntu/elizaos-cpp/test_hybrid/src/logger.h"

Logger::Logger(string prefix) {
    this->prefix = prefix;
}

void Logger::log(string level, string message)
{
    auto timestamp = ((std::make_shared<Date>()))->toISOString();
    console->log(std::string("[") + timestamp + std::string("] [") + this->prefix + std::string("] [") + level + std::string("] ") + message + string_empty);
}

void Logger::info(string message)
{
    this->log(std::string("INFO"), message);
}

void Logger::warn(string message)
{
    this->log(std::string("WARN"), message);
}

void Logger::error(string message)
{
    this->log(std::string("ERROR"), message);
}

void Logger::debug(string message)
{
    this->log(std::string("DEBUG"), message);
}

std::shared_ptr<Logger> createLogger(string prefix)
{
    return std::make_shared<Logger>(prefix);
};


