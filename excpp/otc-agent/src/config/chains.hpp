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
#include "contracts.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// String-based chain identifier for database/API (lowercase, URL-safe)
using Chain = std::string;
using ChainFamily = std::string;

struct ChainConfig {
    std::string id;
    std::string name;
    std::string rpcUrl;
    std::string explorerUrl;
    std::string name;
    std::string symbol;
    double decimals;
    std::optional<std::string> otc;
    std::optional<std::string> usdc;
    ChainFamily type;
    std::optional<ViemChain> viemChain;
    std::optional<double> chainId;
};

// Use centralized network resolution from contracts.ts

    // Hardcoded mainnet/testnet OTC addresses from deployment configs
    
    // Use deployments if available, else env vars, else hardcoded defaults

    // For mainnet, use proxy route to keep Alchemy key server-side
    // For testnet, use public Sepolia RPC

/**
 * Get chain config by identifier
 */
ChainConfig getChainConfig(Chain chain);

/**
 * Check if chain is EVM-based
 */
bool isEVMChain(Chain chain);

/**
 * Check if chain is Solana-based
 */
bool isSolanaChain(Chain chain);

/**
 * Get chain identifier from string chain ID (database format)
 */
std::optional<Chain> getChainFromId(const std::string& chainId);

/**
 * Get chain identifier from numeric chain ID (wagmi/viem format)
 */
std::optional<Chain> getChainFromNumericId(double chainId);

/**
 * Get all viem chains for wagmi configuration
 */
std::vector<ViemChain> getAllViemChains();

} // namespace elizaos
