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



// Default Ollama API URL

/**
 * Validation service for Ollama plugin
 * Provides runtime validation for API availability
 */
class OllamaValidationService extends Service {
  static override serviceType: ServiceTypeName = 'ollama-validation' as ServiceTypeName;
  static serviceName = 'ollama-validation';

  override capabilityDescription = 'Validates Ollama plugin configuration and API availability';

  constructor(runtime: any) {
    super(runtime);
  }

    // No cleanup needed

  /**
   * Check if Ollama is properly configured and available
   */
      // Remove /api suffix for direct API calls

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
 * Checks for the presence of the model via the Ollama API and attempts to download it if not found. Logs progress and errors during the process.
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

        await ensureModelAvailable(runtime, modelName, baseURL);

        // Handle initialization case where params is null

          // Get the expected dimension from runtime settings or use default based on model

            // Use known dimensions for common models

            // Check if we know the dimension for this model

        // Use ollama.embedding() as shown in the docs
        // Return a fallback vector rather than crashing

        await ensureModelAvailable(runtime, model, baseURL);

        // Handle both prompt and messages format

        // Handle both prompt and messages format

        await ensureModelAvailable(runtime, model, baseURL);

        await ensureModelAvailable(runtime, model, baseURL);

        // Return empty object instead of crashing

        await ensureModelAvailable(runtime, model, baseURL);

        // Return empty object instead of crashing
              // Remove /api suffix for direct API calls

} // namespace elizaos
