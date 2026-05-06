#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".api/shared/validation.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * General API rate limiting middleware
 * With trust proxy set to 1, express-rate-limit automatically handles X-Forwarded-For headers
 */
    // No custom keyGenerator needed - express-rate-limit handles IP detection automatically
    // when trust proxy is set (which we set to 1 in index.ts)

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
