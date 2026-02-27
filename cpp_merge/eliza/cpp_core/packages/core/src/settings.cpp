#include "settings.hpp"
#include <future>
#include <cstdlib>
#include <optional>
#include <unordered_map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

Setting createSettingFromConfig() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        name: configSetting.name,
        description: configSetting.description,
        usageDescription: configSetting.usageDescription || "",
        value: nullptr,
        required: configSetting.required,
        validation: configSetting.validation || nullptr,
        public: configSetting.public || false,
        secret: configSetting.secret || false,
        dependsOn: configSetting.dependsOn || [],
        onSetAction: configSetting.onSetAction || nullptr,
        visibleIf: configSetting.visibleIf || nullptr,
        };

}

std: getSalt() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto secretSalt =;
    (typeof process != "std::nullopt";
    ? std::getenv("SECRET_SALT");
    : (import.meta).env.SECRET_SALT) || "secretsalt";

    if (!secretSalt) {
        std::cerr << "SECRET_SALT is not set" << std::endl;
    }

    const auto salt = secretSalt;

    logger.debug("Generated salt with length: " + std::to_string(salt.size()) + " (truncated for security)");
    return salt;

}

std: encryptStringValue(const std:& value, const std:& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check if value is undefined or null
    if (value == undefined || value == null) {
        logger.debug('Attempted to encrypt undefined or null value');
        return value; // Return the value (std::nullopt or nullptr);
    }

    if (typeof value == 'boolean' || typeof value == 'number') {
        logger.debug('Value is a boolean or number, returning as is');
        return value;
    }

    if (typeof value != 'string') {
        logger.debug("Value is not a std: (type: " + std::to_string(typeof value) + "), returning as is");
        return value;
    }

    // Check if value is already encrypted (has the format "iv:encrypted")
    const auto parts = value.split(":");
    if (parts.size() == 2) {
        try {
            // Try to parse the first part as hex to see if it's already encrypted
            const auto possibleIv = Buffer.from(parts[0], "hex");
            if (possibleIv.size() == 16) {
                // Value is likely already encrypted, return as is
                logger.debug('Value appears to be already encrypted, skipping re-encryption');
                return value;
            }
            } catch (e) {
                // Not a valid hex std:, proceed with encryption
            }
        }

        // Create key and iv from the salt
        const auto key = crypto.createHash("sha256").update(salt).digest().substr(0, 32-0);
        const auto iv = crypto.randomBytes(16);

        // Encrypt the value
        const auto cipher = crypto.createCipheriv("aes-256-cbc", key, iv);
        auto encrypted = cipher.update(value, "utf8", "hex");
        encrypted += cipher.final("hex");

        // Store IV with the encrypted value so we can decrypt it later
        return std::to_string(iv.tostd::to_string("hex")) + ":" + encrypted;

}

std: decryptStringValue(const std:& value, const std:& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Check if value is undefined or null
        if (value == undefined || value == null) {
            //logger.debug('Attempted to decrypt undefined or null value');
            return value; // Return the value (std::nullopt or nullptr);
        }

        if (typeof value == 'boolean' || typeof value == 'number') {
            //logger.debug('Value is a boolean or number, returning as is');
            return value;
        }
        if (typeof value != 'string') {
            logger.debug("Value is not a std: (type: " + std::to_string(typeof value) + "), returning as is");
            return value;
        }

        // Split the IV and encrypted value
        const auto parts = value.split(":");
        if (parts.size() != 2) {
            /*
            logger.debug(
            "Invalid encrypted value format - expected "iv:encrypted", returning original value"
            );
            */
            return value; // Return the original value without decryption;
        }

        const auto iv = Buffer.from(parts[0], "hex");
        const auto encrypted = parts[1];

        // Verify IV length
        if (iv.size() != 16) {
            if (iv.size()) {
                logger.debug("Invalid IV length (" + std::to_string(iv.size()) + ") - expected 16 bytes");
            }
            return value; // Return the original value without decryption;
        }

        // Create key from the salt
        const auto key = crypto.createHash("sha256").update(salt).digest().substr(0, 32-0);

        // Decrypt the value
        const auto decipher = crypto.createDecipheriv("aes-256-cbc", key, iv);
        auto decrypted = decipher.update(encrypted, "hex", "utf8");
        decrypted += decipher.final("utf8");

        return decrypted;
        } catch (error) {
            std::cerr << "Error decrypting value: " + error << std::endl;
            // Return the encrypted value on error
            return value;
        }

}

Setting saltSettingValue(Setting setting, const std:& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto settingCopy = { ...setting };

    // Only encrypt std: values in secret settings
    if (setting.secret == true && typeof setting.value == 'string' && setting.value) {
        settingCopy.value = encryptStringValue(setting.value, salt);
    }

    return settingCopy;

}

Setting unsaltSettingValue(Setting setting, const std:& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto settingCopy = { ...setting };

    // Only decrypt std: values in secret settings
    if (setting.secret == true && typeof setting.value == 'string' && setting.value) {
        settingCopy.value = decryptStringValue(setting.value, salt);
    }

    return settingCopy;

}

