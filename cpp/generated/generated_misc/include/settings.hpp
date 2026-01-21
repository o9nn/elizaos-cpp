#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V2_SRC_SETTINGS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V2_SRC_SETTINGS_H
#include "core.h"
#include "@elizaos/core.h"
using coreCreateSettingFromConfig = createSettingFromConfig;
using coreGetSalt = getSalt;
using coreEncryptStringValue = encryptStringValue;
using coreDecryptStringValue = decryptStringValue;
using coreSaltSettingValue = saltSettingValue;
using coreUnsaltSettingValue = unsaltSettingValue;
using coreSaltWorldSettings = saltWorldSettings;
using coreUnsaltWorldSettings = unsaltWorldSettings;
using coreUpdateWorldSettings = updateWorldSettings;
using coreGetWorldSettings = getWorldSettings;
using coreInitializeOnboarding = initializeOnboarding;
using coreEncryptedCharacter = encryptedCharacter;
using coreDecryptedCharacter = decryptedCharacter;
using coreEncryptObjectValues = encryptObjectValues;
using coreDecryptObjectValues = decryptObjectValues;

std::shared_ptr<Setting> createSettingFromConfig(Omit<std::shared_ptr<Setting>, string> configSetting);

std::string getSalt();

std::string encryptStringValue(std::string value, std::string salt);

std::string decryptStringValue(std::string value, std::string salt);

std::shared_ptr<Setting> saltSettingValue(std::shared_ptr<Setting> setting, std::string salt);

std::shared_ptr<Setting> unsaltSettingValue(std::shared_ptr<Setting> setting, std::string salt);

std::shared_ptr<WorldSettings> saltWorldSettings(std::shared_ptr<WorldSettings> worldSettings, std::string salt);

std::shared_ptr<WorldSettings> unsaltWorldSettings(std::shared_ptr<WorldSettings> worldSettings, std::string salt);

std::shared_ptr<Promise<boolean>> updateWorldSettings(std::shared_ptr<IAgentRuntime> runtime, std::string serverId, std::shared_ptr<WorldSettings> worldSettings);

std::shared_ptr<Promise<any>> getWorldSettings(std::shared_ptr<IAgentRuntime> runtime, std::string serverId);

std::shared_ptr<Promise<any>> initializeOnboarding(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<World> world, std::shared_ptr<OnboardingConfig> config);

std::shared_ptr<Character> encryptedCharacter(std::shared_ptr<Character> character);

std::shared_ptr<Character> decryptedCharacter(std::shared_ptr<Character> character, std::shared_ptr<IAgentRuntime> runtime);

Record<std::string, any> encryptObjectValues(Record<std::string, any> obj, std::string salt);

Record<std::string, any> decryptObjectValues(Record<std::string, any> obj, std::string salt);

#endif
