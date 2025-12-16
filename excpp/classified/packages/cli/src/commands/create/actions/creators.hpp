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
 * Handles interactive configuration setup for projects
 * This includes database configuration, AI model setup, and Ollama fallback configuration
 */
std::future<void> handleInteractiveConfiguration(const std::string& targetDir, const std::string& database, const std::string& aiModel, std::optional<std::string> embeddingModel);

/**
 * wraps the creation process with cleanup handlers that remove the directory
 * if the user interrupts with ctrl-c during installation
 */
  // Check if directory already exists before we start

    // Clean up if the directory didn't exist before and exists now
    // This handles cases where fn() created the directory but was interrupted
    // before we could set directoryCreatedByUs flag

  // store handler references for proper cleanup

  // register cleanup on process exit (handles all termination cases)

    // success - remove only our cleanup handlers

    // remove only our cleanup handlers

    // cleanup on error - if the directory didn't exist before and exists now
        // ignore cleanup errors

/**
 * Creates a new plugin with the specified name and configuration.
 */
std::future<void> createPlugin(const std::string& pluginName, const std::string& targetDir, string = 'full' pluginType, auto isNonInteractive = false);

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
