#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_SERVICES_CDP_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_API-CLIENT_SRC_SERVICES_CDP_H
#include "core.h"
#include "../lib/base-client.h"
#include "../types/cdp.h"

class Token;
class NFT;
class Transaction;
class WalletInfo;
class TokensResponse;
class NFTsResponse;
class TransactionHistoryResponse;
class SendTokenRequest;
class SendTokenResponse;
class SendNFTRequest;
class SendNFTResponse;
class SwapPriceRequest;
class SwapPriceResponse;
class SwapRequest;
class SwapResponse;
class SearchTokenRequest;
class CoinGeckoToken;
class SearchTokenResponse;
class CdpService;

extern string NATIVE_TOKEN_ADDRESS;
class Token : public object, public std::enable_shared_from_this<Token> {
public:
    using std::enable_shared_from_this<Token>::shared_from_this;
    string symbol;

    string name;

    string balance;

    string balanceFormatted;

    double usdValue;

    double usdPrice;

    any contractAddress;

    string chain;

    double decimals;

    string icon;
};

class NFT : public object, public std::enable_shared_from_this<NFT> {
public:
    using std::enable_shared_from_this<NFT>::shared_from_this;
    string chain;

    string contractAddress;

    string tokenId;

    string name;

    string description;

    string image;

    string contractName;

    string tokenType;

    string balance;
};

class Transaction : public object, public std::enable_shared_from_this<Transaction> {
public:
    using std::enable_shared_from_this<Transaction>::shared_from_this;
    string chain;

    string hash;

    string from;

    string to;

    string value;

    string asset;

    string category;

    double timestamp;

    string blockNum;

    string explorerUrl;

    any direction;
};

class WalletInfo : public object, public std::enable_shared_from_this<WalletInfo> {
public:
    using std::enable_shared_from_this<WalletInfo>::shared_from_this;
    string address;

    string accountName;
};

class TokensResponse : public object, public std::enable_shared_from_this<TokensResponse> {
public:
    using std::enable_shared_from_this<TokensResponse>::shared_from_this;
    array<std::shared_ptr<Token>> tokens;

    double totalUsdValue;

    string address;

    boolean fromCache;

    boolean synced;
};

class NFTsResponse : public object, public std::enable_shared_from_this<NFTsResponse> {
public:
    using std::enable_shared_from_this<NFTsResponse>::shared_from_this;
    array<std::shared_ptr<NFT>> nfts;

    string address;

    boolean fromCache;

    boolean synced;
};

class TransactionHistoryResponse : public object, public std::enable_shared_from_this<TransactionHistoryResponse> {
public:
    using std::enable_shared_from_this<TransactionHistoryResponse>::shared_from_this;
    array<std::shared_ptr<Transaction>> transactions;

    string address;
};

class SendTokenRequest : public object, public std::enable_shared_from_this<SendTokenRequest> {
public:
    using std::enable_shared_from_this<SendTokenRequest>::shared_from_this;
    string network;

    string to;

    string token;

    string amount;
};

class SendTokenResponse : public object, public std::enable_shared_from_this<SendTokenResponse> {
public:
    using std::enable_shared_from_this<SendTokenResponse>::shared_from_this;
    string transactionHash;

    string from;

    string to;

    string amount;

    string token;

    string network;
};

class SendNFTRequest : public object, public std::enable_shared_from_this<SendNFTRequest> {
public:
    using std::enable_shared_from_this<SendNFTRequest>::shared_from_this;
    string network;

    string to;

    string contractAddress;

    string tokenId;
};

class SendNFTResponse : public object, public std::enable_shared_from_this<SendNFTResponse> {
public:
    using std::enable_shared_from_this<SendNFTResponse>::shared_from_this;
    string transactionHash;

    string from;

    string to;

    string contractAddress;

    string tokenId;

    string network;
};

class SwapPriceRequest : public object, public std::enable_shared_from_this<SwapPriceRequest> {
public:
    using std::enable_shared_from_this<SwapPriceRequest>::shared_from_this;
    string network;

