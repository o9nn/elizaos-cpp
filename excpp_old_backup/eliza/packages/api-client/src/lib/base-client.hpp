#include ".types/base.hpp"
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



class ApiError extends Error {
  constructor(
    public code: string,
    message: string,
    public details?: string,
    public status?: number
  ) {
    super(message);
    this.name = 'ApiError';
  }

  /**
   * Creates a safe response for no-content scenarios (204 responses)
   * Returns a sensible default based on common API patterns
   */
    // For most delete/update operations, return a success indicator
    // This handles the common case of { success: boolean } return types

    // Handle empty baseUrl for relative URLs
      // Fallback for non-browser environments

    // Add query parameters

      // Remove Content-Type header if body is FormData

      // Handle empty responses (204 No Content)
        // For 204 No Content, create a synthetic success response
          // If JSON parsing fails, treat as success for 2xx responses


} // namespace elizaos
