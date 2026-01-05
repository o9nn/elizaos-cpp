#include "settings.hpp"
#include <iostream>
#include <stdexcept>
#include <cstdlib>

namespace elizaos {

Setting createSettingFromConfig(const ConfigSetting& configSetting) {
    // NOTE: Auto-converted from TypeScript - manually refined for C++
    
    Setting setting;
    setting.name = configSetting.name;
    setting.description = configSetting.description;
    setting.usageDescription = configSetting.usageDescription.value_or("");
    setting.value = std::nullopt;
    setting.required = configSetting.required;
    setting.validation = configSetting.validation;
    setting.isPublic = configSetting.isPublic.value_or(false);
    setting.secret = configSetting.secret.value_or(false);
    setting.dependsOn = configSetting.dependsOn.value_or(std::vector<std::string>());
    setting.onSetAction = configSetting.onSetAction;
    setting.visibleIf = configSetting.visibleIf;
    
    return setting;
}

std::string getSalt() {
    // NOTE: Auto-converted from TypeScript - manually refined for C++
    
    // Try to get SECRET_SALT from environment
    const char* secretSalt = std::getenv("SECRET_SALT");
    std::string salt = secretSalt ? std::string(secretSalt) : "secretsalt";
    
    if (!secretSalt) {
        std::cerr << "SECRET_SALT is not set, using default" << std::endl;
    }
    
    std::cout << "Generated salt (length hidden for security)" << std::endl;
    return salt;
}

std::string encryptStringValue(const std::string& value, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - stub implementation for C++
    // TODO: Implement proper AES-256-CBC encryption using OpenSSL or similar library
    
    if (value.empty()) {
        return value;
    }
    
    // Check if already encrypted (has "iv:encrypted" format)
    size_t colonPos = value.find(':');
    if (colonPos != std::string::npos) {
        std::string possibleIv = value.substr(0, colonPos);
        if (possibleIv.length() == 32) { // hex encoded 16 bytes
            // Likely already encrypted
            return value;
        }
    }
    
    // Stub: In production, implement proper AES-256-CBC encryption
    // For now, just mark it as encrypted
    std::cout << "WARNING: Using stub encryption - implement proper crypto" << std::endl;
    return "stub_iv:" + value;
}

std::string decryptStringValue(const std::string& value, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - stub implementation for C++
    // TODO: Implement proper AES-256-CBC decryption using OpenSSL or similar library
    
    if (value.empty()) {
        return value;
    }
    
    // Try to parse "iv:encrypted" format
    size_t colonPos = value.find(':');
    if (colonPos == std::string::npos) {
        // Not encrypted format, return as is
        return value;
    }
    
    std::string iv = value.substr(0, colonPos);
    std::string encrypted = value.substr(colonPos + 1);
    
    // Stub: In production, implement proper AES-256-CBC decryption
    // For now, just return the encrypted part if it's our stub format
    if (iv == "stub_iv") {
        return encrypted;
    }
    
    std::cout << "WARNING: Using stub decryption - implement proper crypto" << std::endl;
    return value;
}

Setting saltSettingValue(const Setting& setting, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - manually refined for C++
    
    Setting settingCopy = setting;
    
    // Only encrypt string values in secret settings
    if (setting.secret && setting.value.has_value()) {
        // Check if value is a string (would need type checking in real implementation)
        // For now, assume it can be converted to string
        std::string strValue = std::any_cast<std::string>(setting.value.value());
        settingCopy.value = encryptStringValue(strValue, salt);
    }
    
    return settingCopy;
}

Setting unsaltSettingValue(const Setting& setting, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - manually refined for C++
    
    Setting settingCopy = setting;
    
    // Only decrypt string values in secret settings
    if (setting.secret && setting.value.has_value()) {
        // Check if value is a string
        std::string strValue = std::any_cast<std::string>(setting.value.value());
        settingCopy.value = decryptStringValue(strValue, salt);
    }
    
    return settingCopy;
}

WorldSettings saltWorldSettings(const WorldSettings& worldSettings, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - manually refined for C++
    
    WorldSettings saltedSettings;
    
    for (const auto& [key, setting] : worldSettings) {
        saltedSettings[key] = saltSettingValue(setting, salt);
    }
    
    return saltedSettings;
}

WorldSettings unsaltWorldSettings(const WorldSettings& worldSettings, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - manually refined for C++
    
    WorldSettings unsaltedSettings;
    
    for (const auto& [key, setting] : worldSettings) {
        unsaltedSettings[key] = unsaltSettingValue(setting, salt);
    }
    
    return unsaltedSettings;
}

} // namespace elizaos
            return value; // Return the value (std::nullopt or nullptr);
        }

        if (typeof value == 'boolean' || typeof value == 'number') {
            //logger.debug('Value is a boolean or number, returning as is');
            return value;
        }
        if (typeof value != 'string') {
            logger.debug(`Value is not a string (type: ${typeof value}), returning as is`);
            return value;
        }

        // Split the IV and encrypted value
        const auto parts = value.split(":");
        if (parts.length != 2) {
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
        if (iv.length != 16) {
            if (iv.length) {
                logger.debug(`Invalid IV length (${iv.length}) - expected 16 bytes`);
            }
            return value; // Return the original value without decryption;
        }

        // Create key from the salt
        const auto key = crypto.createHash("sha256").update(salt).digest().slice(0, 32);

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

Setting saltSettingValue(Setting setting, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto settingCopy = { ...setting };

    // Only encrypt string values in secret settings
    if (setting.secret == true && typeof setting.value == 'string' && setting.value) {
        settingCopy.value = encryptStringValue(setting.value, salt);
    }

    return settingCopy;

}

Setting unsaltSettingValue(Setting setting, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto settingCopy = { ...setting };

    // Only decrypt string values in secret settings
    if (setting.secret == true && typeof setting.value == 'string' && setting.value) {
        settingCopy.value = decryptStringValue(setting.value, salt);
    }

    return settingCopy;

}

WorldSettings saltWorldSettings(WorldSettings worldSettings, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const WorldSettings saltedSettings = {};

    for (const int [key, setting] of Object.entries(worldSettings)) {
        saltedSettings[key] = saltSettingValue(setting, salt);
    }

    return saltedSettings;

}

WorldSettings unsaltWorldSettings(WorldSettings worldSettings, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const WorldSettings unsaltedSettings = {};

    for (const int [key, setting] of Object.entries(worldSettings)) {
        unsaltedSettings[key] = unsaltSettingValue(setting, salt);
    }

    return unsaltedSettings;

}

std::future<bool> updateWorldSettings(IAgentRuntime runtime, const std::string& serverId, WorldSettings worldSettings) {
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

std::future<std::optional<WorldSettings>> getWorldSettings(IAgentRuntime runtime, const std::string& serverId) {
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
    const auto encryptedChar = /* JSON.parse */ /* JSON.stringify */ std::string(character);
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
    const auto decryptedChar = /* JSON.parse */ /* JSON.stringify */ std::string(character);
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

std::unordered_map<std::string, std::any> encryptObjectValues(const std::unordered_map<std::string, std::any>& obj, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std::string, std::any> result = {};

    for (const int [key, value] of Object.entries(obj)) {
        if (typeof value == 'string' && value) {
            result[key] = encryptStringValue(value, salt);
            } else {
                result[key] = value;
            }
        }

        return result;

}

std::unordered_map<std::string, std::any> decryptObjectValues(const std::unordered_map<std::string, std::any>& obj, const std::string& salt) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std::string, std::any> result = {};

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
