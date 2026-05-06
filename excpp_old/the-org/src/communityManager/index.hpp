#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".init.hpp"
#include "plugins/communityManager.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Read and convert to Base64

/**
 * Represents a character named Eli5 with specific behavior traits and message examples.
 *
 * @typedef {Object} Character
 * @property {string} name - The name of the character
 * @property {string[]} plugins - List of plugins used by the character
 * @property {Object} secrets - Object containing sensitive information for the character
 * @property {string} system - Description of the character's behavior in responding to messages
 * @property {string[]} bio - List of behaviors exhibited by the character
 * @property {Object[]} messageExamples - List of message examples with responses from the character
 * @property {Object} style - Object containing style guidelines for the character's responses
 */

/**
 * Configuration object for onboarding settings.
 * @typedef {Object} OnboardingConfig
 * @property {Object} settings - Object containing various settings for onboarding.
 * @property {Object} settings.SHOULD_GREET_NEW_PERSONS - Setting for automatically greeting new users.
 * @property {string} settings.SHOULD_GREET_NEW_PERSONS.name - The name of the setting.
 * @property {string} settings.SHOULD_GREET_NEW_PERSONS.description - The description of the setting.
 * @property {string} settings.SHOULD_GREET_NEW_PERSONS.usageDescription - The usage description of the setting.
 * @property {boolean} settings.SHOULD_GREET_NEW_PERSONS.required - Indicates if the setting is required.
 * @property {boolean} settings.SHOULD_GREET_NEW_PERSONS.public - Indicates if the setting is public.
 * @property {boolean} settings.SHOULD_GREET_NEW_PERSONS.secret - Indicates if the setting is secret.
 * @property {Function} settings.SHOULD_GREET_NEW_PERSONS.validation - The function for validating the setting value.
 * @property {Object} settings.GREETING_CHANNEL - Setting for the channel to use for greeting new users.
 * @property {string} settings.GREETING_CHANNEL.name - The name of the setting.
 * @property {string} settings.GREETING_CHANNEL.description - The description of the setting.
 * @property {string} settings.GREETING_CHANNEL.usageDescription - The usage description of the setting.
 * @property {boolean} settings.GREETING_CHANNEL.required - Indicates if the setting is required.
 * @property {boolean} settings.GREETING_CHANNEL.public - Indicates if the setting is public.
 * @property {boolean} settings.GREETING_CHANNEL.secret - Indicates if the setting is secret.
 * @property {string[]} settings.GREETING_CHANNEL.dependsOn - Array of settings that this setting depends on.
 * @property {Function} settings.GREETING_CHANNEL.onSetAction - The action to perform when the setting value is set.
 */


} // namespace elizaos
