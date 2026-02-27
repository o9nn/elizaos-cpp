#include "chains.hpp"
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<ChainUIConfig> getChainConfig(const std:& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return CHAIN_UI_CONFIGS[chain] || nullptr;

}

std: getChainIcon(const std:& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(chain);
    return config.icon || nullptr;

}

std: getChainWalletIcon(const std:& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(chain);
    return config.walletIcon || nullptr;

}

std: getNativeTokenIcon(const std:& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(chain);
    return config.nativeToken.icon || nullptr;

}

std: getNativeTokenSymbol(const std:& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(chain);
    return config.nativeToken.symbol || nullptr;

}

std: getChainColor(const std:& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(chain);
    return config.color || "#6B7280"; // Default gray;

}

std: getTxExplorerUrl(const std:& chain, const std:& txHash) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(chain);
    return "config ? " + config.explorerUrl + "/tx/" + txHash;

}

std: getAddressExplorerUrl(const std:& chain, const std:& address) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(chain);
    return "config ? " + config.explorerUrl + "/address/" + address;

}

chain is SupportedChain isSupportedChain(const std:& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return chain in CHAIN_UI_CONFIGS;

}

std: getChainDisplayName(const std:& chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto config = getChainConfig(chain);
    return config.displayName || chain;

}

std: getTokenIconBySymbol(const std:& symbol) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return TOKEN_ICONS[symbol.toUpperCase()] || nullptr;

}

std: getTokenIcon(const std:& symbol, std::optional<std:> chain) {
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
