#include "chains.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<ChainConfig> getChainConfig(const std::string& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return CHAIN_CONFIGS[network] || nullptr;

}

std::optional<Chain> getViemChain(const std::string& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(network);
    return config.chain || nullptr;

}

std::string getRpcUrl(const std::string& network, const std::string& alchemyKey) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(network);
    return config ? config.rpcUrl(alchemyKey) : nullptr;

}

std::string getExplorerUrl(const std::string& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(network);
    return config.explorerUrl || nullptr;

}

std::string getTxExplorerUrl(const std::string& network, const std::string& txHash) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto explorerUrl = getExplorerUrl(network);
    return std::to_string(explorerUrl) + "/tx/" + std::to_string(txHash);

}

std::string getAddressExplorerUrl(const std::string& network, const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto explorerUrl = getExplorerUrl(network);
    return std::to_string(explorerUrl) + "/address/" + std::to_string(address);

}

void getNativeTokenInfo(const std::string& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(network);
    return config.nativeToken || nullptr;

}

std::string getCoingeckoPlatform(const std::string& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(network);
    return config.coingeckoPlatform || nullptr;

}

network is SupportedNetwork isSupportedNetwork(const std::string& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return network in CHAIN_CONFIGS;

}

bool isMainnet(const std::string& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return MAINNET_NETWORKS.includes(network);

}

bool isTestnet(const std::string& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return TESTNET_NETWORKS.includes(network);

}

bool isCdpSwapSupported(const std::string& network) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(network);
    return config.swap.cdpSupported || false;

}

std::vector<SupportedNetwork> getCdpSwapSupportedNetworks() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return ALL_NETWORKS.filter(network => isCdpSwapSupported(network));

}

std::string normalizeTokenAddress(const std::string& token) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Check if it's already a valid contract address (0x followed by 40 hex chars)
    if (/^0x[a-fA-F0-9]{40}$/.test(token)) {
        return token;
    }
    // Otherwise, treat it as native token
    return NATIVE_TOKEN_ADDRESS;

}

} // namespace elizaos
