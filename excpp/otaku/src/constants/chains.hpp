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
 * Supported blockchain networks
 */
using SupportedNetwork = std::string;

/**
 * Chain configuration interface
 */
struct ChainConfig {
    std::string name;
    Chain chain;
    std::string explorerUrl;
    std::string symbol;
    std::string name;
    std::string coingeckoId;
    double decimals;
    std::string coingeckoPlatform;
    bool cdpSupported;
};

/**
 * Centralized chain configurations
 * Add new chains here to support them across the entire application
 */

/**
 * Get mainnet networks only (excludes testnets)
 */

/**
 * Get testnet networks only
 */

/**
 * Get all supported networks
 */

/**
 * Helper: Get chain config by network name
 */
std::optional<ChainConfig> getChainConfig(const std::string& network);

/**
 * Helper: Get viem chain object by network name
 */
std::optional<Chain> getViemChain(const std::string& network);

/**
 * Helper: Get RPC URL for a network
 */
std::string getRpcUrl(const std::string& network, const std::string& alchemyKey);

/**
 * Helper: Get explorer URL for a network
 */
std::string getExplorerUrl(const std::string& network);

/**
 * Helper: Get transaction explorer URL
 */
std::string getTxExplorerUrl(const std::string& network, const std::string& txHash);

/**
 * Helper: Get address explorer URL
 */
std::string getAddressExplorerUrl(const std::string& network, const std::string& address);

/**
 * Helper: Get native token info for a network
 */
void getNativeTokenInfo(const std::string& network);

/**
 * Helper: Get CoinGecko platform ID for a network
 */
std::string getCoingeckoPlatform(const std::string& network);

/**
 * Helper: Check if a network is supported
 */

/**
 * Helper: Check if a network is a mainnet
 */
bool isMainnet(const std::string& network);

/**
 * Helper: Check if a network is a testnet
 */
bool isTestnet(const std::string& network);

/**
 * Helper: Check if CDP SDK supports swaps on a network
 */
bool isCdpSwapSupported(const std::string& network);

/**
 * Helper: Get networks that support CDP swaps
 */
std::vector<SupportedNetwork> getCdpSwapSupportedNetworks();

// Swap Protocol Constants

/**
 * Native token address used by swap protocols (0x + Ee repeated)
 * This special address represents native tokens (ETH, MATIC, etc.) in swap protocols
 */

/**
 * Normalize token address for swap protocols
 * If the token address is not a valid contract address (0x...), treat it as native token
 */
std::string normalizeTokenAddress(const std::string& token);

/**
 * Uniswap V3 SwapRouter addresses per network
 */

/**
 * Uniswap V3 Quoter V2 addresses per network
 */

/**
 * Wrapped native token addresses per network
 * Uniswap V3 requires wrapped tokens for native currency swaps
 */

/**
 * Uniswap V3 pool fee tiers (in hundredths of a bip, i.e. 1e-6)
 */

} // namespace elizaos
