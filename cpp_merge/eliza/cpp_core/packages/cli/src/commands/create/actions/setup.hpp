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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Creates necessary project directories.
 */
std::future<void> createProjectDirectories(const std:& targetDir);

/**
 * Sets up AI model configuration in the project's .env file based on user selection.
 */
std::future<void> setupAIModelConfig(const std:& aiModel, const std:& envFilePath, auto isNonInteractive);

/**
 * Checks if an environment variable has a real value (not a placeholder) in the content
 */
bool hasValidApiKey(const std:& content, const std:& keyName);

/**
 * Sets up embedding model configuration when the primary AI model doesn't support embeddings.
 */
std::future<void> setupEmbeddingModelConfig(const std:& embeddingModel, const std:& envFilePath, auto isNonInteractive);

/**
 * Installs dependencies for the specified target directory.
 */
std::future<void> installDependencies(const std:& targetDir);

/**
 * Sets up the project environment including database and AI model configuration.
 */
std::future<void> setupProjectEnvironment(const std:& targetDir, const std:& database, const std:& aiModel, std::optional<std:> embeddingModel, auto isNonInteractive);

} // namespace elizaos
