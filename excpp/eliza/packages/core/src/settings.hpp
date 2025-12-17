#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "entities.hpp"
#include "logger.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Creates a new Setting object based on provided config settings.
 * @param {Omit<Setting, "value">} configSetting - The configuration settings for the new Setting object.
 * @returns {Setting} - The newly created Setting object.
 */
/**
 * Creates a Setting object from a configSetting object by omitting the 'value' property.
 *
 * @param {Omit<Setting, 'value'>} configSetting - The configSetting object to create the Setting from.
 * @returns {Setting} A new Setting object created from the provided configSetting object.
 */
Setting createSettingFromConfig();

/**
 * Retrieves the salt based on env variable SECRET_SALT
 *
 * @returns {string} The salt for the agent.
 */
std::string getSalt();

/**
 * Common encryption function for string values
 * @param {string} value - The string value to encrypt
 * @param {string} salt - The salt to use for encryption
 * @returns {string} - The encrypted value in 'iv:encrypted' format
 */
std::string encryptStringValue(const std::string& value, const std::string& salt);

/**
 * Common decryption function for string values
 * @param {string} value - The encrypted value in 'iv:encrypted' format
 * @param {string} salt - The salt to use for decryption
 * @returns {string} - The decrypted string value
 */
std::string decryptStringValue(const std::string& value, const std::string& salt);

/**
 * Applies salt to the value of a setting
 * Only applies to secret settings with string values
 */
Setting saltSettingValue(Setting setting, const std::string& salt);

/**
 * Removes salt from the value of a setting
 * Only applies to secret settings with string values
 */
Setting unsaltSettingValue(Setting setting, const std::string& salt);

/**
 * Applies salt to all settings in a WorldSettings object
 */
WorldSettings saltWorldSettings(WorldSettings worldSettings, const std::string& salt);

/**
 * Removes salt from all settings in a WorldSettings object
 */
WorldSettings unsaltWorldSettings(WorldSettings worldSettings, const std::string& salt);

/**
 * Updates settings state in world metadata
 */
std::future<bool> updateWorldSettings(IAgentRuntime runtime, const std::string& serverId, WorldSettings worldSettings);

/**
 * Gets settings state from world metadata
 */

/**
 * Initializes settings configuration for a server
 */

/**
 * Encrypts sensitive data in a Character object
 * @param {Character} character - The character object to encrypt secrets for
 * @returns {Character} - A copy of the character with encrypted secrets
 */
Character encryptedCharacter(Character character);

/**
 * Decrypts sensitive data in a Character object
 * @param {Character} character - The character object with encrypted secrets
 * @param {IAgentRuntime} runtime - The runtime information needed for salt generation
 * @returns {Character} - A copy of the character with decrypted secrets
 */
Character decryptedCharacter(Character character, IAgentRuntime _runtime);

/**
 * Helper function to encrypt all string values in an object
 * @param {Record<string, any>} obj - Object with values to encrypt
 * @param {string} salt - The salt to use for encryption
 * @returns {Record<string, any>} - Object with encrypted values
 */
std::unordered_map<std::string, std::any> encryptObjectValues(const std::unordered_map<std::string, std::any>& obj, const std::string& salt);

/**
 * Helper function to decrypt all string values in an object
 * @param {Record<string, any>} obj - Object with encrypted values
 * @param {string} salt - The salt to use for decryption
 * @returns {Record<string, any>} - Object with decrypted values
 */
std::unordered_map<std::string, std::any> decryptObjectValues(const std::unordered_map<std::string, std::any>& obj, const std::string& salt);


} // namespace elizaos
