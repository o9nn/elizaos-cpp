#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Express middleware for validating API Key authentication based on an environment variable.
 *
 * If the ELIZA_SERVER_AUTH_TOKEN environment variable is set, this middleware
 * checks for a matching 'X-API-KEY' header in incoming requests.
 *
 * If the environment variable is *not* set, the middleware allows all requests
 * to pass through without authentication checks.
 *
 * @param req - Express request object.
 * @param res - Express response object.
 * @param next - Express next function.
 */
void apiKeyAuthMiddleware(Request req, Response res, NextFunction next);

} // namespace elizaos
