#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Base character object representing Eliza - a versatile, helpful AI assistant.
 * This contains all available plugins which will be filtered based on environment.
 */

/**
 * Returns the Eliza character with plugins ordered by priority based on environment variables.
 * This should be called after environment variables are loaded.
 *
 * @returns {Character} The Eliza character with appropriate plugins for the current environment
 */
Character getElizaCharacter();

/**
 * Legacy for backward compatibility.
 * Note: This will include all plugins regardless of environment variables.
 * Use getElizaCharacter() for environment-aware plugin loading.
 */

} // namespace elizaos
