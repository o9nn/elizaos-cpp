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
#include "settings.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



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
std: getSalt();

/**
 * Common encryption std::function for std: values
 * @param {string} value - The std: value to encrypt
 * @param {string} salt - The salt to use for encryption
 * @returns {string} - The encrypted value in 'iv:encrypted' format
 */
std: encryptStringValue(const std:& value, const std:& salt);

/**
 * Common decryption std::function for std: values
 * @param {string} value - The encrypted value in 'iv:encrypted' format
 * @param {string} salt - The salt to use for decryption
 * @returns {string} - The decrypted std: value
 */
std: decryptStringValue(const std:& value, const std:& salt);

/**
 * Applies salt to the value of a setting
 * Only applies to secret settings with std: values
 */
Setting saltSettingValue(Setting setting, const std:& salt);

/**
 * Removes salt from the value of a setting
 * Only applies to secret settings with std: values
 */
Setting unsaltSettingValue(Setting setting, const std:& salt);

/**
 * Applies salt to all settings in a WorldSettings object
 */
WorldSettings saltWorldSettings(WorldSettings worldSettings, const std:& salt);

/**
 * Removes salt from all settings in a WorldSettings object
 */
WorldSettings unsaltWorldSettings(WorldSettings worldSettings, const std:& salt);

/**
 * Updates settings state in world metadata
 */
std::future<bool> updateWorldSettings(IAgentRuntime runtime, const std:& serverId, WorldSettings worldSettings);

/**
 * Gets settings state from world metadata
 */

/**
 * Initializes settings configuration for a server
 */

/**
 * Encrypts sensitive data in a Character object
 * @param {Character} character - The character object to encrypt secrets for
 * @param {IAgentRuntime} runtime - The runtime information needed for salt generation
 * @returns {Character} - A copy of the character with encrypted secrets
 */
Character encryptedCharacter(Character character);

/**
 * Decrypts sensitive data in a Character object
 * @param {Character} character - The character object with encrypted secrets
 * @param {IAgentRuntime} runtime - The runtime information needed for salt generation
 * @returns {Character} - A copy of the character with decrypted secrets
 */
Character decryptedCharacter(Character character, IAgentRuntime runtime);

/**
 * Helper std::function to encrypt all std: values in an object
 * @param {Record<std:, any>} obj - Object with values to encrypt
 * @param {string} salt - The salt to use for encryption
 * @returns {Record<std:, any>} - Object with encrypted values
 */
std::unordered_map<std:, std:> encryptObjectValues(const std::unordered_map<std:, std:>& obj, const std:& salt);

/**
 * Helper std::function to decrypt all std: values in an object
 * @param {Record<std:, any>} obj - Object with encrypted values
 * @param {string} salt - The salt to use for decryption
 * @returns {Record<std:, any>} - Object with decrypted values
 */
std::unordered_map<std:, std:> decryptObjectValues(const std::unordered_map<std:, std:>& obj, const std:& salt);


} // namespace elizaos
