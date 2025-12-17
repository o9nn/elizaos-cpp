#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".api/shared/response-utils.hpp"
#include ".api/shared/validation.hpp"
#include "elizaos/core.hpp"
#include "jwt.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Middleware to validate that an agent exists
 */
      return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');

      return sendError(res, 404, 'NOT_FOUND', 'Agent not found');

    // Add runtime to request object for use in route handlers

/**
 * Middleware to validate UUID parameters
 */

    // Use enhanced validation for channel IDs

      // Log security event for invalid IDs
      return sendError(res, 400, 'INVALID_ID', `Invalid ${paramName} format`);

    // Add validated UUID to request params

/**
 * Enhanced channel ID validation middleware with additional security
 */

      return sendError(res, 400, 'MISSING_CHANNEL_ID', 'Channel ID is required');

      // Rate limit failed attempts to prevent brute force
      return sendError(res, 400, 'INVALID_CHANNEL_ID', 'Invalid channel ID format');

    // Store validated channel ID

/**
 * Middleware to require that the requester is a participant of the channel
 * Accepts either JWT-authenticated user (req.userId) or server-auth (X-API-KEY)
 */
    // First ensure authenticated via JWT or API key
        return sendError(res, 400, 'MISSING_CHANNEL_ID', 'Channel ID is required');

      // If server-authenticated, allow bypass (internal system calls)
        return next();

        return sendError(res, 401, 'UNAUTHORIZED', 'Authentication required');

          return sendError(res, 403, 'FORBIDDEN', 'You are not a participant of this channel');
        return sendError(res, 500, 'INTERNAL_ERROR', 'Failed to verify channel participation');

/**
 * Convenience middleware: require authenticated user or API key
 */

/**
 * Middleware to validate request content type for POST/PUT/PATCH requests
 */
    // Only validate Content-Type for methods that typically have request bodies

      // Skip validation if request has no body (Content-Length is 0 or undefined)
        return next();

      // Allow multipart for file uploads, JSON for regular API requests


} // namespace elizaos