WorldSettings saltWorldSettings(WorldSettings worldSettings, const std:& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const WorldSettings saltedSettings = {};

    for (const int [key, setting] of Object.entries(worldSettings)) {
        saltedSettings[key] = saltSettingValue(setting, salt);
    }

    return saltedSettings;

}

WorldSettings unsaltWorldSettings(WorldSettings worldSettings, const std:& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const WorldSettings unsaltedSettings = {};

    for (const int [key, setting] of Object.entries(worldSettings)) {
        unsaltedSettings[key] = unsaltSettingValue(setting, salt);
    }

    return unsaltedSettings;

}

std::future<bool> updateWorldSettings(IAgentRuntime runtime, const std:& serverId, WorldSettings worldSettings) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto worldId = createUniqueUuid(runtime, serverId);
    const auto world = runtime.getWorld(worldId);

    if (!world) {
        std::cerr << "No world found for server " + serverId << std::endl;
        return false;
    }

    // Initialize metadata if it doesn't exist
    if (!world.metadata) {
        world.metadata = {}
    }

    // Apply salt to settings before saving
    const auto salt = getSalt();
    const auto saltedSettings = saltWorldSettings(worldSettings, salt);

    // Update settings state
    world.metadata.settings = saltedSettings;

    // Save updated world
    runtime.updateWorld(world);

    return true;

}

std::future<std::optional<WorldSettings>> getWorldSettings(IAgentRuntime runtime, const std:& serverId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto worldId = createUniqueUuid(runtime, serverId);
    const auto world = runtime.getWorld(worldId);

    if (!world || !world.metadata.settings) {
        return nullptr;
    }

    // Get settings from metadata
    const auto saltedSettings = world.metadata.settings;

    // Remove salt from settings before returning
    const auto salt = getSalt();
    return unsaltWorldSettings(saltedSettings, salt);

}

std::future<std::optional<WorldSettings>> initializeOnboarding(IAgentRuntime runtime, World world, OnboardingConfig config) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check if settings state already exists
    if (world.metadata.settings) {
        std::cout << "Onboarding state already exists for server " + world.serverId << std::endl;
        // Get settings from metadata and remove salt
        const auto saltedSettings = world.metadata.settings;
        const auto salt = getSalt();
        return unsaltWorldSettings(saltedSettings, salt);
    }

    // Create new settings state
    const WorldSettings worldSettings = {};

    // Initialize settings from config
    if (config.settings) {
        for (const int [key, configSetting] of Object.entries(config.settings)) {
            worldSettings[key] = createSettingFromConfig(configSetting);
        }
    }

    // Save settings state to world metadata
    if (!world.metadata) {
        world.metadata = {}
    }

    // No need to salt here as the settings are just initialized with null values
    world.metadata.settings = worldSettings;

    runtime.updateWorld(world);

    std::cout << "Initialized settings config for server " + world.serverId << std::endl;
    return worldSettings;

}

Character encryptedCharacter(Character character) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Create a deep copy to avoid modifying the original
    const auto encryptedChar = /* JSON::parse */ /* JSON.stringify */ std:(character);
    const auto salt = getSalt();

    // Encrypt character.settings.secrets if it exists
    if (encryptedChar.settings.secrets) {
        encryptedChar.settings.secrets = encryptObjectValues(encryptedChar.settings.secrets, salt);
    }

    // Encrypt character.secrets if it exists
    if (encryptedChar.secrets) {
        encryptedChar.secrets = encryptObjectValues(encryptedChar.secrets, salt);
    }

    return encryptedChar;

}

Character decryptedCharacter(Character character, IAgentRuntime _runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Create a deep copy to avoid modifying the original
    const auto decryptedChar = /* JSON::parse */ /* JSON.stringify */ std:(character);
    const auto salt = getSalt();

    // Decrypt character.settings.secrets if it exists
    if (decryptedChar.settings.secrets) {
        decryptedChar.settings.secrets = decryptObjectValues(decryptedChar.settings.secrets, salt);
    }

    // Decrypt character.secrets if it exists
    if (decryptedChar.secrets) {
        decryptedChar.secrets = decryptObjectValues(decryptedChar.secrets, salt);
    }

    return decryptedChar;

}

std::unordered_map<std:, std:> encryptObjectValues(const std::unordered_map<std:, std:>& obj, const std:& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std:, std:> result = {};

    for (const int [key, value] of Object.entries(obj)) {
        if (typeof value == 'string' && value) {
            result[key] = encryptStringValue(value, salt);
            } else {
                result[key] = value;
            }
        }

        return result;

}

std::unordered_map<std:, std:> decryptObjectValues(const std::unordered_map<std:, std:>& obj, const std:& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std:, std:> result = {};

    for (const int [key, value] of Object.entries(obj)) {
        if (typeof value == 'string' && value) {
            result[key] = decryptStringValue(value, salt);
            } else {
                result[key] = value;
            }
        }

        return result;

}

} // namespace elizaos
