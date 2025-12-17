#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Helper module to isolate discord.js imports and avoid node: protocol issues
 * when the package is dynamically loaded.
 */

// Re-just the types we need

// Create a lazy loader for anything that's not just a type

/**
 */
std::future<void> getDiscordJs();

/**
 * Gets the PermissionsBitField from discord.js
 */
std::future<void> getPermissionsBitField();

/**
 * Gets the SlashCommandBuilder from discord.js
 */
std::future<void> getSlashCommandBuilder();

} // namespace elizaos
