#include "retry-cache.hpp"
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



// Cache TTL for Solana pool info (30 seconds)

// Rate limiting: delay between sequential RPC calls (ms)

// Helper to check if error is rate limit related
bool isRateLimitError(unknown error);

// Helper to delay between RPC calls
std::future<void> delay(double ms);

struct SolanaPoolInfo {
    "Raydium" | "Meteora" | "Orca" | "PumpSwap" protocol;
    std::string address;
    std::string tokenA;
    std::string tokenB;
    double liquidity;
    double tvlUsd;
    std::optional<double> priceUsd;
    "SOL" | "USDC" baseToken;
    std::optional<string; // SOL vault account (lamports)> solVault;
    std::optional<string; // Token vault account (SPL tokens)> tokenVault;
};

// PumpSwap AMM Program (same for mainnet/devnet)

// Mainnet Mints

// Devnet Mints

std::future<SolanaPoolInfo | null> findBestSolanaPool(const std::string& tokenMint, "mainnet" | "devnet" = "mainnet" cluster, std::optional<Connection> rpcConnection);

// std::future<std::vector<SolanaPoolInfo>> findMeteoraPools(Connection //   connection, PublicKey
// //   mint);

} // namespace elizaos
