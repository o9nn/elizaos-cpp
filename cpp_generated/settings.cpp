#include "/home/runner/work/elizaos-cpp/elizaos-cpp/plugin-specification/core-plugin-v2/src/settings.h"

std::shared_ptr<Setting> createSettingFromConfig(Omit<std::shared_ptr<Setting>, string> configSetting)
{
    return coreCreateSettingFromConfig(configSetting);
};


string getSalt()
{
    return coreGetSalt();
};


string encryptStringValue(string value, string salt)
{
    return coreEncryptStringValue(value, salt);
};


string decryptStringValue(string value, string salt)
{
    return coreDecryptStringValue(value, salt);
};


std::shared_ptr<Setting> saltSettingValue(std::shared_ptr<Setting> setting, string salt)
{
    return coreSaltSettingValue(setting, salt);
};


std::shared_ptr<Setting> unsaltSettingValue(std::shared_ptr<Setting> setting, string salt)
{
    return coreUnsaltSettingValue(setting, salt);
};


std::shared_ptr<WorldSettings> saltWorldSettings(std::shared_ptr<WorldSettings> worldSettings, string salt)
{
    return coreSaltWorldSettings(worldSettings, salt);
};


std::shared_ptr<WorldSettings> unsaltWorldSettings(std::shared_ptr<WorldSettings> worldSettings, string salt)
{
    return coreUnsaltWorldSettings(worldSettings, salt);
};


std::shared_ptr<Promise<boolean>> updateWorldSettings(std::shared_ptr<IAgentRuntime> runtime, string serverId, std::shared_ptr<WorldSettings> worldSettings)
{
    return coreUpdateWorldSettings(runtime, serverId, worldSettings);
};


std::shared_ptr<Promise<any>> getWorldSettings(std::shared_ptr<IAgentRuntime> runtime, string serverId)
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


Record<string, any> encryptObjectValues(Record<string, any> obj, string salt)
{
    return coreEncryptObjectValues(obj, salt);
};


Record<string, any> decryptObjectValues(Record<string, any> obj, string salt)
{
    return coreDecryptObjectValues(obj, salt);
};


