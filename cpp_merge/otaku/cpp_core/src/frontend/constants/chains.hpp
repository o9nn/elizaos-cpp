#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Frontend Chain Configuration
 * 
 * Centralized configuration for blockchain networks in the UI
 */

/**
 * Supported blockchain networks
 */
using SupportedChain = std:;

/**
 * Chain UI configuration interface
 */
struct ChainUIConfig {
    SupportedChain id;
    std: name;
    std: displayName;
    std: icon;
    std: walletIcon;
    std: symbol;
    std: name;
    std: icon;
    std: explorerUrl;
    std: color;
};

/**
 * Centralized chain UI configurations
 */

/**
 * All supported chains as an array
 */

/**
 * Helper: Get chain config by chain name
 */
std::optional<ChainUIConfig> getChainConfig(const std:& chain);

/**
 * Helper: Get chain icon path
 */
std: getChainIcon(const std:& chain);

/**
 * Helper: Get chain wallet icon path
 */
std: getChainWalletIcon(const std:& chain);
/**
 * Helper: Get native token icon path by chain
 */
std: getNativeTokenIcon(const std:& chain);

/**
 * Helper: Get native token symbol by chain
 */
std: getNativeTokenSymbol(const std:& chain);

/**
 * Helper: Get chain color
 */
std: getChainColor(const std:& chain);

/**
 * Helper: Get transaction explorer URL
 */
std: getTxExplorerUrl(const std:& chain, const std:& txHash);

/**
 * Helper: Get address explorer URL
 */
std: getAddressExplorerUrl(const std:& chain, const std:& address);

/**
 * Helper: Check if a chain is supported
 */

/**
 * Helper: Get chain display name
 */
std: getChainDisplayName(const std:& chain);

/**
 * Token icon mapping for common tokens
 * Maps token symbol to icon path
 */
  // Add more common tokens as needed

/**
 * Helper: Get token icon by symbol
 * Returns null if no icon is available (will use fallback in component)
 */
std: getTokenIconBySymbol(const std:& symbol);

/**
 * Helper: Get token icon by symbol or chain
 * First tries to get from TOKEN_ICONS, then falls back to native token icon
 */
std: getTokenIcon(const std:& symbol, std::optional<std:> chain);


} // namespace elizaos
