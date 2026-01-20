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



// =============================================================================
// TYPES
// =============================================================================

struct EvmDeployment {
    std::string network;
    double chainId;
    std::string rpc;
    std::optional<std::string> deployer;
    { contracts;
    std::string otc;
    std::string usdc;
    std::optional<std::string> elizaToken;
    std::optional<std::string> registrationHelper;
    std::optional<std::string> elizaUsdFeed;
    std::optional<std::string> ethUsdFeed;
    std::optional<{> accounts;
    std::optional<std::string> owner;
    std::optional<std::string> agent;
    std::optional<std::string> approver;
    std::optional<std::string> testWallet;
};

struct SolanaDeployment {
    std::string network;
    std::string rpc;
    std::optional<std::string> deployer;
    std::string programId;
    std::string desk;
    std::optional<std::string> deskOwner;
    std::string usdcMint;
    std::optional<Record<string, {> registeredTokens;
    std::string mint;
    std::string registry;
    std::string treasury;
    std::optional<double> priceUsd;
};

// =============================================================================
// DEPLOYMENT CONFIGS
// =============================================================================

using NetworkType = std::variant<"local", "testnet", "mainnet">;

// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

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
std::string getOtcAddress(std::optional<NetworkType> network);

/**
 * Get Solana desk address for current network
 */
std::string getSolanaDesk(std::optional<NetworkType> network);

/**
 * Get Solana program ID for current network
 */
std::string getSolanaProgramId(std::optional<NetworkType> network);

} // namespace elizaos
