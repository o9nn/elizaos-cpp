#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// src/utils/registry/schema.ts

/**
 * Defines the possible types of plugins:
 * - "adapter"
 * - "client"
 * - "plugin"
 */
using PluginType = std::variant<std::string, std::string, std::string>;

// TODO: we should handle this better later
/**
 * Function that determines the type of plugin based on the name provided.
 * @param {string} name - The name of the plugin.
 * @returns {PluginType} The type of plugin ('adapter', 'client', or 'plugin').
 */
PluginType getPluginType(const std::string& name);

/**
 * Type definition for the Registry type which is inferred from the registrySchema
 */
using Registry = z::infer<typeof registrySchema>;

} // namespace elizaos
