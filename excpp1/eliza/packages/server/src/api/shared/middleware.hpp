#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"
#include "response-utils.hpp"
#include "validation.hpp"

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
 * Security middleware to add additional API protection
 */
    // Add security headers specific to API responses

    // Remove potentially sensitive headers

    // Log security-relevant information

    // Log suspicious patterns

    // Check for suspicious request patterns with safe, non-backtracking regexes

    // Use safer string matching instead of potentially dangerous regexes

    // Safe SQL injection detection without backtracking regex

    // Check for SQL injection patterns more safely

    // Check for other suspicious patterns

/**
 * Middleware to validate request content type for POST/PUT/PATCH requests
 */
    // Only validate Content-Type for methods that typically have request bodies

      // Skip validation if request has no body (Content-Length is 0 or undefined)
        return next();

      // Allow multipart for file uploads, JSON for regular API requests

/**
 * General API rate limiting middleware
 */

/**
 * Strict rate limiting for file system operations
 */

/**
 * Very strict rate limiting for upload operations
 */

/**
 * Rate limiting specifically for channel validation attempts
 * Prevents brute force attacks on channel IDs
 */
      // Skip rate limiting if channel ID is valid (successful validations)

} // namespace elizaos
