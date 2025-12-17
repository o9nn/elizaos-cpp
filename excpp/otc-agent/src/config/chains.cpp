#include "chains.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

ChainConfig getChainConfig(Chain chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return SUPPORTED_CHAINS[chain];

}

bool isEVMChain(Chain chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return SUPPORTED_CHAINS[chain].type == "evm";

}

bool isSolanaChain(Chain chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return SUPPORTED_CHAINS[chain].type == "solana";

}

std::optional<Chain> getChainFromId(const std::string& chainId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    for (const int [key, config] of Object.entries(SUPPORTED_CHAINS)) {
        if (config.id == chainId) return key as Chain;
    }
    return nullptr;

}

std::optional<Chain> getChainFromNumericId(double chainId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    for (const int [key, config] of Object.entries(SUPPORTED_CHAINS)) {
        if (config.chainId == chainId) return key as Chain;
    }
    return nullptr;

}

std::vector<ViemChain> getAllViemChains() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return Object.values(SUPPORTED_CHAINS);
    .filter((config) => config.viemChain);
    .map((config) => config.viemChain!);

}

} // namespace elizaos
