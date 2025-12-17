#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Custom RPC URLs - use proxy routes to keep API keys server-side

// Get absolute URL for proxy routes (needed for wagmi HTTP transport)
std::string getProxyUrl(const std::string& path);

// Determine available chains based on configuration
void getAvailableChains();

// Build transports dynamically based on available chains
void getTransports();

// Create connectors only on client side to avoid indexedDB SSR errors
// Note: WalletConnect is handled by Privy, so we only use injected connector here
// farcasterMiniApp connector is prioritized when in Farcaster context
void getConnectors();

// Wagmi configuration for Privy integration
// Privy handles wallet connection, wagmi handles contract interactions

// Export chains for UI reference

} // namespace elizaos
