#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/src/settings.h"

std::shared_ptr<Setting> createSettingFromConfig(Omit<std::shared_ptr<Setting>, string> configSetting)
{
    return coreCreateSettingFromConfig(configSetting);
};


std::string getSalt()
{
    return coreGetSalt();
};


std::string encryptStringValue(std::string value, std::string salt)
{
    return coreEncryptStringValue(value, salt);
};


std::string decryptStringValue(std::string value, std::string salt)
{
    return coreDecryptStringValue(value, salt);
};


std::shared_ptr<Setting> saltSettingValue(std::shared_ptr<Setting> setting, std::string salt)
{
    return coreSaltSettingValue(setting, salt);
};


std::shared_ptr<Setting> unsaltSettingValue(std::shared_ptr<Setting> setting, std::string salt)
{
    return coreUnsaltSettingValue(setting, salt);
};


std::shared_ptr<WorldSettings> saltWorldSettings(std::shared_ptr<WorldSettings> worldSettings, std::string salt)
{
    return coreSaltWorldSettings(worldSettings, salt);
};


std::shared_ptr<WorldSettings> unsaltWorldSettings(std::shared_ptr<WorldSettings> worldSettings, std::string salt)
{
    return coreUnsaltWorldSettings(worldSettings, salt);
};


std::shared_ptr<Promise<boolean>> updateWorldSettings(std::shared_ptr<IAgentRuntime> runtime, std::string serverId, std::shared_ptr<WorldSettings> worldSettings)
{
    return coreUpdateWorldSettings(runtime, serverId, worldSettings);
};


std::shared_ptr<Promise<any>> getWorldSettings(std::shared_ptr<IAgentRuntime> runtime, std::string serverId)
{
    return coreGetWorldSettings(runtime, serverId);
};


std::shared_ptr<Promise<any>> initializeOnboarding(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<World> world, std::shared_ptr<OnboardingConfig> config)
{
    return coreInitializeOnboarding(runtime, world, config);
};


std::shared_ptr<Character> encryptedCharacter(std::shared_ptr<Character> character)
{
    return coreEncryptedCharacter(character);
};


std::shared_ptr<Character> decryptedCharacter(std::shared_ptr<Character> character, std::shared_ptr<IAgentRuntime> runtime)
{
    return coreDecryptedCharacter(character, runtime);
};


Record<std::string, any> encryptObjectValues(Record<std::string, any> obj, std::string salt)
{
    return coreEncryptObjectValues(obj, salt);
};


Record<std::string, any> decryptObjectValues(Record<std::string, any> obj, std::string salt)
{
    return coreDecryptObjectValues(obj, salt);
};


