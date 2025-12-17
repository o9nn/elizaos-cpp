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

/**
 * Frontend Chain Configuration
 * 
 * Centralized configuration for blockchain networks in the UI
 */

/**
 * Supported blockchain networks
 */
using SupportedChain = std::variant<'base', 'ethereum', 'polygon', 'arbitrum', 'optimism', 'scroll'>;

/**
 * Chain UI configuration interface
 */
struct ChainUIConfig {
    SupportedChain id;
    std::string name;
    std::string displayName;
    string; // Path to icon/logo icon;
    string; // Path to wallet icon walletIcon;
    { nativeToken;
    std::string symbol;
    std::string name;
    string; // Path to token icon icon;
    std::string explorerUrl;
    string; // Brand color for the chain color;
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

/**
 * Helper: Get chain icon path
 */

/**
 * Helper: Get chain wallet icon path
 */
/**
 * Helper: Get native token icon path by chain
 */

/**
 * Helper: Get native token symbol by chain
 */

/**
 * Helper: Get chain color
 */
std::string getChainColor(const std::string& chain);

/**
 * Helper: Get transaction explorer URL
 */

/**
 * Helper: Get address explorer URL
 */

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

/**
 * Helper: Get token icon by symbol or chain
 * First tries to get from TOKEN_ICONS, then falls back to native token icon
 */


} // namespace elizaos
