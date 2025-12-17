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

      // Server expects X-API-KEY header; keep backward compatibility

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

      // Parse JSON response
        // If JSON parsing fails, treat as success for 2xx responses

      // Handle error responses
        // Try to extract error information from response

      // Handle successful responses
      // Check if response is wrapped in { success: true, data: ... } format
        // Response is not wrapped - return the data directly
        // This handles server endpoints like /health, /ping, /status

  /**
   * Set JWT authentication token for this service
   * Updates the Authorization header for all future requests
   * 
   * @param token JWT authentication token
   */

  /**
   * Clear JWT authentication token from this service
   * Removes the Authorization header
   */

} // namespace elizaos
