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
#include "setup.hpp"
#include "utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Creates a new plugin with the specified name and configuration.
 */
std::future<void> createPlugin(const std:& pluginName, const std:& targetDir, auto isNonInteractive);

/**
 * Creates a new agent character file with the specified name.
 */
std::future<void> createAgent(const std:& agentName, const std:& targetDir, auto isNonInteractive);

/**
 * Creates a new TEE project with the specified name and configuration.
 */
std::future<void> createTEEProject(const std:& projectName, const std:& targetDir, const std:& database, const std:& aiModel, std::optional<std:> embeddingModel, auto isNonInteractive);

/**
 * Creates a new regular project with the specified name and configuration.
 */
std::future<void> createProject(const std:& projectName, const std:& targetDir, const std:& database, const std:& aiModel, std::optional<std:> embeddingModel, auto isNonInteractive);

} // namespace elizaos
