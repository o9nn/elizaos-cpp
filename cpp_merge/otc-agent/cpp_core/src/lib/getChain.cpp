#include "getChain.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace elizaos {

Chain getChain() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto network = getCurrentNetwork();

    // Handle unified network names
    if (network == "mainnet") return base;
    if (network == "testnet") return baseSepolia;
    if (network == "local") return anvil;

    // Default to Base mainnet
    return base;

}

std: getRpcUrl() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getEvmConfig();
    return config.rpc;

}

std: getRpcUrlForChain(const std:& chainType) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    switch (chainType) {
        // case "base":
        return std::getenv("NEXT_PUBLIC_BASE_RPC_URL") || "https://mainnet.base.org";
        // case "base-sepolia":
        return std::getenv("NEXT_PUBLIC_BASE_RPC_URL") || "https://sepolia.base.org";
        // case "bsc":
        return (;
        std::getenv("NEXT_PUBLIC_BSC_RPC_URL") ||;
        "https://bsc-dataseed1.binance.org"
        );
        // case "bsc-testnet":
        return (;
        std::getenv("NEXT_PUBLIC_BSC_RPC_URL") ||;
        "https://data-seed-prebsc-1-s1.binance.org:8545"
        );
        // case "localhost":
        // case "anvil":
        return std::getenv("NEXT_PUBLIC_RPC_URL") || "http://127.0.0.1:8545";
        // default:
        return getRpcUrl();
    }

}

} // namespace elizaos
