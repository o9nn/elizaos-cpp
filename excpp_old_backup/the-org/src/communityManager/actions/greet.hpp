#include "elizaos/core.hpp"
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

// If a user is new to the server, greet them in the general channel
// Only available if the SHOULD_GREET_NEW_PERSONS setting is true, which should be loaded from the cache from settings

/**
 * Action to greet new users in the configured channel
 * @typedef {Object} Action
 * @property {string} name - The name of the action
 * @property {string[]} similes - Array of similar actions
 * @property {string} description - Description of the action
 * @property {Function} validate - Function to validate the action
 * @property {Function} handler - Function to handle the action
 * @property {ActionExample[][]} examples - Array of examples for the action
 */

      // Check if greeting is enabled for this server

      // Check if this is a new user join event or command to greet

      // Get greeting settings

      // Build greeting message

      // Create memory of greeting

      // Send greeting


} // namespace elizaos
