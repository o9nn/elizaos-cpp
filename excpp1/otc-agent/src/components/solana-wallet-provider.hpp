#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Get Solana network from unified chain config
 */
WalletAdapterNetwork getSolanaNetwork();

/**
 * Get Solana RPC endpoint - supports proxy path or full URL
 */
std::string getSolanaEndpoint();


} // namespace elizaos
