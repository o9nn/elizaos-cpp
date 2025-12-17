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
using SupportedChain = std::string;

/**
 * Chain UI configuration interface
 */
struct ChainUIConfig {
    SupportedChain id;
    std::string name;
    std::string displayName;
    std::string icon;
    std::string walletIcon;
    std::string symbol;
    std::string name;
    std::string icon;
    std::string explorerUrl;
    std::string color;
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
std::optional<ChainUIConfig> getChainConfig(const std::string& chain);

/**
 * Helper: Get chain icon path
 */
std::string getChainIcon(const std::string& chain);

/**
 * Helper: Get chain wallet icon path
 */
std::string getChainWalletIcon(const std::string& chain);
/**
 * Helper: Get native token icon path by chain
 */
std::string getNativeTokenIcon(const std::string& chain);

/**
 * Helper: Get native token symbol by chain
 */
std::string getNativeTokenSymbol(const std::string& chain);

/**
 * Helper: Get chain color
 */
std::string getChainColor(const std::string& chain);

/**
 * Helper: Get transaction explorer URL
 */
std::string getTxExplorerUrl(const std::string& chain, const std::string& txHash);

/**
 * Helper: Get address explorer URL
 */
std::string getAddressExplorerUrl(const std::string& chain, const std::string& address);

/**
 * Helper: Check if a chain is supported
 */

/**
 * Helper: Get chain display name
 */
std::string getChainDisplayName(const std::string& chain);

/**
 * Token icon mapping for common tokens
 * Maps token symbol to icon path
 */
  // Add more common tokens as needed

/**
 * Helper: Get token icon by symbol
 * Returns null if no icon is available (will use fallback in component)
 */
std::string getTokenIconBySymbol(const std::string& symbol);

/**
 * Helper: Get token icon by symbol or chain
 * First tries to get from TOKEN_ICONS, then falls back to native token icon
 */
std::string getTokenIcon(const std::string& symbol, std::optional<std::string> chain);


} // namespace elizaos
