#include ".lib/base-client.hpp"
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
    std::string symbol;
    std::string name;
    std::string balance;
    std::string balanceFormatted;
    double usdValue;
    double usdPrice;
    string | null contractAddress;
    std::string chain;
    double decimals;
    std::optional<string; // Token icon URL from CoinGecko> icon;
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
    std::optional<string; // For ERC1155> balance;
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
    'sent' | 'received' direction;
};

struct WalletInfo {
    std::string address;
    std::string accountName;
};

struct TokensResponse {
    std::vector<Token> tokens;
    double totalUsdValue;
    std::string address;
    std::optional<bool> fromCache;
    std::optional<bool> synced;
};

struct NFTsResponse {
    std::vector<NFT> nfts;
    std::string address;
    std::optional<bool> fromCache;
    std::optional<bool> synced;
};

struct TransactionHistoryResponse {
    std::vector<Transaction> transactions;
    std::string address;
};

struct SendTokenRequest {
    std::string network;
    std::string to;
    std::string token;
    std::string amount;
};

struct SendTokenResponse {
    std::string transactionHash;
    std::string from;
    std::string to;
    std::string amount;
    std::string token;
    std::string network;
};

struct SendNFTRequest {
    std::string network;
    std::string to;
    std::string contractAddress;
    std::string tokenId;
};

struct SendNFTResponse {
    std::string transactionHash;
    std::string from;
    std::string to;
    std::string contractAddress;
    std::string tokenId;
    std::string network;
};

struct SwapPriceRequest {
    std::string network;
    std::string fromToken;
    std::string toToken;
    std::string fromAmount;
};

struct SwapPriceResponse {
    bool liquidityAvailable;
    std::string toAmount;
    std::string minToAmount;
    std::string fromAmount;
    std::string fromToken;
    std::string toToken;
    std::string network;
};

struct SwapRequest {
    std::string network;
    std::string fromToken;
    std::string toToken;
    std::string fromAmount;
    double slippageBps;
};

struct SwapResponse {
    std::string transactionHash;
    std::string from;
    std::string fromToken;
    std::string toToken;
    std::string fromAmount;
    std::string toAmount;
    std::string network;
    std::string method;
};

struct SearchTokenRequest {
    std::string query;
    std::optional<std::string> chain;
};

struct CoinGeckoToken {
    std::string id;
    std::string symbol;
    std::string name;
    string | null contractAddress;
    string | null chain;
    string | null icon;
    number | null price;
    std::optional<std::unordered_map<std::string, std::string>> platforms;
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
  async getOrCreateWallet(name: string): Promise<WalletInfo> {
    const response = await this.post<WalletInfo>('/api/cdp/wallet', { name });
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
