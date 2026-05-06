#pragma once
#include <algorithm>
#include <any>
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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Configure transformers.js
// Disable image processing to avoid sharp dependency

struct EmbeddingState {
    std::optional<std::any> pipeline;
    std::string modelName;
    double dimensions;
};

// Model configurations with their dimensions

using ModelName = keyof typeof MODEL_CONFIGS;

/**
 * Get the configured embedding model name
 */
ModelName getEmbeddingModelName(IAgentRuntime runtime);

/**
 * Initialize the embedding pipeline if not already initialized
 */
std::future<void> initializePipeline(ModelName modelName);

/**
 * Process embedding parameters to extract text input
 */
std::string extractTextFromParams(const std::optional<TextEmbeddingParams>& params);

/**
 * Generate embeddings for a single text input
 */

/**
 * Validate that the local embedding plugin is working correctly
 */
std::future<bool> validateEmbeddingPlugin(IAgentRuntime runtime);

/**
 * Local embedding plugin using transformers.js
 */

    // Pre-initialize the model

    // Validate the plugin is working


} // namespace elizaos
