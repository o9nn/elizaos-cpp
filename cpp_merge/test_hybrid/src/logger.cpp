#include "logger.hpp"

Logger::Logger(string prefix) {
    this->prefix = prefix;
}

void Logger::log(string level, string message)
{
    auto timestamp = ((std::make_shared<Date>()))->toISOString();
    console->log(std:("[") + timestamp + std:("] [") + this->prefix + std:("] [") + level + std:("] ") + message + string_empty);
}

void Logger::info(string message)
{
    this->log(std:("INFO"), message);
}

void Logger::warn(string message)
{
    this->log(std:("WARN"), message);
}

void Logger::error(string message)
{
    this->log(std:("ERROR"), message);
}

void Logger::debug(string message)
{
    this->log(std:("DEBUG"), message);
}

std::shared_ptr<Logger> createLogger(string prefix)
{
    return std::make_shared<Logger>(prefix);
};


