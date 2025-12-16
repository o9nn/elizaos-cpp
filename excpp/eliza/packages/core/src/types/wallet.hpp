#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "service.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Represents a single asset holding within a wallet, including its value.
 * This extends a generic TokenBalance with wallet-specific valuation.
 */

/**
 * Represents the entire portfolio of assets in a wallet.
 */
struct WalletPortfolio {
    double totalValueUsd;
    std::vector<WalletAsset> assets;
};

/**
 * Abstract interface for a Wallet Service.
 * Plugins that provide wallet functionality (e.g., for Solana, EVM) should implement this service.
 * It provides a standardized way for other plugins to query the state of a wallet.
 */


} // namespace elizaos
