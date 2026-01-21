#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Transaction timeout constants for CDP plugin
 * All timeouts in milliseconds
 */

/** Standard timeout for transaction confirmation (20 seconds) */

/** Extended timeout for bridge operations (2 minutes / 120 seconds) */

/** Bridge status polling interval (2 seconds) */

/** Maximum bridge polling attempts (60 × 2s = 2 minutes) */

/**
 * Wait for transaction confirmation and verify it succeeded
 * @param publicClient Viem public client
 * @param hash Transaction hash
 * @param operationType Type of operation for error messages (e.g., "swap", "transfer", "NFT transfer")
 * @param timeout Timeout in milliseconds (defaults to TX_CONFIRMATION_TIMEOUT)
 * @throws Error if transaction reverts or times out
 */
std::future<void> waitForTxConfirmation(PublicClient publicClient, `0x${string}` hash, std::string operationType = "transaction", double timeout = TX_CONFIRMATION_TIMEOUT);


} // namespace elizaos
