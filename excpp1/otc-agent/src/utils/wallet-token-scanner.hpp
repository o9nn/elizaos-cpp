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
 * Wallet token scanner utilities
 * Scans user wallets for all tokens via backend APIs
 *
 * Strategy:
 * - EVM (Base/BSC): Backend API using Alchemy
 * - Solana: Backend API using Helius
 */

struct ScannedToken {
    std::string address;
    std::string symbol;
    std::string name;
    std::string balance;
    double decimals;
    std::optional<std::string> logoUrl;
    Chain chain;
    bool isRegistered;
    std::optional<double> priceUsd;
    std::optional<double> balanceUsd;
};

/**
 * Scan wallet for ERC20 tokens using backend API
 * Works for Base, BSC, and other EVM chains
 * Requires ALCHEMY_API_KEY in environment
 */

/**
 * Scan wallet for all SPL tokens on Solana
 * Uses backend API which handles everything (balances + metadata + prices)
 */

/**
 * Get registered token addresses from database
 * Returns empty set on failure to allow scanner to continue
 */

/**
 * Scan wallet for tokens on any supported chain
 * Returns tokens with balances, metadata, prices, and registration status
 */

/**
 * Scan wallet on multiple chains simultaneously
 */

} // namespace elizaos
