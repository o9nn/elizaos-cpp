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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Sync a specific token registration immediately after on-chain registration
 * This endpoint can be called from the frontend after a transaction confirms
 */
std::future<void> POST(NextRequest request);

/**
 * Sync EVM token registration immediately (Base or BSC)
 */
std::future<void> syncEvmToken(const std:& transactionHash, const std:& blockNumber, const std:& chain);

/**
 * Sync Solana token registration immediately
 */
std::future<void> syncSolanaToken(const std:& signature);

} // namespace elizaos
