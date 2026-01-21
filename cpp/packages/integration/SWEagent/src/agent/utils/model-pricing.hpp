#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Model pricing data for cost calculation
 * Based on actual API pricing as of 2024
 */

struct ModelPricing {
    double inputCostPer1k;
    double outputCostPer1k;
    std::optional<double> maxInputTokens;
    std::optional<double> maxOutputTokens;
    std::string provider;
};

/**
 * Comprehensive model pricing database
 * Prices are in USD per 1000 tokens
 */
  // Current OpenAI Models

  // Future/Hypothetical OpenAI Models

  // Claude 4 Sonnet pricing is tiered based on prompt size.
  // For prompts ≤ 200K tokens:
  //   Input: $3 / 1K tokens, Output: $15 / 1K tokens
  // For prompts > 200K tokens:
  //   Input: $6 / 1K tokens, Output: $22.50 / 1K tokens
  // Prompt caching (not currently modeled in this structure):
  //   ≤ 200K: Write $3.75/1K, Read $0.30/1K
  //   >200K: Write $7.50/1K, Read $0.60/1K
  // This entry models the ≤ 200K tier for standard completions.
  // Claude 4.1 Opus pricing:
  //   Input: $15 / 1K tokens, Output: $75 / 1K tokens
  //   Prompt caching: Write $18.75/1K, Read $1.50/1K

  // Google Models

  // Cohere Models

  // AWS Bedrock Models (same pricing as Anthropic)

  // Azure OpenAI Models (same pricing as OpenAI)

  // Together AI Models

  // Mistral AI Models

  // Perplexity Models

  // DeepSeek Models

  // Local/Open Source Models (no cost)

/**
 * Calculate cost for a model based on token usage
 */
double calculateCost(const std::string& modelName, double inputTokens, double outputTokens);

/**
 * Get model token limits
 */
void getModelLimits(const std::string& modelName); {

    // Try to find a base model

/**
 * Check if a model supports a certain number of tokens
 */
bool isWithinTokenLimit(const std::string& modelName, double inputTokens, double outputTokens);

} // namespace elizaos
