#include "..settings.hpp"
#include "..types.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;

;

/**
 * Creates a Setting object from a configSetting object by omitting the 'value' property.
 *
 * @param {Omit<Setting, 'value'>} configSetting - The configSetting object to create the Setting from.
 * @returns {Setting} A new Setting object created from the provided configSetting object.
 */


/**
 * Retrieves the salt based on env variable SECRET_SALT
 *
 * @returns {string} The salt for the agent.
 */


/**
 * Common encryption  value - The string value to encrypt
 * @param {string} salt - The salt to use for encryption
 * @returns {string} - The encrypted value in 'iv:encrypted' format
 */


/**
 * Common decryption  value - The encrypted value in 'iv:encrypted' format
 * @param {string} salt - The salt to use for decryption
 * @returns {string} - The decrypted string value
 */


/**
 * Applies salt to the value of a setting
 * Only applies to secret settings with string values
 */


/**
 * Removes salt from the value of a setting
 * Only applies to secret settings with string values
 */


/**
 * Applies salt to all settings in a WorldSettings object
 */


/**
 * Removes salt from all settings in a WorldSettings object
 */


/**
 * Updates settings state in world metadata
 */
std::future<bool> updateWorldSettings(IAgentRuntime runtime, const std::string& serverId, WorldSettings worldSettings);

/**
 * Gets settings state from world metadata
 */
std::future<WorldSettings | null> getWorldSettings(IAgentRuntime runtime, const std::string& serverId);

/**
 * Initializes settings configuration for a server
 */
std::future<WorldSettings | null> initializeOnboarding(IAgentRuntime runtime, World world, OnboardingConfig config);

/**
 * Encrypts sensitive data in a Character object
 * @param {Character} character - The character object to encrypt secrets for
 * @param {IAgentRuntime} runtime - The runtime information needed for salt generation
 * @returns {Character} - A copy of the character with encrypted secrets
 */


/**
 * Decrypts sensitive data in a Character object
 * @param {Character} character - The character object with encrypted secrets
 * @param {IAgentRuntime} runtime - The runtime information needed for salt generation
 * @returns {Character} - A copy of the character with decrypted secrets
 */


/**
 * Helper  obj - Object with values to encrypt
 * @param {string} salt - The salt to use for encryption
 * @returns {Record<string, any>} - Object with encrypted values
 */


/**
 * Helper  obj - Object with encrypted values
 * @param {string} salt - The salt to use for decryption
 * @returns {Record<string, any>} - Object with decrypted values
 */


{ decryptStringValue as decryptSecret };

} // namespace elizaos
