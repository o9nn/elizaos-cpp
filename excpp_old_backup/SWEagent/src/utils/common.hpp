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
 * Common utility functions
 */

/**
 * Format duration in seconds to human readable format
 */
std::string formatDuration(double seconds);

/**
 * Truncate string to specified length
 */
std::string truncateString(const std::string& str, double maxLength);

/**
 * Calculate hash of data
 */
std::string calculateHash(unknown data);

/**
 * Retry options interface
 */
struct RetryOptions {
    std::optional<double> retries;
    std::optional<double> delay;
    std::optional<(error: Error, attempt: number) => void> onRetry;
};

/**
 * Retry an async operation
 */

/**
 * Debounce a function
 */

/**
 * Throttle a function
 */


} // namespace elizaos
