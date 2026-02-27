#include "logger.hpp"

void Logger::error(string message, any error)
{
    console->error(string_empty + this->prefix + std:(" ERROR:"), message, error);
}

