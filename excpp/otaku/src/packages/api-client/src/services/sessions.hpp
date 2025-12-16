#include ".lib/base-client.hpp"
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



/**
 * Query parameters for session messages API
 */
struct SessionMessageQueryParams {
    std::optional<std::string> limit;
    std::optional<std::string> before;
    std::optional<std::string> after;
};

/**
 * Validates and converts a date parameter to timestamp string
 * @param value Date, string, or number to convert
 * @param paramName Name of the parameter for error messages
 * @returns Timestamp string or undefined if value is invalid
 */

/**
 * Validates required parameters
 * @param value Parameter value to validate
 * @param paramName Name of the parameter for error messages
 * @throws Error if the parameter is invalid
 */

/**
 * Service for managing messaging sessions between users and agents
 */
class SessionsService extends BaseApiClient {
  /**
   * Get health status of the sessions service
   * @returns Health check response
   */
  async checkHealth(): Promise<SessionsHealthResponse> {
    return this.get<SessionsHealthResponse>('/api/messaging/sessions/health');
  }

  /**
   * Create a new messaging session
   * @param params Session creation parameters
   * @returns Created session response
   */

  /**
   * Get session details
   * @param sessionId Session ID
   * @returns Session information
   */

  /**
   * Send a message in a session
   * @param sessionId Session ID
   * @param params Message parameters
   * @returns Message response
   */

  /**
   * Get messages from a session
   * @param sessionId Session ID
   * @param params Query parameters for pagination and filtering
   * @returns Messages response
   */

    // Convert date parameters with validation

  /**
   * Delete a session
   * @param sessionId Session ID
   * @returns Success response
   */

  /**
   * List all active sessions (admin endpoint)
   * @returns List of active sessions
   */

} // namespace elizaos
