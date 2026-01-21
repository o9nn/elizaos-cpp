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

extern std::string NATIVE_TOKEN_ADDRESS;
class Token : public object, public std::enable_shared_from_this<Token> {
public:
    using std::enable_shared_from_this<Token>::shared_from_this;
    std::string symbol;

    std::string name;

    std::string balance;

    std::string balanceFormatted;

    double usdValue;

    double usdPrice;

    std::any contractAddress;

    std::string chain;

    double decimals;

    std::string icon;
};

class NFT : public object, public std::enable_shared_from_this<NFT> {
public:
    using std::enable_shared_from_this<NFT>::shared_from_this;
    std::string chain;

    std::string contractAddress;

    std::string tokenId;

    std::string name;

    std::string description;

    std::string image;

    std::string contractName;

    std::string tokenType;

    std::string balance;
};

class Transaction : public object, public std::enable_shared_from_this<Transaction> {
public:
    using std::enable_shared_from_this<Transaction>::shared_from_this;
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

    std::any direction;
};

class WalletInfo : public object, public std::enable_shared_from_this<WalletInfo> {
public:
    using std::enable_shared_from_this<WalletInfo>::shared_from_this;
    std::string address;

    std::string accountName;
};

class TokensResponse : public object, public std::enable_shared_from_this<TokensResponse> {
public:
    using std::enable_shared_from_this<TokensResponse>::shared_from_this;
    array<std::shared_ptr<Token>> tokens;

    double totalUsdValue;

    std::string address;

    boolean fromCache;

    boolean synced;
};

class NFTsResponse : public object, public std::enable_shared_from_this<NFTsResponse> {
public:
    using std::enable_shared_from_this<NFTsResponse>::shared_from_this;
    array<std::shared_ptr<NFT>> nfts;

    std::string address;

    boolean fromCache;

    boolean synced;
};

class TransactionHistoryResponse : public object, public std::enable_shared_from_this<TransactionHistoryResponse> {
public:
    using std::enable_shared_from_this<TransactionHistoryResponse>::shared_from_this;
    array<std::shared_ptr<Transaction>> transactions;

    std::string address;
};

class SendTokenRequest : public object, public std::enable_shared_from_this<SendTokenRequest> {
public:
    using std::enable_shared_from_this<SendTokenRequest>::shared_from_this;
    std::string network;

    std::string to;

    std::string token;

    std::string amount;
};

class SendTokenResponse : public object, public std::enable_shared_from_this<SendTokenResponse> {
public:
    using std::enable_shared_from_this<SendTokenResponse>::shared_from_this;
    std::string transactionHash;

    std::string from;

    std::string to;

    std::string amount;

    std::string token;

    std::string network;
};

class SendNFTRequest : public object, public std::enable_shared_from_this<SendNFTRequest> {
public:
    using std::enable_shared_from_this<SendNFTRequest>::shared_from_this;
    std::string network;

    std::string to;

    std::string contractAddress;

    std::string tokenId;
};

class SendNFTResponse : public object, public std::enable_shared_from_this<SendNFTResponse> {
public:
    using std::enable_shared_from_this<SendNFTResponse>::shared_from_this;
    std::string transactionHash;

    std::string from;

    std::string to;

    std::string contractAddress;

    std::string tokenId;

    std::string network;
};

class SwapPriceRequest : public object, public std::enable_shared_from_this<SwapPriceRequest> {
public:
    using std::enable_shared_from_this<SwapPriceRequest>::shared_from_this;
    std::string network;

    std::string fromToken;

    std::string toToken;

    std::string fromAmount;
};

class SwapPriceResponse : public object, public std::enable_shared_from_this<SwapPriceResponse> {
public:
    using std::enable_shared_from_this<SwapPriceResponse>::shared_from_this;
    boolean liquidityAvailable;

    std::string toAmount;

    std::string minToAmount;

    std::string fromAmount;

    std::string fromToken;

    std::string toToken;

    std::string network;
};

class SwapRequest : public object, public std::enable_shared_from_this<SwapRequest> {
public:
    using std::enable_shared_from_this<SwapRequest>::shared_from_this;
    std::string network;

    std::string fromToken;

    std::string toToken;

    std::string fromAmount;

    double slippageBps;
};

class SwapResponse : public object, public std::enable_shared_from_this<SwapResponse> {
public:
    using std::enable_shared_from_this<SwapResponse>::shared_from_this;
    std::string transactionHash;

    std::string from;

    std::string fromToken;

    std::string toToken;

    std::string fromAmount;

    std::string toAmount;

    std::string network;

    std::string method;
};

class SearchTokenRequest : public object, public std::enable_shared_from_this<SearchTokenRequest> {
public:
    using std::enable_shared_from_this<SearchTokenRequest>::shared_from_this;
    std::string query;

    std::string chain;
};

class CoinGeckoToken : public object, public std::enable_shared_from_this<CoinGeckoToken> {
public:
    using std::enable_shared_from_this<CoinGeckoToken>::shared_from_this;
    std::string id;

    std::string symbol;

    std::string name;

    std::any contractAddress;

    std::any chain;

    std::any icon;

    std::any price;

    Record<std::string, string> platforms;

    double decimals;

    std::any marketCap;

    std::any volume24h;

    std::any priceChange24h;
};

class SearchTokenResponse : public object, public std::enable_shared_from_this<SearchTokenResponse> {
public:
    using std::enable_shared_from_this<SearchTokenResponse>::shared_from_this;
    array<std::shared_ptr<CoinGeckoToken>> tokens;
};

class CdpService : public BaseApiClient, public std::enable_shared_from_this<CdpService> {
public:
    using std::enable_shared_from_this<CdpService>::shared_from_this;
    virtual std::shared_ptr<Promise<std::shared_ptr<WalletInfo>>> getOrCreateWallet(std::string name);
    virtual std::shared_ptr<Promise<std::shared_ptr<TokensResponse>>> getTokens(std::string chain = undefined);
    virtual std::shared_ptr<Promise<std::shared_ptr<TokensResponse>>> syncTokens(std::string chain = undefined);
    virtual std::shared_ptr<Promise<std::shared_ptr<NFTsResponse>>> getNFTs(std::string chain = undefined);
    virtual std::shared_ptr<Promise<std::shared_ptr<NFTsResponse>>> syncNFTs(std::string chain = undefined);
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
