#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/commands/env/utils/file-operations.h"

std::shared_ptr<Promise<string>> getGlobalEnvPath()
{
    auto service = std::async([=]() { getEnvFileService(); });
    return service->getFilePath();
};


std::shared_ptr<Promise<any>> getLocalEnvPath()
{
    auto localEnvPath = path->join(process->cwd(), std::string(".env"));
    return (existsSync(localEnvPath)) ? any(localEnvPath) : any(nullptr);
};


std::shared_ptr<Promise<EnvVars>> parseEnvFile(string filePath)
{
    auto service = createEnvFileService(filePath);
    return service->read();
};


std::shared_ptr<Promise<void>> writeEnvFile(string filePath, EnvVars envVars)
{
    auto service = createEnvFileService(filePath);
    std::async([=]() { service->write(envVars, object{
        object::pair{std::string("preserveComments"), true}, 
        object::pair{std::string("updateProcessEnv"), true}
    }); });
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<boolean>> resetEnvFile(string filePath)
{
    try
    {
        if (!existsSync(filePath)) {
            return false;
        }
        auto service = createEnvFileService(filePath);
        auto envVars = std::async([=]() { service->read(); });
        if (Object->keys(envVars)->get_length() == 0) {
            return false;
        }
        auto resetVars = Object->keys(envVars)->reduce([=](auto acc, auto key) mutable
        {
            acc[key] = string_empty;
            return acc;
        }
        , as<EnvVars>(object{}));
        std::async([=]() { service->write(resetVars, object{
            object::pair{std::string("preserveComments"), true}, 
            object::pair{std::string("updateProcessEnv"), false}
        }); });
        return true;
    }
    catch (const any& error)
    {
        console->error(std::string("Error resetting environment file: ") + (is<Error>(error)) ? error->message : String(error) + string_empty);
        return false;
    }
};


