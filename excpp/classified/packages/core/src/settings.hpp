#include "entities.hpp"
#include "logger.hpp"
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
;
import type {
  Character,
  IAgentRuntime,
  OnboardingConfig,
  Setting,
  World,
  WorldSettings,
} from './types';

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
;
}

/**
 * Retrieves the salt based on env variable SECRET_SALT
 *
 * @returns {string} The salt for the agent.
 */


  const salt = secretSalt;

  logger.debug(`Generated salt with length: ${salt.length} (truncated for security)`);
  return salt;
}

/**
 * Common encryption  value - The string value to encrypt
 * @param {string} salt - The salt to use for encryption
 * @returns {string} - The encrypted value in 'iv:encrypted' format
 */


  if (typeof value === 'boolean' || typeof value === 'number') {
    logger.debug('Value is a boolean or number, returning as is');
    return value;
  }

  if (typeof value !== 'string') {
    logger.debug(`Value is not a string (type: ${typeof value}), returning as is`);
    return value;
  }

  // Check if value is already encrypted (has the format "iv:encrypted")
  const parts = value.split(':');
  if (parts.length === 2) {
    try {
      // Try to parse the first part as hex to see if it's already encrypted
      const possibleIv = Buffer.from(parts[0], 'hex');
      if (possibleIv.length === 16) {
        // Value is likely already encrypted, return as is
        logger.debug('Value appears to be already encrypted, skipping re-encryption');
        return value;
      }
    } catch (e) {
      // Not a valid hex string, proceed with encryption
    }
  }

  // Create key and iv from the salt
  const key = crypto.createHash('sha256').update(salt).digest().slice(0, 32);
  const iv = crypto.randomBytes(16);

  // Encrypt the value
  const cipher = crypto.createCipheriv('aes-256-cbc', key, iv);
  let encrypted = cipher.update(value, 'utf8', 'hex');
  encrypted += cipher.final('hex');

  // Store IV with the encrypted value so we can decrypt it later
  return `${iv.toString('hex')}:${encrypted}`;
}

/**
 * Common decryption  value - The encrypted value in 'iv:encrypted' format
 * @param {string} salt - The salt to use for decryption
 * @returns {string} - The decrypted string value
 */


    if (typeof value === 'boolean' || typeof value === 'number') {
      //logger.debug('Value is a boolean or number, returning as is');
      return value;
    }
    if (typeof value !== 'string') {
      logger.debug(`Value is not a string (type: ${typeof value}), returning as is`);
      return value;
    }

    // Split the IV and encrypted value
    const parts = value.split(':');
    if (parts.length !== 2) {
      /*
      logger.debug(
        `Invalid encrypted value format - expected 'iv:encrypted', returning original value`
      );
      */
      return value; // Return the original value without decryption
    }

    const iv = Buffer.from(parts[0], 'hex');
    const encrypted = parts[1];

    // Verify IV length
    if (iv.length !== 16) {
      if (iv.length) {
        logger.debug(`Invalid IV length (${iv.length}) - expected 16 bytes`);
      }
      return value; // Return the original value without decryption
    }

    // Create key from the salt
    const key = crypto.createHash('sha256').update(salt).digest().slice(0, 32);

    // Decrypt the value
    const decipher = crypto.createDecipheriv('aes-256-cbc', key, iv);
    let decrypted = decipher.update(encrypted, 'hex', 'utf8');
    decrypted += decipher.final('utf8');

    return decrypted;
  } catch (error) {
    logger.error(`Error decrypting value: ${error}`);
    // Return the encrypted value on error
    return value;
  }
}

/**
 * Applies salt to the value of a setting
 * Only applies to secret settings with string values
 */
;

  // Only encrypt string values in secret settings
  if (setting.secret === true && typeof setting.value === 'string' && setting.value) {
    settingCopy.value = encryptStringValue(setting.value, salt);
  }

  return settingCopy;
}

/**
 * Removes salt from the value of a setting
 * Only applies to secret settings with string values
 */
;

  // Only decrypt string values in secret settings
  if (setting.secret === true && typeof setting.value === 'string' && setting.value) {
    settingCopy.value = decryptStringValue(setting.value, salt);
  }

  return settingCopy;
}

