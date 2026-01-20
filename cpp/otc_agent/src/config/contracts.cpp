#include "contracts.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

NetworkType getCurrentNetwork() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto explicitNetwork = process.env.NEXT_PUBLIC_NETWORK || process.env.NETWORK;

    if (explicitNetwork == "mainnet") return "mainnet";
    if (explicitNetwork == "testnet" || explicitNetwork == "sepolia") return "testnet";
    if (explicitNetwork == "local" || explicitNetwork == "localnet" || explicitNetwork == "anvil") return "local";

    // Legacy flag support
    if (process.env.NEXT_PUBLIC_USE_MAINNET == "true") return "mainnet";

    // Default to mainnet for production
    return "mainnet";

}

void getContracts(std::optional<NetworkType> network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto net = network || getCurrentNetwork();
    return CONTRACT_DEPLOYMENTS[net];

}

EvmDeployment getEvmConfig(std::optional<NetworkType> network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto net = network || getCurrentNetwork();
    const auto config = CONTRACT_DEPLOYMENTS[net].evm;

    // Allow env overrides
    return {
        ...config,
        rpc: process.env.NEXT_PUBLIC_RPC_URL || config.rpc,
        contracts: {
            ...config.contracts,
            otc: process.env.NEXT_PUBLIC_OTC_ADDRESS ||
            (net == "mainnet" ? process.env.NEXT_PUBLIC_OTC_ADDRESS_MAINNET : nullptr) ||
            config.contracts.otc,
            usdc: process.env.NEXT_PUBLIC_USDC_ADDRESS || config.contracts.usdc,
            },
            accounts: {
                ...config.accounts,
                approver: process.env.APPROVER_ADDRESS || config.accounts.approver,
                },
                };

}

SolanaDeployment getSolanaConfig(std::optional<NetworkType> network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto net = network || getCurrentNetwork();
    const auto config = CONTRACT_DEPLOYMENTS[net].solana;

    // Allow env overrides
    return {
        ...config,
        rpc: process.env.NEXT_PUBLIC_SOLANA_RPC ||
        (net == "mainnet" ? process.env.NEXT_PUBLIC_SOLANA_RPC_MAINNET : nullptr) ||
        config.rpc,
        programId: process.env.NEXT_PUBLIC_SOLANA_PROGRAM_ID || config.programId,
        desk: process.env.NEXT_PUBLIC_SOLANA_DESK ||
        (net == "mainnet" ? process.env.NEXT_PUBLIC_SOLANA_DESK_MAINNET : nullptr) ||
        config.desk,
        usdcMint: process.env.NEXT_PUBLIC_SOLANA_USDC_MINT || config.usdcMint,
        };

}

std::string getOtcAddress(std::optional<NetworkType> network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getEvmConfig(network);
    return config.contracts.otc;

}

std::string getSolanaDesk(std::optional<NetworkType> network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getSolanaConfig(network);
    return config.desk;

}

std::string getSolanaProgramId(std::optional<NetworkType> network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getSolanaConfig(network);
    return config.programId;

}

} // namespace elizaos
