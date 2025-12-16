#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Validates a Solana address format
 */
bool isValidSolanaAddress(const std::string& address);

/**
 * Fetches data with retry logic and proper error handling
 */
std::future<std::any> fetchWithRetry(const std::string& url, RequestInit = {} options, const std::variant<'solana', 'base' = 'solana'>& chain, auto maxRetries = 3);

/**
 * Decodes a base58 string to Uint8Array
 */
Uint8Array decodeBase58(const std::string& str);

/**
 * Tracks analyzed tokens with timestamps
 */
struct AnalyzedToken {
    std::string address;
    double timestamp;
    std::string symbol;
};

/**
 * Manages analyzed token history
 */

} // namespace elizaos
