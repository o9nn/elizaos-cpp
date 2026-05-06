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

/**
 * Mock factory functions for testing
 */

/**
 * Creates a mock IAgentRuntime with all required properties
 */
IAgentRuntime createMockAgentRuntime(std::optional<std::optional<IAgentRuntime>> overrides);

/**
 * Creates a mock DatabaseAdapter with message server methods
 */

/**
 * Creates a mock Express Request
 */
Request createMockRequest(std::optional<std::optional<Request>> overrides);

/**
 * Creates a mock Express Response
 */
Response createMockResponse();

/**
 * Creates a mock Express NextFunction
 */
NextFunction createMockNext();

/**
 * Creates a mock Socket.IO Server
 */
void createMockSocketIO();

/**
 * Creates a mock HTTP Server
 */
void createMockHttpServer();

/**
 * Creates a mock Service
 */
Service createMockService(std::optional<std::optional<Service>> overrides);

/**
 * Creates mock multer file
 */

} // namespace elizaos
