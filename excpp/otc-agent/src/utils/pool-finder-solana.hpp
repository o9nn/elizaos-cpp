#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "retry-cache.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Cache TTL for Solana pool info (30 seconds)

// Rate limiting: delay between sequential RPC calls (ms)

// Helper to check if error is rate limit related
bool isRateLimitError(const std::any& error);

// Helper to delay between RPC calls
std::future<void> delay(double ms);

struct SolanaPoolInfo {
    std::string protocol;
    std::string address;
    std::string tokenA;
    std::string tokenB;
    double liquidity;
    double tvlUsd;
    std::optional<double> priceUsd;
    std::string baseToken;
    std::optional<std::string> solVault;
    std::optional<std::string> tokenVault;
};

// PumpSwap AMM Program (same for mainnet/devnet)

// Mainnet Mints

// Devnet Mints

// std::future<std::vector<SolanaPoolInfo>> findMeteoraPools();

} // namespace elizaos
