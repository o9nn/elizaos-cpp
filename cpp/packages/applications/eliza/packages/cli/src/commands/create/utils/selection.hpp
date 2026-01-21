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
 * Returns a list of available databases for project initialization without requiring external API calls.
 */

/**
 * Gets available AI models for selection during project creation.
 */
std::vector<AIModelOption> getAvailableAIModels();

/**
 * Gets available database options for selection during project creation.
 */
std::vector<DatabaseOption> getAvailableDatabases();

/**
 * Prompts user to select a database type with interactive UI.
 */
std::future<std::string> selectDatabase();

/**
 * Prompts user to select an AI model with interactive UI.
 */
std::future<std::string> selectAIModel();

/**
 * Gets available embedding models for selection when primary AI model doesn't support embeddings.
 */
std::vector<AIModelOption> getAvailableEmbeddingModels();

/**
 * Prompts user to select an embedding model when the primary AI model doesn't support embeddings.
 */
std::future<std::string> selectEmbeddingModel();

} // namespace elizaos
