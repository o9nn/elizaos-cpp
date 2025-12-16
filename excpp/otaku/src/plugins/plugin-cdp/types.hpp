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

;

/**
 * CDP Plugin Type Definitions
 */

/**
 * Networks supported by CDP corresponding to DEFAULT_RPC_URLS
 */
using CdpNetwork = std::variant<, "ethereum", "base", "optimism", "arbitrum", "polygon", "base-sepolia">;


const DEFAULT_RPC_URLS: Record<number, string> = {
  [mainnet.id]: "https://ethereum.publicnode.com",
  [base.id]: "https://mainnet.base.org",
  [arbitrum.id]: "https://arb1.arbitrum.io/rpc",
  [polygon.id]: "https://polygon-rpc.com",
  [baseSepolia.id]: "https://sepolia.base.org",
  [optimism.id]: "https://mainnet.optimism.io",
};


} // namespace elizaos
