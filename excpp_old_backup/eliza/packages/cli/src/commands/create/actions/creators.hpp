#include ".utils.hpp"
#include "setup.hpp"
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



/**
 * Creates a new plugin with the specified name and configuration.
 */
std::future<void> createPlugin(const std::string& pluginName, const std::string& targetDir, auto isNonInteractive = false);

/**
 * Creates a new agent character file with the specified name.
 */
std::future<void> createAgent(const std::string& agentName, const std::string& targetDir, auto isNonInteractive = false);

/**
 * Creates a new TEE project with the specified name and configuration.
 */
std::future<void> createTEEProject(const std::string& projectName, const std::string& targetDir, const std::string& database, const std::string& aiModel, std::optional<std::string> embeddingModel, auto isNonInteractive = false);

/**
 * Creates a new regular project with the specified name and configuration.
 */
std::future<void> createProject(const std::string& projectName, const std::string& targetDir, const std::string& database, const std::string& aiModel, std::optional<std::string> embeddingModel, auto isNonInteractive = false);

} // namespace elizaos
