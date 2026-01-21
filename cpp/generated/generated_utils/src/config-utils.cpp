#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/start/utils/config-utils.h"

std::shared_ptr<Promise<std::shared_ptr<RuntimeSettings>>> loadEnvConfig()
{
    auto envInfo = std::async([=]() { UserEnvironment->getInstanceInfo(); });
    if (envInfo->paths->envFilePath) {
        dotenv->config(object{
            object::pair{std::string("path"), envInfo->paths->envFilePath}
        });
    }
    return as<std::shared_ptr<RuntimeSettings>>(process->env);
};


