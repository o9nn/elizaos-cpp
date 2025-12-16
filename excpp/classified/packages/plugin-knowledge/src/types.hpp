#include "elizaos/core.hpp"
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

;
;

// Schema for validating model configuration
const ModelConfigSchema = z.object({
  // Provider configuration
  // NOTE: If EMBEDDING_PROVIDER is not specified, the plugin automatically assumes
  // plugin-openai is being used and will use OPENAI_EMBEDDING_MODEL and
  // OPENAI_EMBEDDING_DIMENSIONS for configuration
  EMBEDDING_PROVIDER: z.enum(['openai', 'google', 'ollama']).optional(),
  TEXT_PROVIDER: z.enum(['openai', 'anthropic', 'openrouter', 'google', 'ollama']).optional(),

  // API keys
  OPENAI_API_KEY: z.string().optional(),
  ANTHROPIC_API_KEY: z.string().optional(),
  OPENROUTER_API_KEY: z.string().optional(),
  GOOGLE_API_KEY: z.string().optional(),
  OLLAMA_API_KEY: z.string().optional(),

  // Base URLs (optional for most providers)
  OPENAI_BASE_URL: z.string().optional(),
  ANTHROPIC_BASE_URL: z.string().optional(),
  OPENROUTER_BASE_URL: z.string().optional(),
  GOOGLE_BASE_URL: z.string().optional(),
  OLLAMA_BASE_URL: z.string().optional(),
  OLLAMA_API_ENDPOINT: z.string().optional(),

  // Model names
  TEXT_EMBEDDING_MODEL: z.string(),
  TEXT_MODEL: z.string().optional(),

  // Token limits
  MAX_INPUT_TOKENS: z
    .string()
    .or(z.number())
    .transform((val) => (typeof val === 'string' ? parseInt(val, 10) : val)),
  MAX_OUTPUT_TOKENS: z
    .string()
    .or(z.number())
    .optional()
    .transform((val) => (val ? (typeof val === 'string' ? parseInt(val, 10) : val) : 4096)),

  // Embedding dimension
  // For OpenAI: Only applies to text-embedding-3-small and text-embedding-3-large models
  // Default: 768 dimensions
  EMBEDDING_DIMENSION: z
    .string()
    .or(z.number())
    .optional()
    .transform((val) => (val ? (typeof val === 'string' ? parseInt(val, 10) : val) : 768)),

  // config setting
  LOAD_DOCS_ON_STARTUP: z.boolean().default(false),

  // Contextual Knowledge settings
  CTX_KNOWLEDGE_ENABLED: z.boolean().default(false),
});

using ModelConfig = z.infer<typeof ModelConfigSchema>;

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
    std::optional<'anthropic' | 'openai' | 'openrouter' | 'google' | 'ollama'> provider;
    std::optional<std::string> modelName;
    std::optional<double> maxTokens;
    std::optional<std::string> cacheDocument;
    std::optional<{> cacheOptions;
    'ephemeral' type;
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
    std::optional<std::unordered_map<std::string, unknown>> metadata;
};


// Extend the core service types with knowledge service
declare module '@elizaos/core' {
  struct ServiceTypeRegistry {
    'knowledge' KNOWLEDGE;
};

}

// Export service type constant
const KnowledgeServiceType = {
  KNOWLEDGE: 'knowledge' as const,
} satisfies Partial<import('@elizaos/core').ServiceTypeRegistry>;

// Re-the unified DocumentMetadata from core
type { DocumentMetadata } from '@elizaos/core';

struct KnowledgeConfig {
    bool CTX_KNOWLEDGE_ENABLED;
    bool LOAD_DOCS_ON_STARTUP;
    std::optional<string | number> MAX_INPUT_TOKENS;
    std::optional<string | number> MAX_OUTPUT_TOKENS;
    std::optional<std::string> EMBEDDING_PROVIDER;
    std::optional<std::string> TEXT_PROVIDER;
    std::optional<std::string> TEXT_EMBEDDING_MODEL;
};


struct LoadResult {
    double successful;
    double failed;
    std::optional<std::vector<{ filename: string; error: string }>> errors;
};


} // namespace elizaos