/**
 * Applies salt to all settings in a WorldSettings object
 */
;

  for (const [key, setting] of Object.entries(worldSettings)) {
    saltedSettings[key] = saltSettingValue(setting, salt);
  }

  return saltedSettings;
}

/**
 * Removes salt from all settings in a WorldSettings object
 */
;

  for (const [key, setting] of Object.entries(worldSettings)) {
    unsaltedSettings[key] = unsaltSettingValue(setting, salt);
  }

  return unsaltedSettings;
}

/**
 * Updates settings state in world metadata
 */
std::future<bool> updateWorldSettings(IAgentRuntime runtime, const std::string& serverId, WorldSettings worldSettings);`);
    return false;
  }

  // Initialize metadata if it doesn't exist
  if (!world.metadata) {
    world.metadata = {};
  }

  // Apply salt to settings before saving
  const salt = getSalt();
  const saltedSettings = saltWorldSettings(worldSettings, salt);

  // Update settings state
  world.metadata.settings = saltedSettings;

  // Save updated world
  await runtime.updateWorld(world);

  return true;
}

/**
 * Gets settings state from world metadata
 */
std::future<WorldSettings | null> getWorldSettings(IAgentRuntime runtime, const std::string& serverId);

  // Get settings from metadata
  const saltedSettings = world.metadata.settings as WorldSettings;

  // Remove salt from settings before returning
  const salt = getSalt();
  return unsaltWorldSettings(saltedSettings, salt);
}

/**
 * Initializes settings configuration for a server
 */
std::future<WorldSettings | null> initializeOnboarding(IAgentRuntime runtime, World world, OnboardingConfig config);`);
    // Get settings from metadata and remove salt
    const saltedSettings = world.metadata.settings as WorldSettings;
    const salt = getSalt();
    return unsaltWorldSettings(saltedSettings, salt);
  }

  // Create new settings state
  const worldSettings: WorldSettings = {};

  // Initialize settings from config
  if (config.settings) {
    for (const [key, configSetting] of Object.entries(config.settings)) {
      worldSettings[key] = createSettingFromConfig(configSetting);
    }
  }

  // Save settings state to world metadata
  if (!world.metadata) {
    world.metadata = {};
  }

  // No need to salt here as the settings are just initialized with null values
  world.metadata.settings = worldSettings;

  await runtime.updateWorld(world);

  logger.info(`Initialized settings config for server ${world.serverId}`);
  return worldSettings;
}

/**
 * Encrypts sensitive data in a Character object
 * @param {Character} character - The character object to encrypt secrets for
 * @returns {Character} - A copy of the character with encrypted secrets
 */


  // Encrypt character.secrets if it exists
  if (encryptedChar.secrets) {
    encryptedChar.secrets = encryptObjectValues(encryptedChar.secrets, salt);
  }

  return encryptedChar;
}

/**
 * Decrypts sensitive data in a Character object
 * @param {Character} character - The character object with encrypted secrets
 * @param {IAgentRuntime} runtime - The runtime information needed for salt generation
 * @returns {Character} - A copy of the character with decrypted secrets
 */


  // Decrypt character.secrets if it exists
  if (decryptedChar.secrets) {
    decryptedChar.secrets = decryptObjectValues(decryptedChar.secrets, salt);
  }

  return decryptedChar;
}

/**
 * Helper  obj - Object with values to encrypt
 * @param {string} salt - The salt to use for encryption
 * @returns {Record<string, any>} - Object with encrypted values
 */
;

  for (const [key, value] of Object.entries(obj)) {
    if (typeof value === 'string' && value) {
      result[key] = encryptStringValue(value, salt);
    } else {
      result[key] = value;
    }
  }

  return result;
}

/**
 * Helper  obj - Object with encrypted values
 * @param {string} salt - The salt to use for decryption
 * @returns {Record<string, any>} - Object with decrypted values
 */
;

  for (const [key, value] of Object.entries(obj)) {
    if (typeof value === 'string' && value) {
      result[key] = decryptStringValue(value, salt);
    } else {
      result[key] = value;
    }
  }

  return result;
}

{ decryptStringValue as decryptSecret };

} // namespace elizaos
