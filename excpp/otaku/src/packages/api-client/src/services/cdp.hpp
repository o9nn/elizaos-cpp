#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".lib/base-client.hpp"

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
    std::string contractAddress;
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
    std::string direction;
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
    std::string contractAddress;
    std::string chain;
    std::string icon;
    std::optional<double> price;
    std::optional<double> decimals;
    std::optional<std::optional<double>> marketCap;
    std::optional<std::optional<double>> volume24h;
    std::optional<std::optional<double>> priceChange24h;
};

struct SearchTokenResponse {
    std::vector<CoinGeckoToken> tokens;
};

/**
 * Service for interacting with CDP wallet endpoints
 */
class CdpService {
public:
    std::future<WalletInfo> getOrCreateWallet(const std::string& name);
    std::future<TokensResponse> getTokens(std::optional<std::string> chain);
    std::future<TokensResponse> syncTokens(std::optional<std::string> chain);
    std::future<NFTsResponse> getNFTs(std::optional<std::string> chain);
    std::future<NFTsResponse> syncNFTs(std::optional<std::string> chain);
    std::future<TransactionHistoryResponse> getHistory();
    std::future<SendTokenResponse> sendToken(SendTokenRequest request);
    std::future<SendNFTResponse> sendNFT(SendNFTRequest request);
    std::future<SwapPriceResponse> getSwapPrice(SwapPriceRequest request);
    std::future<SwapResponse> swap(SwapRequest request);
    std::future<SearchTokenResponse> searchTokens(SearchTokenRequest request);
    std::future<TopAndTrendingTokensResponse> getTopAndTrendingTokens(TopAndTrendingTokensRequest request);
};


} // namespace elizaos
