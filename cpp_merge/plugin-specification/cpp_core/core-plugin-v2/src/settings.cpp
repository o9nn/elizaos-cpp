#include "settings.hpp"
#include <future>
#include <optional>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

Setting createSettingFromConfig(Omit<Setting, "value"> configSetting) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreCreateSettingFromConfig(configSetting);

}

std: getSalt() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreGetSalt();

}

std: encryptStringValue(const std:& value, const std:& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreEncryptStringValue(value, salt);

}

std: decryptStringValue(const std:& value, const std:& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreDecryptStringValue(value, salt);

}

Setting saltSettingValue(Setting setting, const std:& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreSaltSettingValue(setting, salt);

}

Setting unsaltSettingValue(Setting setting, const std:& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreUnsaltSettingValue(setting, salt);

}

WorldSettings saltWorldSettings(WorldSettings worldSettings, const std:& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreSaltWorldSettings(worldSettings, salt);

}

WorldSettings unsaltWorldSettings(WorldSettings worldSettings, const std:& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreUnsaltWorldSettings(worldSettings, salt);

}

std::future<bool> updateWorldSettings(IAgentRuntime runtime, const std:& serverId, WorldSettings worldSettings) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreUpdateWorldSettings(runtime, serverId, worldSettings);

}

std::future<std::optional<WorldSettings>> getWorldSettings(IAgentRuntime runtime, const std:& serverId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreGetWorldSettings(runtime, serverId);

}

std::future<std::optional<WorldSettings>> initializeOnboarding(IAgentRuntime runtime, World world, OnboardingConfig config) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreInitializeOnboarding(runtime, world, config);

}

Character encryptedCharacter(Character character) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreEncryptedCharacter(character);

}

Character decryptedCharacter(Character character, IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreDecryptedCharacter(character, runtime);

}

std::unordered_map<std:, std:> encryptObjectValues(const std::unordered_map<std:, std:>& obj, const std:& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreEncryptObjectValues(obj, salt);

}

std::unordered_map<std:, std:> decryptObjectValues(const std::unordered_map<std:, std:>& obj, const std:& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreDecryptObjectValues(obj, salt);

}

} // namespace elizaos
