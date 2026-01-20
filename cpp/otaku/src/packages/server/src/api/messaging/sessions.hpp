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
#include <variant>
#include <vector>
#include "elizaos/core.hpp"
#include "errors/SessionErrors.hpp"
#include "middleware.hpp"
#include "utils/media-transformer.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Extended Router interface with cleanup method
 */
  /**
   * Cleanup function to properly dispose of resources
   * Should be called when the router is being destroyed or replaced
   */

/**
 * Safely parses an integer from a string with fallback
 * Handles NaN, undefined, and invalid inputs gracefully
 * @param value - The value to parse
 * @param fallback - Default value if parsing fails
 * @param min - Optional minimum value (inclusive)
 * @param max - Optional maximum value (inclusive)
 * @returns Parsed integer or fallback value
 */
double safeParseInt(const std::string& value, double fallback, std::optional<double> min, std::optional<double> max);

// Session configuration constants with safe parsing

// Session storage

// Agent-specific timeout configurations (cached from agent settings)

// Track active cleanup intervals and handlers to prevent memory leaks

/**
 * Type guard to check if an object is a valid Session
 */

/**
 * Type guard for CreateSessionRequest
 */

/**
 * Type guard for SendMessageRequest
 */

/**
 * Type guard for timeout configuration
 * Accepts numbers or strings (which will be parsed later)
 */

/**
 * Type for parsed raw message
 */
struct ParsedRawMessage {
    std::optional<std::string> thought;
    std::optional<std::vector<std::string>> actions;
    std::optional<std::string> content;
    std::optional<std::vector<std::any>> attachments;
};

// Input validation constants

/**
 * Gets the timeout configuration for an agent
 * This could be extended to fetch from agent settings/database
 */
SessionTimeoutConfig getAgentTimeoutConfig(IAgentRuntime agent);

/**
 * Merges timeout configurations with proper precedence:
 * 1. Session-specific config (highest priority)
 * 2. Agent-specific config
 * 3. Global defaults (lowest priority)
 */
SessionTimeoutConfig mergeTimeoutConfigs(std::optional<SessionTimeoutConfig> sessionConfig, std::optional<SessionTimeoutConfig> agentConfig);

/**
 * Calculates the expiration date for a session
 */

/**
 * Checks if a session should trigger a warning
 */
bool shouldWarnAboutExpiration(Session session);

/**
 * Renews a session if auto-renew is enabled
 */
bool renewSession(Session session);

/**
 * Creates session info response with calculated fields
 */
SessionInfoResponse createSessionInfoResponse(Session session);

/**
 * Validates session metadata
 */
void validateMetadata(const std::any& metadata);

/**
 * Validates message content
 */

/**
 * Express async handler wrapper to catch errors
 */
using AsyncRequestHandler = std::function<std::variant<std::future<void>, void>(express::Request, express::Response, express::NextFunction)>;

/**
 * Creates a unified sessions router for simplified messaging
 * This abstracts away the complexity of servers/channels for simple use cases
 *
 * @param agents - Map of agent IDs to runtime instances
 * @param serverInstance - The server instance for message handling
 * @returns Router with cleanup method to prevent memory leaks
 */
SessionRouter createSessionsRouter(ElizaOS elizaOS, AgentServer serverInstance);

} // namespace elizaos
