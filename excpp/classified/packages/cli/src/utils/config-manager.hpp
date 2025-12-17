#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "user-environment.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Interface for the agent's configuration
 */
struct AgentConfig {
    std::string lastUpdated;
    std::optional<bool> isDefault;
};

/**
 * Retrieves the file path to the agent's configuration file.
 *
 * @returns A promise that resolves to the absolute path of the configuration file.
 */
std::future<std::string> getConfigFilePath();

std::future<bool> fileExists(const std::string& p);

/**
 * Loads the agent configuration from disk, returning a default configuration if the file does not exist or cannot be read.
 *
 * @returns The loaded {@link AgentConfig} object, or a default configuration if loading fails.
 */
std::future<AgentConfig> loadConfig();

/**
 * Saves the agent configuration object to disk, updating its last updated timestamp.
 *
 * @param config - The agent configuration to save.
 *
 * @remark
 * If the target directory does not exist, it is created. Errors during saving are logged but not thrown.
 */
std::future<void> saveConfig(AgentConfig config);

} // namespace elizaos
