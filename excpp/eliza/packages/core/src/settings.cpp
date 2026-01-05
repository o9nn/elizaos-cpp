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
