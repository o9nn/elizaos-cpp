#include ".lib/base-client.hpp"
#include ".types/audio.hpp"
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



class AudioService extends BaseApiClient {
  /**
   * Make a binary request using BaseApiClient infrastructure
   */
  private async requestBinary(
    method: string,
    path: string,
    options?: {
      body?: any;
      params?: Record<string, any>;
      headers?: Record<string, string>;
    }
    // Handle empty baseUrl for relative URLs
      // Fallback for non-browser environments

    // Add query parameters

      // Remove Content-Type header if body is FormData

  /**
   * Convert audio input to appropriate FormData value
   */

      // Handle base64 data URLs (e.g., "data:audio/mp3;base64,...")

      // Handle plain base64 strings (try to decode)
          // If base64 decoding fails, treat as file path or other string

      // For file paths or other strings, return as-is (server will handle file reading)

    // Handle Buffer and ArrayBuffer types

    // Cast to any for runtime type checking since TypeScript can't narrow the union type properly

      // Handle typed arrays like Uint8Array

  /**
   * Check if a string appears to be base64 encoded
   */
    // Basic base64 pattern check (allows padding)

    // Must be at least 4 characters and divisible by 4 (with padding)

  /**
   * Safe check for Buffer type (works in both Node.js and browser environments)
   */

  /**
   * Handle speech conversation
   */

      // String (file path or other string identifier)

  /**
   * Generate speech from text
   */
    // Get the binary audio data using BaseApiClient infrastructure

    // Convert to base64

    // Default format (server should ideally return this in a header)

  /**
   * Synthesize audio message
   */

  /**
   * Transcribe audio to text
   */

      // String (file path or other string identifier)

  /**
   * Process speech input
   */

      // String (file path or other string identifier)


} // namespace elizaos
