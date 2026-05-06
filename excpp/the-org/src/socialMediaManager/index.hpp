#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "actions/post.hpp"
#include "init.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Read and convert to Base64

/**
 * Represents a character with specific attributes and behaviors for social media management.
 *
 * @typedef {Object} Character
 * @property {string} name - The name of the character.
 * @property {string[]} plugins - The list of plugins used by the character.
 * @property {Object} secrets - Object containing sensitive information like application ID and API token.
 * @property {Object} settings - Object containing various settings for the character.
 * @property {string} system - Description of the character's role and approach in social media management.
 * @property {string[]} bio - List of characteristics and beliefs of the character.
 * @property {Object[]} messageExamples - Examples of interactions with other individuals for messaging guidance.
 * @property {string[]} postExamples - Examples of post messages that the character would use.
 * @property {Object} style - Object containing guidelines for communication style in different scenarios.
 * @property {string[]} topics - List of topics related to the character's expertise.
 */

/**
 * @typedef {Object} OnboardingConfig
 * @property {string} settings.ORG_NAME - Organization Name setting.
 * @property {string} settings.ORG_DESCRIPTION - Organization Description setting.
 * @property {string} settings.ORG_STYLE - Brand Style setting.
 * @property {string} settings.TWITTER_USERNAME - Twitter Username setting.
 * @property {string} settings.TWITTER_EMAIL - Twitter Email setting.
 * @property {string} settings.TWITTER_PASSWORD - Twitter Password setting.
 * @property {string} settings.TWITTER_2FA_SECRET - Twitter 2FA Secret setting.
 * @property {string} settings.ANNOUNCEMENTS_CHANNELS - Announcements Channels setting.
 */
    // Basic Auth Settings
    // array of announcements channels on different platforms, specifically telegram, discord, slack


} // namespace elizaos
