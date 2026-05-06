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
 * Retry utility for handling flaky browser operations
 */

struct RetryConfig {
    double maxAttempts;
    double initialDelay;
    double maxDelay;
    double backoffMultiplier;
};

  // Support both maxRetries and maxAttempts


} // namespace elizaos
