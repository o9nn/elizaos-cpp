#include "config-utils.h"
#include <string>

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


boolean hasCharacterSecrets(any character)
{
    return AND((character["settings"]["secrets"]), (Object->keys(character["settings"]["secrets"])->get_length() > 0));
};


void ensureCharacterSettings(any character)
{
    if (!character["settings"]) {
        character["settings"] = object{};
    }
};


std::shared_ptr<Promise<any>> loadLocalEnvSecrets()
{
    auto envPath = std::async([=]() { getLocalEnvPath(); });
    if (!envPath) {
        return nullptr;
    }
    return std::async([=]() { parseEnvFile(envPath); });
};


std::shared_ptr<Promise<boolean>> setDefaultSecretsFromEnv(any character)
{
    ensureCharacterSettings(character);
    if (hasCharacterSecrets(character)) {
        return false;
    }
    auto envSecrets = std::async([=]() { loadLocalEnvSecrets(); });
    if (!envSecrets) {
        return false;
    }
    character["settings"]["secrets"] = envSecrets;
    return true;
};


