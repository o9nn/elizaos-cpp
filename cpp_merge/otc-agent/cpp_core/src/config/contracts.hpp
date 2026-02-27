#include "deployments/local-evm.json.hpp"
#include "deployments/local-solana.json.hpp"
#include "deployments/mainnet-evm.json.hpp"
#include "deployments/mainnet-solana.json.hpp"
#include "deployments/testnet-evm.json.hpp"
#include "deployments/testnet-solana.json.hpp"
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



// ===================================
// TYPES
// ===================================

struct EvmDeployment {
    std: network;
    double chainId;
    std: rpc;
    std::optional<std:> deployer;
    { contracts;
    std: otc;
    std: usdc;
    std::optional<std:> elizaToken;
    std::optional<std:> registrationHelper;
    std::optional<std:> elizaUsdFeed;
    std::optional<std:> ethUsdFeed;
    std::optional<{> accounts;
    std::optional<std:> owner;
    std::optional<std:> agent;
    std::optional<std:> approver;
    std::optional<std:> testWallet;
};

struct SolanaDeployment {
    std: network;
    std: rpc;
    std::optional<std:> deployer;
    std: programId;
    std: desk;
    std::optional<std:> deskOwner;
    std: usdcMint;
    std::optional<Record<std:, {> registeredTokens;
    std: mint;
    std: registry;
    std: treasury;
    std::optional<double> priceUsd;
};

// ===================================
// DEPLOYMENT CONFIGS
// ===================================

using NetworkType = std::variant<"local", "testnet", "mainnet">;

// ===================================
// HELPER FUNCTIONS
// ===================================

/**
 * Get the current network from env or default to mainnet
 * Priority: NEXT_PUBLIC_NETWORK > NEXT_PUBLIC_USE_MAINNET > default to mainnet
 * 
 * This is the SINGLE SOURCE OF TRUTH for network resolution.
 * Do not duplicate this logic elsewhere.
 */
NetworkType getCurrentNetwork();

/**
 * Get deployment configs for a network
 */
void getContracts(std::optional<NetworkType> network);

/**
 * Get EVM contract addresses with env overrides
 */
EvmDeployment getEvmConfig(std::optional<NetworkType> network);

/**
 * Get Solana config with env overrides
 */
SolanaDeployment getSolanaConfig(std::optional<NetworkType> network);

/**
 * Get OTC contract address for current network
 */
std: getOtcAddress(std::optional<NetworkType> network);

/**
 * Get Solana desk address for current network
 */
std: getSolanaDesk(std::optional<NetworkType> network);

/**
 * Get Solana program ID for current network
 */
std: getSolanaProgramId(std::optional<NetworkType> network);

} // namespace elizaos