    string fromToken;

    string toToken;

    string fromAmount;
};

class SwapPriceResponse : public object, public std::enable_shared_from_this<SwapPriceResponse> {
public:
    using std::enable_shared_from_this<SwapPriceResponse>::shared_from_this;
    boolean liquidityAvailable;

    string toAmount;

    string minToAmount;

    string fromAmount;

    string fromToken;

    string toToken;

    string network;
};

class SwapRequest : public object, public std::enable_shared_from_this<SwapRequest> {
public:
    using std::enable_shared_from_this<SwapRequest>::shared_from_this;
    string network;

    string fromToken;

    string toToken;

    string fromAmount;

    double slippageBps;
};

class SwapResponse : public object, public std::enable_shared_from_this<SwapResponse> {
public:
    using std::enable_shared_from_this<SwapResponse>::shared_from_this;
    string transactionHash;

    string from;

    string fromToken;

    string toToken;

    string fromAmount;

    string toAmount;

    string network;

    string method;
};

class SearchTokenRequest : public object, public std::enable_shared_from_this<SearchTokenRequest> {
public:
    using std::enable_shared_from_this<SearchTokenRequest>::shared_from_this;
    string query;

    string chain;
};

class CoinGeckoToken : public object, public std::enable_shared_from_this<CoinGeckoToken> {
public:
    using std::enable_shared_from_this<CoinGeckoToken>::shared_from_this;
    string id;

    string symbol;

    string name;

    any contractAddress;

    any chain;

    any icon;

    any price;

    Record<string, string> platforms;

    double decimals;

    any marketCap;

    any volume24h;

    any priceChange24h;
};

class SearchTokenResponse : public object, public std::enable_shared_from_this<SearchTokenResponse> {
public:
    using std::enable_shared_from_this<SearchTokenResponse>::shared_from_this;
    array<std::shared_ptr<CoinGeckoToken>> tokens;
};

class CdpService : public BaseApiClient, public std::enable_shared_from_this<CdpService> {
public:
    using std::enable_shared_from_this<CdpService>::shared_from_this;
    virtual std::shared_ptr<Promise<std::shared_ptr<WalletInfo>>> getOrCreateWallet(string name);
    virtual std::shared_ptr<Promise<std::shared_ptr<TokensResponse>>> getTokens(string chain = undefined);
    virtual std::shared_ptr<Promise<std::shared_ptr<TokensResponse>>> syncTokens(string chain = undefined);
    virtual std::shared_ptr<Promise<std::shared_ptr<NFTsResponse>>> getNFTs(string chain = undefined);
    virtual std::shared_ptr<Promise<std::shared_ptr<NFTsResponse>>> syncNFTs(string chain = undefined);
    virtual std::shared_ptr<Promise<std::shared_ptr<TransactionHistoryResponse>>> getHistory();
    virtual std::shared_ptr<Promise<std::shared_ptr<SendTokenResponse>>> sendToken(std::shared_ptr<SendTokenRequest> request);
    virtual std::shared_ptr<Promise<std::shared_ptr<SendNFTResponse>>> sendNFT(std::shared_ptr<SendNFTRequest> request);
    virtual std::shared_ptr<Promise<std::shared_ptr<SwapPriceResponse>>> getSwapPrice(std::shared_ptr<SwapPriceRequest> request);
    virtual std::shared_ptr<Promise<std::shared_ptr<SwapResponse>>> swap(std::shared_ptr<SwapRequest> request);
    virtual std::shared_ptr<Promise<std::shared_ptr<SearchTokenResponse>>> searchTokens(std::shared_ptr<SearchTokenRequest> request);
    virtual std::shared_ptr<Promise<std::shared_ptr<TopAndTrendingTokensResponse>>> getTopAndTrendingTokens(std::shared_ptr<TopAndTrendingTokensRequest> request);
    CdpService(std::shared_ptr<ApiClientConfig> config);
};

#endif
