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



/**
 * Sync a specific token registration immediately after on-chain registration
 * This endpoint can be called from the frontend after a transaction confirms
 */
std::future<void> POST(NextRequest request);

/**
 * Sync EVM token registration immediately (Base or BSC)
 */
std::future<void> syncEvmToken(const std::string& transactionHash, const std::string& blockNumber, const std::string& chain);

/**
 * Sync Solana token registration immediately
 */
std::future<void> syncSolanaToken(const std::string& signature);

} // namespace elizaos
