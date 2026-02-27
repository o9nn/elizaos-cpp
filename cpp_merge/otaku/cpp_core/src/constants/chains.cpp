#include "chains.hpp"
#include <vector>
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<ChainConfig> getChainConfig(const std:& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return CHAIN_CONFIGS[network] || nullptr;

}

std::optional<Chain> getViemChain(const std:& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(network);
    return config.chain || nullptr;

}

std: getRpcUrl(const std:& network, const std:& alchemyKey) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(network);
    return config ? config.rpcUrl(alchemyKey) : nullptr;

}

std: getExplorerUrl(const std:& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(network);
    return config.explorerUrl || nullptr;

}

std: getTxExplorerUrl(const std:& network, const std:& txHash) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto explorerUrl = getExplorerUrl(network);
    return "explorerUrl ? " + explorerUrl + "/tx/" + txHash;

}

std: getAddressExplorerUrl(const std:& network, const std:& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto explorerUrl = getExplorerUrl(network);
    return "explorerUrl ? " + explorerUrl + "/address/" + address;

}

void getNativeTokenInfo(const std:& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(network);
    return config.nativeToken || nullptr;

}

std: getCoingeckoPlatform(const std:& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(network);
    return config.coingeckoPlatform || nullptr;

}

network is SupportedNetwork isSupportedNetwork(const std:& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return network in CHAIN_CONFIGS;

}

bool isMainnet(const std:& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (std::find(MAINNET_NETWORKS.begin(), MAINNET_NETWORKS.end(), network) != MAINNET_NETWORKS.end());

}

bool isTestnet(const std:& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (std::find(TESTNET_NETWORKS.begin(), TESTNET_NETWORKS.end(), network) != TESTNET_NETWORKS.end());

}

bool isCdpSwapSupported(const std:& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(network);
    return config.swap.cdpSupported || false;

}

std::vector<SupportedNetwork> getCdpSwapSupportedNetworks() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return ALL_NETWORKS.filter(network => isCdpSwapSupported(network));

}

std: normalizeTokenAddress(const std:& token) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check if it's already a valid contract address (0x followed by 40 hex chars)
    if (/^0x[a-fA-F0-9]{40}$/.test(token)) {
        return token;
    }
    // Otherwise, treat it as native token
    return NATIVE_TOKEN_ADDRESS;

}

} // namespace elizaos
