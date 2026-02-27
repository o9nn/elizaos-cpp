#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V2_SETTINGS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CORE_SRC_SPECS_V2_SETTINGS_H
#include "core.hpp"
#include "../../settings.h"
using coreCreateSettingFromConfig = createSettingFromConfig;
using coreDecryptedCharacter = decryptedCharacter;
using coreDecryptObjectValues = decryptObjectValues;
using coreDecryptStringValue = decryptStringValue;
using coreEncryptedCharacter = encryptedCharacter;
using coreEncryptObjectValues = encryptObjectValues;
using coreEncryptStringValue = encryptStringValue;
using coreGetSalt = getSalt;
using coreGetWorldSettings = getWorldSettings;
using coreInitializeOnboarding = initializeOnboarding;
using coreSaltSettingValue = saltSettingValue;
using coreSaltWorldSettings = saltWorldSettings;
using coreUnsaltSettingValue = unsaltSettingValue;
using coreUnsaltWorldSettings = unsaltWorldSettings;
using coreUpdateWorldSettings = updateWorldSettings;
#include "../../types.h"

std::shared_ptr<Setting> createSettingFromConfig(Omit<std::shared_ptr<Setting>, string> configSetting);

string getSalt();

string encryptStringValue(string value, string salt);

string decryptStringValue(string value, string salt);

std::shared_ptr<Setting> saltSettingValue(std::shared_ptr<Setting> setting, string salt);

std::shared_ptr<Setting> unsaltSettingValue(std::shared_ptr<Setting> setting, string salt);

std::shared_ptr<WorldSettings> saltWorldSettings(std::shared_ptr<WorldSettings> worldSettings, string salt);

std::shared_ptr<WorldSettings> unsaltWorldSettings(std::shared_ptr<WorldSettings> worldSettings, string salt);

std::shared_ptr<Promise<boolean>> updateWorldSettings(std::shared_ptr<IAgentRuntime> runtime, string serverId, std::shared_ptr<WorldSettings> worldSettings);

std::shared_ptr<Promise<any>> getWorldSettings(std::shared_ptr<IAgentRuntime> runtime, string serverId);

std::shared_ptr<Promise<any>> initializeOnboarding(std::shared_ptr<IAgentRuntime> runtime, World world, std::shared_ptr<OnboardingConfig> config);

std::shared_ptr<Character> encryptedCharacter(std::shared_ptr<Character> character);

std::shared_ptr<Character> decryptedCharacter(std::shared_ptr<Character> character, std::shared_ptr<IAgentRuntime> runtime);

Record<string, any> encryptObjectValues(Record<string, any> obj, string salt);

Record<string, any> decryptObjectValues(Record<string, any> obj, string salt);

#endif
