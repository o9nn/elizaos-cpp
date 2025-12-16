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

;
import type { TopAndTrendingTokensRequest, TopAndTrendingTokensResponse } from '../types/cdp';

/**
 * Native token address used by swap protocols
 * This special address represents native tokens (ETH, MATIC, etc.)
 */
const NATIVE_TOKEN_ADDRESS = '0xEeeeeEeeeEeEeeEeEeEeeEEEeeeeEeeeeeeeEEeE';

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
  async getTokens(chain?: string): Promise<TokensResponse> {
    const params = chain ? `?chain=${encodeURIComponent(chain)}` : '';
    const response = await this.get<TokensResponse>(`/api/cdp/wallet/tokens${params}`);
    return response;
  }

  /**
   * Force sync token balances (bypasses cache)
   * Uses authenticated userId from JWT token
   * @param chain Optional specific chain to fetch (e.g., 'base', 'ethereum', 'polygon')
   */
  async syncTokens(chain?: string): Promise<TokensResponse> {
    const body = chain ? { chain } : {};
    const response = await this.post<TokensResponse>('/api/cdp/wallet/tokens/sync', body);
    return response;
  }

  /**
   * Get NFT holdings across networks (uses cache if available)
   * Uses authenticated userId from JWT token
   * @param chain Optional specific chain to fetch (e.g., 'base', 'ethereum', 'polygon')
   */
  async getNFTs(chain?: string): Promise<NFTsResponse> {
    const params = chain ? `?chain=${encodeURIComponent(chain)}` : '';
    const response = await this.get<NFTsResponse>(`/api/cdp/wallet/nfts${params}`);
    return response;
  }

  /**
   * Force sync NFTs (bypasses cache)
   * Uses authenticated userId from JWT token
   * @param chain Optional specific chain to fetch (e.g., 'base', 'ethereum', 'polygon')
   */
  async syncNFTs(chain?: string): Promise<NFTsResponse> {
    const body = chain ? { chain } : {};
    const response = await this.post<NFTsResponse>('/api/cdp/wallet/nfts/sync', body);
    return response;
  }

  /**
   * Get transaction history across networks
   * Uses authenticated userId from JWT token
   */
  async getHistory(): Promise<TransactionHistoryResponse> {
    const response = await this.get<TransactionHistoryResponse>('/api/cdp/wallet/history');
    return response;
  }

  /**
   * Send tokens from server wallet
   */
  async sendToken(request: SendTokenRequest): Promise<SendTokenResponse> {
    const response = await this.post<SendTokenResponse>('/api/cdp/wallet/send', request);
    return response;
  }

  /**
   * Send NFT from server wallet
   */
  async sendNFT(request: SendNFTRequest): Promise<SendNFTResponse> {
    const response = await this.post<SendNFTResponse>('/api/cdp/wallet/send-nft', request);
    return response;
  }

  /**
   * Get swap price estimate
   */
  async getSwapPrice(request: SwapPriceRequest): Promise<SwapPriceResponse> {
    const response = await this.post<SwapPriceResponse>('/api/cdp/wallet/swap-price', request);
    return response;
  }

  /**
   * Execute token swap
   */
  async swap(request: SwapRequest): Promise<SwapResponse> {
    const response = await this.post<SwapResponse>('/api/cdp/wallet/swap', request);
    return response;
  }

  /**
   * Search for tokens using CoinGecko
   */
  async searchTokens(request: SearchTokenRequest): Promise<SearchTokenResponse> {
    const params = new URLSearchParams();
    params.append('query', request.query);
    if (request.chain) {
      params.append('chain', request.chain);
    }
    const response = await this.get<SearchTokenResponse>(`/api/cdp/tokens/search?${params.toString()}`);
    return response;
  }

  /**
   * Get top tokens by market cap and trending tokens for a specific chain
   */
  async getTopAndTrendingTokens(request: TopAndTrendingTokensRequest): Promise<TopAndTrendingTokensResponse> {
    const params = new URLSearchParams();
    params.append('chain', request.chain);
    if (request.limit) {
      params.append('limit', request.limit.toString());
    }
    const response = await this.get<TopAndTrendingTokensResponse>(`/api/cdp/tokens/top-and-trending?${params.toString()}`);
    return response;
  }
}

} // namespace elizaos
