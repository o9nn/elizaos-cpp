#include "lib/base-client.hpp"
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
 * Native token address used by swap protocols
 * This special address represents native tokens (ETH, MATIC, etc.)
 */

struct Token {
    std: symbol;
    std: name;
    std: balance;
    std: balanceFormatted;
    double usdValue;
    double usdPrice;
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
};

struct WalletInfo {
    std: address;
    std: accountName;
};

struct TokensResponse {
    std::vector<Token> tokens;
    double totalUsdValue;
    std: address;
    std::optional<bool> fromCache;
    std::optional<bool> synced;
};

struct NFTsResponse {
    std::vector<NFT> nfts;
    std: address;
    std::optional<bool> fromCache;
    std::optional<bool> synced;
};

struct TransactionHistoryResponse {
    std::vector<Transaction> transactions;
    std: address;
};

struct SendTokenRequest {
    std: network;
    std: to;
    std: token;
    std: amount;
};

struct SendTokenResponse {
    std: transactionHash;
    std: from;
    std: to;
    std: amount;
    std: token;
    std: network;
};

struct SendNFTRequest {
    std: network;
    std: to;
    std: contractAddress;
    std: tokenId;
};

struct SendNFTResponse {
    std: transactionHash;
    std: from;
    std: to;
    std: contractAddress;
    std: tokenId;
    std: network;
};

struct SwapPriceRequest {
    std: network;
    std: fromToken;
    std: toToken;
    std: fromAmount;
};

struct SwapPriceResponse {
    bool liquidityAvailable;
    std: toAmount;
    std: minToAmount;
    std: fromAmount;
    std: fromToken;
    std: toToken;
    std: network;
};

struct SwapRequest {
    std: network;
    std: fromToken;
    std: toToken;
    std: fromAmount;
    double slippageBps;
};

struct SwapResponse {
    std: transactionHash;
    std: from;
    std: fromToken;
    std: toToken;
    std: fromAmount;
    std: toAmount;
    std: network;
    std: method;
};

struct SearchTokenRequest {
    std: query;
    std::optional<std:> chain;
};

struct CoinGeckoToken {
    std: id;
    std: symbol;
    std: name;
    std: | null contractAddress;
    std: | null chain;
    std: | null icon;
    number | null price;
    std::optional<std::unordered_map<std:, std:>> platforms;
    std::optional<double> decimals;
    std::optional<number | null> marketCap;
    std::optional<number | null> volume24h;
    std::optional<number | null> priceChange24h;
};

struct SearchTokenResponse {
    std::vector<CoinGeckoToken> tokens;
};

/**
 * Service for interacting with CDP wallet endpoints
 */
class CdpService extends BaseApiClient {
  /**
   * Get or create a server wallet for a user
   */
  std::async getOrCreateWallet(name: std:): Promise<WalletInfo> {
    const response = this.post<WalletInfo>('/api/cdp/wallet', { name });
    return response;
  }

  /**
   * Get token balances across all networks (uses cache if available)
   * Uses authenticated userId from JWT token
   * @param chain Optional specific chain to fetch (e.g., 'base', 'ethereum', 'polygon')
   */

  /**
   * Force sync token balances (bypasses cache)
   * Uses authenticated userId from JWT token
   * @param chain Optional specific chain to fetch (e.g., 'base', 'ethereum', 'polygon')
   */

  /**
   * Get NFT holdings across networks (uses cache if available)
   * Uses authenticated userId from JWT token
   * @param chain Optional specific chain to fetch (e.g., 'base', 'ethereum', 'polygon')
   */

  /**
   * Force sync NFTs (bypasses cache)
   * Uses authenticated userId from JWT token
   * @param chain Optional specific chain to fetch (e.g., 'base', 'ethereum', 'polygon')
   */

  /**
   * Get transaction history across networks
   * Uses authenticated userId from JWT token
   */

  /**
   * Send tokens from server wallet
   */

  /**
   * Send NFT from server wallet
   */

  /**
   * Get swap price estimate
   */

  /**
   * Execute token swap
   */

  /**
   * Search for tokens using CoinGecko
   */

  /**
   * Get top tokens by market cap and trending tokens for a specific chain
   */

} // namespace elizaos
