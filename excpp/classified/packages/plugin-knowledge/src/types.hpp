#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Schema for validating model configuration
  // Provider configuration
  // NOTE: If EMBEDDING_PROVIDER is not specified, the plugin automatically assumes
  // plugin-openai is being used and will use OPENAI_EMBEDDING_MODEL and
  // OPENAI_EMBEDDING_DIMENSIONS for configuration

  // API keys

  // Base URLs (optional for most providers)

  // Model names

  // Token limits

  // Embedding dimension
  // For OpenAI: Only applies to text-embedding-3-small and text-embedding-3-large models
  // Default: 768 dimensions

  // config setting

  // Contextual Knowledge settings

using ModelConfig = z::infer<typeof ModelConfigSchema>;

/**
 * Interface for provider rate limits
 */
struct ProviderRateLimits {
    double maxConcurrentRequests;
    double requestsPerMinute;
    std::optional<double> tokensPerMinute;
    std::string provider;
};

/**
 * Options for text generation overrides
 */
struct TextGenerationOptions {
    std::optional<std::string> provider;
    std::optional<std::string> modelName;
    std::optional<double> maxTokens;
    std::optional<std::string> cacheDocument;
    std::string type;
    std::optional<bool> autoCacheContextualRetrieval;
};

/**
 * Options for adding knowledge to the system
 */
struct AddKnowledgeOptions {
    std::optional<UUID> agentId;
    UUID worldId;
    UUID roomId;
    UUID entityId;
    UUID clientDocumentId;
    std::string contentType;
    std::string originalFilename;
    std::string content;
};

// Extend the core service types with knowledge service
  struct ServiceTypeRegistry {
    std::string KNOWLEDGE;
};

// Export service type constant

// Re-the unified DocumentMetadata from core

struct KnowledgeConfig {
    bool CTX_KNOWLEDGE_ENABLED;
    bool LOAD_DOCS_ON_STARTUP;
    std::optional<std::variant<std::string, double>> MAX_INPUT_TOKENS;
    std::optional<std::variant<std::string, double>> MAX_OUTPUT_TOKENS;
    std::optional<std::string> EMBEDDING_PROVIDER;
    std::optional<std::string> TEXT_PROVIDER;
    std::optional<std::string> TEXT_EMBEDDING_MODEL;
};

struct LoadResult {
    double successful;
    double failed;
};


} // namespace elizaos
