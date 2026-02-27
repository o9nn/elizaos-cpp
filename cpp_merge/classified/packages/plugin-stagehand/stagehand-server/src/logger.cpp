#include "logger.hpp"
#include <string>

void Logger::error(string message, any error)
{
    console->error(string_empty + this->prefix + std::string(" ERROR:"), message, error);
}

