#include "chains.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<ChainUIConfig> getChainConfig(const std::string& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return CHAIN_UI_CONFIGS[chain] || nullptr;

}

std::string getChainIcon(const std::string& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(chain);
    return config.icon || nullptr;

}

std::string getChainWalletIcon(const std::string& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(chain);
    return config.walletIcon || nullptr;

}

std::string getNativeTokenIcon(const std::string& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(chain);
    return config.nativeToken.icon || nullptr;

}

std::string getNativeTokenSymbol(const std::string& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(chain);
    return config.nativeToken.symbol || nullptr;

}

std::string getChainColor(const std::string& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(chain);
    return config.color || "#6B7280"; // Default gray;

}

std::string getTxExplorerUrl(const std::string& chain, const std::string& txHash) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(chain);
    return "config ? " + config.explorerUrl + "/tx/" + txHash;

}

std::string getAddressExplorerUrl(const std::string& chain, const std::string& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(chain);
    return "config ? " + config.explorerUrl + "/address/" + address;

}

chain is SupportedChain isSupportedChain(const std::string& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return chain in CHAIN_UI_CONFIGS;

}

std::string getChainDisplayName(const std::string& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(chain);
    return config.displayName || chain;

}

std::string getTokenIconBySymbol(const std::string& symbol) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return TOKEN_ICONS[symbol.toUpperCase()] || nullptr;

}

std::string getTokenIcon(const std::string& symbol, std::optional<std::string> chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Try to get from common token icons first
    const auto tokenIcon = getTokenIconBySymbol(symbol);
    if (tokenIcon) return tokenIcon;

    // Fall back to native token icon if chain is provided
    if (chain) {
        return getNativeTokenIcon(chain);
    }

    return nullptr;

}

} // namespace elizaos
