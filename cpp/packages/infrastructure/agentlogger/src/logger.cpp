#include "test_hybrid/src/logger.h"

Logger::Logger(std::string prefix) {
    this->prefix = prefix;
}

void Logger::log(std::string level, std::string message)
{
    auto timestamp = ((std::make_shared<Date>()))->toISOString();
    console->log(std::string("[") + timestamp + std::string("] [") + this->prefix + std::string("] [") + level + std::string("] ") + message + string_empty);
}

void Logger::info(std::string message)
{
    this->log(std::string("INFO"), message);
}

void Logger::warn(std::string message)
{
    this->log(std::string("WARN"), message);
}

void Logger::error(std::string message)
{
    this->log(std::string("ERROR"), message);
}

void Logger::debug(std::string message)
{
    this->log(std::string("DEBUG"), message);
}

std::shared_ptr<Logger> createLogger(std::string prefix)
{
    return std::make_shared<Logger>(prefix);
};


