#include "settings.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

Setting createSettingFromConfig() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreCreateSettingFromConfig(configSetting);

}

std::string getSalt() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreGetSalt();

}

std::string encryptStringValue(const std::string& value, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreEncryptStringValue(value, salt);

}

std::string decryptStringValue(const std::string& value, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreDecryptStringValue(value, salt);

}

Setting saltSettingValue(Setting setting, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreSaltSettingValue(setting, salt);

}

Setting unsaltSettingValue(Setting setting, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreUnsaltSettingValue(setting, salt);

}

WorldSettings saltWorldSettings(WorldSettings worldSettings, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreSaltWorldSettings(worldSettings, salt);

}

WorldSettings unsaltWorldSettings(WorldSettings worldSettings, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreUnsaltWorldSettings(worldSettings, salt);

}

std::future<bool> updateWorldSettings(IAgentRuntime runtime, const std::string& serverId, WorldSettings worldSettings) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreUpdateWorldSettings(runtime, serverId, worldSettings);

}

std::future<std::optional<WorldSettings>> getWorldSettings(IAgentRuntime runtime, const std::string& serverId) {
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

std::unordered_map<std::string, std::any> encryptObjectValues(const std::unordered_map<std::string, std::any>& obj, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreEncryptObjectValues(obj, salt);

}

std::unordered_map<std::string, std::any> decryptObjectValues(const std::unordered_map<std::string, std::any>& obj, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreDecryptObjectValues(obj, salt);

}

} // namespace elizaos
