#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Default Ollama API URL

/**
 * Validation service for Ollama plugin
 * Provides runtime validation for API availability
 */
class OllamaValidationService {
public:
    OllamaValidationService(const std::any& runtime);
    std::future<OllamaValidationService> start(const std::any& runtime);
    std::future<void> stop();
    std::future<bool> isValid();
};

/**
 * Retrieves the Ollama API base URL from runtime settings.
 *
 * If the API endpoint is not set in the runtime, defaults to the standard Ollama URL.
 * The URL should include the /api path for ollama-ai-provider compatibility.
 *
 * @returns The base URL for the Ollama API.
 */

  // Ensure the URL ends with /api for ollama-ai-provider

/**
 * Ensures that the specified Ollama model is available locally, downloading it if necessary.
 *
 */
  // Remove /api suffix for direct API calls

/**
 * Converts a messages array to a prompt string
 */
std::string messagesToPrompt(const std::vector<std::any>& messages);

/**
 * Generates text from the Ollama API using the specified model and parameters.
 *
 * Returns the generated text, or an error message if generation fails.
 */
std::future<void> generateOllamaText(ReturnType<typeof createOllama> ollama, const std::string& model, std::optional<std::any> params);

/**
 * Generates an object from the Ollama API using the specified model and parameters.
 *
 * Returns the generated object, or an empty object if generation fails.
 */
std::future<void> generateOllamaObject(ReturnType<typeof createOllama> ollama, const std::string& model, ObjectGenerationParams params);

    // Register the validation service

    // Check if endpoint is configured

      // Validate Ollama API endpoint by checking if it's accessible
      // Remove /api suffix for direct API calls

        // Handle initialization case where params is null

          // Get the expected dimension from runtime settings or use default based on model

            // Use known dimensions for common models

            // Check if we know the dimension for this model

        // Use ollama.embedding() as shown in the docs
        // Return a fallback vector rather than crashing

        // Handle both prompt and messages format

        // Handle both prompt and messages format

        // Return empty object instead of crashing

        // Return empty object instead of crashing
              // Remove /api suffix for direct API calls

} // namespace elizaos
