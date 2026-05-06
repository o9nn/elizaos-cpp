#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".init.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Read and convert to Base64

/**
 * Represents a character with a name and a list of plugins for various functionalities.
 * @typedef {Object} Character
 * @property {string} name - The name of the character.
 * @property {Array<string>} plugins - The list of plugins associated with the character.
 * @property {Object} secrets - The secrets related to the character.
 */

/**
 * Configuration object for onboarding settings.
 * @typedef {Object} OnboardingConfig
 * @property {Object} settings - Settings object.
 * @property {Object} settings.MONITORED_PLATFORMS - Object representing monitored platforms settings.
 * @property {string} settings.MONITORED_PLATFORMS.name - Name of the monitored platforms setting.
 * @property {string} settings.MONITORED_PLATFORMS.description - Description of the monitored platforms setting.
 * @property {boolean} settings.MONITORED_PLATFORMS.required - Indicates if the monitored platforms setting is required.
 * @property {boolean} settings.MONITORED_PLATFORMS.public - Indicates if the monitored platforms setting is public.
 * @property {boolean} settings.MONITORED_PLATFORMS.secret - Indicates if the monitored platforms setting is secret.
 * @property {string} settings.MONITORED_PLATFORMS.usageDescription - Description of how the monitored platforms setting should be used.
 * @property {Function} settings.MONITORED_PLATFORMS.validation - Validation function for the monitored platforms setting.
 */


} // namespace elizaos
