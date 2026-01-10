#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/utils/env-prompt.h"

std::shared_ptr<Promise<string>> getEnvFilePath()
{
    auto service = std::async([=]() { getEnvFileService(); });
    return service->getFilePath();
};


std::shared_ptr<Promise<Record<string, string>>> readEnvFile()
{
    auto service = std::async([=]() { getEnvFileService(); });
    return service->read();
};


std::shared_ptr<Promise<void>> writeEnvFile(Record<string, string> envVars)
{
    auto service = std::async([=]() { getEnvFileService(); });
    std::async([=]() { service->write(envVars, object{
        object::pair{std::string("preserveComments"), false}, 
        object::pair{std::string("updateProcessEnv"), true}
    }); });
    return std::shared_ptr<Promise<void>>();
};


