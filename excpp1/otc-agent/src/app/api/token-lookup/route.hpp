#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Codex GraphQL endpoint and Solana network ID

struct TokenInfo {
    std::string address;
    std::string symbol;
    std::string name;
    double decimals;
    std::optional<std::string> logoUrl;
    std::string chain;
    std::optional<double> priceUsd;
};

/**
 * Check if address looks like a Solana address (base58, 32-44 chars)
 */
bool isSolanaAddress(const std::string& address);

/**
 * Check if address looks like an EVM address (0x followed by 40 hex chars)
 */
bool isEvmAddress(const std::string& address);

/**
 * Look up Solana token via Codex API
 */

/**
 * Look up EVM token via Alchemy API
 */

/**
 * GET /api/token-lookup?address=0x...&chain=base
 * GET /api/token-lookup?address=So111...
 *
 * Looks up a single token by contract address.
 * Auto-detects chain if not provided for Solana addresses.
 */
std::future<void> GET(NextRequest request);

} // namespace elizaos
