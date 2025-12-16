#include "service.hpp"
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

  /**
   * Retrieves the entire portfolio of assets held by the wallet.
   * @param owner - Optional: The specific wallet address/owner to query if the service manages multiple.
   * @returns A promise that resolves to the wallet's portfolio.
   */

  /**
   * Retrieves the balance of a specific asset in the wallet.
   * @param assetAddress - The mint address or native identifier of the asset.
   * @param owner - Optional: The specific wallet address/owner to query.
   * @returns A promise that resolves to the user-friendly (decimal-adjusted) balance of the asset held.
   */

  /**
   * Transfers SOL from a specified keypair to a given public key.
   * This is a low-level function primarily for Solana-based wallet services.
   * @param from - The Keypair of the sender.
   * @param to - The PublicKey of the recipient.
   * @param lamports - The amount in lamports to transfer.
   * @returns A promise that resolves with the transaction signature.
   */

} // namespace elizaos
