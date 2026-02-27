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



struct Token {
    std: symbol;
    std: name;
    std: balance;
    std: balanceFormatted;
    number | null usdValue;
    number | null usdPrice;
    std: | null contractAddress;
    std: chain;
    double decimals;
    std::optional<std:; // Token icon URL from CoinGecko> icon;
};

struct NFT {
    std: chain;
    std: contractAddress;
    std: tokenId;
    std: name;
    std: description;
    std: image;
    std: contractName;
    std: tokenType;
    std::optional<std:; // For ERC1155> balance;
    std::optional<Array<{> attributes;
    std: trait_type;
    std: | number value;
};

struct Transaction {
    std: chain;
    std: hash;
    std: from;
    std: to;
    std: value;
    std: asset;
    std: category;
    double timestamp;
    std: blockNum;
    std: explorerUrl;
    'sent' | 'received' direction;
    std::optional<std: | null> icon;
    std::optional<std: | null> contractAddress;
};

struct CDPWalletCardProps {
    std: userId;
    std::optional<std:> walletAddress;
    std::optional<[&](balance) { return void> onBalanceChange; };
    std::optional<[&]() { return void; }; // Optional callback to close parent container (Sheet/Sidebar)> onActionClick;
};

// Expose refresh methods via ref
struct CDPWalletCardRef {
    [&]() { return Promise<void> refreshTokens; };
    [&]() { return Promise<void> refreshNFTs; };
    [&]() { return Promise<void> refreshAll; };
};

  // Format address for display (shortened)
  
  // Tokens state
  
  // NFTs state
  
  // Transaction history state

  // Helper: Sort tokens by USD value (highest first), fallback to chain order then symbol

  // Helper: Sort NFTs by chain order (matches SUPPORTED_CHAINS order)
      // If chain not found, put it at the end

  // Expose refresh methods via ref
      syncTokens();
      syncNfts();

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
    
        syncTokens();
        syncNfts();
        fetchHistory();

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
