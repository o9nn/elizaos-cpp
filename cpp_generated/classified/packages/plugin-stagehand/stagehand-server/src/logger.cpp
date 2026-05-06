#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-stagehand/stagehand-server/src/logger.h"

void Logger::error(string message, any error)
{
    console->error(string_empty + this->prefix + std::string(" ERROR:"), message, error);
}

