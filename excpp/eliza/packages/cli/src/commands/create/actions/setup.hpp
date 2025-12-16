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
 * Creates necessary project directories.
 */
std::future<void> createProjectDirectories(const std::string& targetDir);

/**
 * Sets up AI model configuration in the project's .env file based on user selection.
 */
std::future<void> setupAIModelConfig(const std::string& aiModel, const std::string& envFilePath, auto isNonInteractive = false);

/**
 * Checks if an environment variable has a real value (not a placeholder) in the content
 */
bool hasValidApiKey(const std::string& content, const std::string& keyName);

/**
 * Sets up embedding model configuration when the primary AI model doesn't support embeddings.
 */
std::future<void> setupEmbeddingModelConfig(const std::string& embeddingModel, const std::string& envFilePath, auto isNonInteractive = false);

/**
 * Installs dependencies for the specified target directory.
 */
std::future<void> installDependencies(const std::string& targetDir);

/**
 * Sets up the project environment including database and AI model configuration.
 */
std::future<void> setupProjectEnvironment(const std::string& targetDir, const std::string& database, const std::string& aiModel, std::optional<std::string> embeddingModel, auto isNonInteractive = false);

} // namespace elizaos
