#include "config-utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<RuntimeSettings> loadEnvConfig() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envInfo = UserEnvironment.getInstanceInfo();
    if (envInfo.paths.envFilePath) {
        dotenv.config({ path: envInfo.paths.envFilePath });
    }
    return process.env;

}

bool hasCharacterSecrets(const std::any& character) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return character.settings.secrets && Object.keys(character.settings.secrets).length > 0;

}

void ensureCharacterSettings(const std::any& character) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!character.settings) {
        character.settings = {}
    }

}

std::future<std::optional<std::unordered_map<std::string, std::string>>> loadLocalEnvSecrets() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto envPath = getLocalEnvPath();
    if (!envPath) {
        return nullptr;
    }
    return parseEnvFile(envPath);

}

std::future<bool> setDefaultSecretsFromEnv(const std::any& character) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Ensure settings exist
    ensureCharacterSettings(character);

    // If character already has secrets, nothing to do
    if (hasCharacterSecrets(character)) {
        return false;
    }

    // Load secrets from local .env
    const auto envSecrets = loadLocalEnvSecrets();
    if (!envSecrets) {
        return false;
    }

    // Set the secrets
    character.settings.secrets = envSecrets;
    return true;

}

} // namespace elizaos
