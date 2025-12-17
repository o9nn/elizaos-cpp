#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "...constants/chains.hpp"
#include "...contexts/ModalContext.hpp"
#include "...lib/elizaClient.hpp"
#include "...lib/number-format.hpp"
#include "..ui/bullet.hpp"
#include "..ui/button.hpp"
#include "..ui/card.hpp"
#include "FundModal.hpp"
#include "NFTDetailModal.hpp"
#include "SendModal.hpp"
#include "SwapModal.hpp"
#include "TokenDetailModal.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct Token {
    std::string symbol;
    std::string name;
    std::string balance;
    std::string balanceFormatted;
    std::optional<double> usdValue;
    std::optional<double> usdPrice;
    std::optional<std::string> contractAddress;
    std::string chain;
    double decimals;
    std::optional<std::string> icon;
};

struct NFT {
    std::string chain;
    std::string contractAddress;
    std::string tokenId;
    std::string name;
    std::string description;
    std::string image;
    std::string contractName;
    std::string tokenType;
    std::optional<std::string> balance;
    std::string trait_type;
    std::variant<std::string, double> value;
};

struct Transaction {
    std::string chain;
    std::string hash;
    std::string from;
    std::string to;
    std::string value;
    std::string asset;
    std::string category;
    double timestamp;
    std::string blockNum;
    std::string explorerUrl;
    std::variant<'sent', 'received'> direction;
    std::optional<std::optional<std::string>> icon;
    std::optional<std::optional<std::string>> contractAddress;
};

struct CDPWalletCardProps {
    std::string userId;
    std::optional<std::string> walletAddress;
};

// Expose refresh methods via ref
struct CDPWalletCardRef {
};

  // Format address for display (shortened)
  
  // Tokens state
  
  // NFTs state
  
  // Transaction history state

  // Helper: Sort tokens by USD value (highest first), fallback to chain order then symbol

  // Helper: Sort NFTs by chain order (matches SUPPORTED_CHAINS order)
      // If chain not found, put it at the end

  // Expose refresh methods via ref

  // Calculate total USD value whenever tokens change

  // Sync tokens (force refresh) concurrently across all chains with progressive updates

      // Fetch all chains concurrently with sync and update as each completes
          
          // Update UI immediately when this chain returns
              // Remove old tokens from this chain
              // Add new tokens from this chain
              return sortTokensByUsdValueDesc(mergedTokens);

      // Wait for all chain syncs to complete

  // Sync NFTs (force refresh) concurrently across all chains with progressive updates

      // Fetch all chains concurrently with sync and update as each completes
          
          // Update UI immediately when this chain returns
            // Replace only this chain's NFTs, keep others intact
              // Remove old NFTs from this chain
              // Add new NFTs from this chain
              // Sort by chain order to maintain consistent display
              return sortNftsByChainOrder(mergedNfts);

      // Wait for all chain syncs to complete

  // Fetch tokens concurrently across all chains with progressive chain-by-chain updates

      // Fetch all chains concurrently and update UI as each chain completes
          
          // Update UI immediately when this chain returns
              // Remove old tokens from this chain
              // Add new tokens from this chain
              return sortTokensByUsdValueDesc(mergedTokens);

      // Wait for all chains to complete (but UI already updated progressively)

  // Fetch NFTs concurrently across all chains with progressive chain-by-chain updates

      // Fetch all chains concurrently and update UI as each chain completes
          
          // Update UI immediately when this chain returns
            // Replace only this chain's NFTs, keep others intact
              // Remove old NFTs from this chain
              // Add new NFTs from this chain
              // Sort by chain order to maintain consistent display
              return sortNftsByChainOrder(mergedNfts);

      // Wait for all chains to complete (but UI already updated progressively)

  // Fetch transaction history

  // Notify parent of balance changes

  // Initial load

  // Load data based on active tab

  // Refresh all data using sync APIs with concurrent chain-by-chain updates

  // Format date

  // Handle copy address for a specific chain

  // Handle showing address popup

  // Handle hiding address popup with delay

  // Group transactions by date (sorted by most recent first)

  // Preserve the date order (most recent first)
    // Get the first transaction's timestamp from each group to determine order

  // Get token icon - returns JSX element
    // If token has icon from API, use it
            // Fallback to circle with first letter if image fails to load

    // Try to get icon from constants

    // Fallback: gray circle with first letter

  // Get transaction icon - returns JSX element
    // If transaction has icon from API, use it
            // Fallback to circle with first letter if image fails to load

    // Try to get icon from constants based on asset symbol

    // Fallback: gray circle with first letter

                // Close parent container (Sheet/Sidebar) if callback provided
                
                // Close parent container (Sheet/Sidebar) if callback provided
                
                // Close parent container (Sheet/Sidebar) if callback provided

                // Loading skeletons
                // Token list
              // NFT List
                // Loading state
                // NFT list
                            // Fallback to emoji if image fails to load
              // Transaction History
                            
                            // Format amount with truncation

// Add display name for debugging

} // namespace elizaos
