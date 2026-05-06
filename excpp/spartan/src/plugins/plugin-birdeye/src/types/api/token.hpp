#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


// Token Trades Types
struct TokenTradesParams {
    std::string address;
    std::optional<double> limit;
    std::optional<double> offset;
    std::optional<std::string> type;
};

struct TokenTradesResponse {
    bool success;
    std::optional<std::string> signature;
    std::optional<double> blockNumber;
    std::optional<double> unixTime;
    std::optional<std::string> type;
    std::optional<std::string> tokenAddress;
    std::optional<double> tokenAmount;
    std::optional<double> tokenAmountUI;
    std::optional<std::string> tokenSymbol;
    std::optional<double> tokenDecimals;
    std::optional<double> priceUsd;
    std::optional<double> volumeUsd;
    std::optional<std::string> maker;
    std::optional<std::string> taker;
    std::optional<std::string> txType;
    std::optional<std::string> poolAddress;
    std::optional<std::string> poolName;
    std::optional<std::string> dex;
};

struct TokenListParams {
    std::optional<std::string> sort_by;
    std::optional<std::string> sort_type;
    std::optional<double> offset;
    std::optional<double> limit;
    std::optional<double> min_liquidity;
};

// Token List Types
struct TokenListResponse {
    bool success;
    std::optional<std::string> address;
    std::optional<std::string> symbol;
    std::optional<std::string> name;
    std::optional<double> decimals;
    std::optional<std::string> logoURI;
    std::optional<std::string> coingeckoId;
    std::optional<double> volume24h;
    std::optional<double> priceChange24h;
    std::optional<double> price;
};

// Token Security Types
struct TokenSecurityParams {
    std::string address;
};

struct TokenSecurityResponse {
    bool success;
    std::optional<std::string> address;
    std::optional<double> totalSupply;
    std::optional<bool> mintable;
    std::optional<bool> proxied;
    std::optional<std::string> proxy;
    std::optional<std::string> ownerAddress;
    std::optional<std::string> creatorAddress;
    std::optional<bool> honeypot;
    std::optional<bool> trading_cooldown;
    std::optional<bool> transfer_pausable;
    std::optional<bool> is_blacklisted;
    std::optional<bool> is_whitelisted;
    std::optional<bool> is_proxy;
    std::optional<bool> is_mintable;
    std::optional<bool> can_take_back_ownership;
    std::optional<bool> hidden_owner;
    std::optional<bool> anti_whale_modifiable;
    std::optional<bool> is_anti_whale;
    std::optional<bool> trading_pausable;
    std::optional<bool> can_be_blacklisted;
    std::optional<bool> is_true_token;
    std::optional<bool> is_airdrop_scam;
    std::optional<bool> slippage_modifiable;
    std::optional<bool> is_honeypot;
    std::optional<bool> transfer_pausable_time;
    std::optional<bool> is_wrapped;
};

// Token Overview Types
struct TokenOverviewParams {
    std::string address;
};

struct TokenOverviewResponse {
    bool success;
    std::optional<std::string> address;
    std::optional<double> decimals;
    std::optional<std::string> symbol;
    std::optional<std::string> name;
    std::optional<std::string> coingeckoId;
    std::optional<std::string> serumV3Usdc;
    std::optional<std::string> serumV3Usdt;
    std::optional<std::string> website;
    std::optional<std::string> telegram;
    std::optional<std::string> twitter;
    std::optional<std::string> description;
    std::optional<std::string> discord;
    std::optional<std::string> medium;
    std::optional<std::string> logoURI;
    std::optional<double> liquidity;
    std::optional<double> lastTradeUnixTime;
    std::optional<std::string> lastTradeHumanTime;
    std::optional<double> price;
    std::optional<double> history30mPrice;
    std::optional<double> priceChange30mPercent;
    std::optional<double> history1hPrice;
    std::optional<double> priceChange1hPercent;
    std::optional<double> history2hPrice;
    std::optional<double> priceChange2hPercent;
    std::optional<double> history4hPrice;
    std::optional<double> priceChange4hPercent;
    std::optional<double> history6hPrice;
    std::optional<double> priceChange6hPercent;
    std::optional<double> history8hPrice;
    std::optional<double> priceChange8hPercent;
    std::optional<double> history12hPrice;
    std::optional<double> priceChange12hPercent;
    std::optional<double> history24hPrice;
    std::optional<double> priceChange24hPercent;
    std::optional<double> uniqueWallet30m;
    std::optional<double> uniqueWalletHistory30m;
    std::optional<double> uniqueWallet30mChangePercent;
    std::optional<double> uniqueWallet1h;
    std::optional<double> uniqueWalletHistory1h;
    std::optional<double> uniqueWallet1hChangePercent;
    std::optional<double> uniqueWallet2h;
    std::optional<double> uniqueWalletHistory2h;
    std::optional<double> uniqueWallet2hChangePercent;
    std::optional<double> uniqueWallet4h;
    std::optional<double> uniqueWalletHistory4h;
    std::optional<double> uniqueWallet4hChangePercent;
    std::optional<double> uniqueWallet8h;
    std::optional<double> uniqueWalletHistory8h;
    std::optional<double> uniqueWallet8hChangePercent;
    std::optional<double> uniqueWallet24h;
    std::optional<double> uniqueWalletHistory24h;
    std::optional<double> uniqueWallet24hChangePercent;
    std::optional<double> supply;
    std::optional<double> mc;
    std::optional<double> circulatingSupply;
    std::optional<double> realMc;
    std::optional<double> holder;
    std::optional<double> trade30m;
    std::optional<double> tradeHistory30m;
    std::optional<double> trade30mChangePercent;
    std::optional<double> sell30m;
    std::optional<double> sellHistory30m;
    std::optional<double> sell30mChangePercent;
    std::optional<double> buy30m;
    std::optional<double> buyHistory30m;
    std::optional<double> buy30mChangePercent;
    std::optional<double> v30m;
    std::optional<double> v30mUSD;
    std::optional<double> vHistory30m;
    std::optional<double> vHistory30mUSD;
    std::optional<double> v30mChangePercent;
    std::optional<double> vBuy30m;
    std::optional<double> vBuy30mUSD;
    std::optional<double> vBuyHistory30m;
    std::optional<double> vBuyHistory30mUSD;
    std::optional<double> vBuy30mChangePercent;
    std::optional<double> vSell30m;
    std::optional<double> vSell30mUSD;
    std::optional<double> vSellHistory30m;
    std::optional<double> vSellHistory30mUSD;
    std::optional<double> vSell30mChangePercent;
    std::optional<double> trade1h;
    std::optional<double> tradeHistory1h;
    std::optional<double> trade1hChangePercent;
    std::optional<double> sell1h;
    std::optional<double> sellHistory1h;
    std::optional<double> sell1hChangePercent;
    std::optional<double> buy1h;
    std::optional<double> buyHistory1h;
    std::optional<double> buy1hChangePercent;
    std::optional<double> v1h;
    std::optional<double> v1hUSD;
    std::optional<double> vHistory1h;
    std::optional<double> vHistory1hUSD;
    std::optional<double> v1hChangePercent;
    std::optional<double> vBuy1h;
    std::optional<double> vBuy1hUSD;
    std::optional<double> vBuyHistory1h;
    std::optional<double> vBuyHistory1hUSD;
    std::optional<double> vBuy1hChangePercent;
    std::optional<double> vSell1h;
    std::optional<double> vSell1hUSD;
    std::optional<double> vSellHistory1h;
    std::optional<double> vSellHistory1hUSD;
    std::optional<double> vSell1hChangePercent;
    std::optional<double> trade2h;
    std::optional<double> tradeHistory2h;
    std::optional<double> trade2hChangePercent;
    std::optional<double> sell2h;
    std::optional<double> sellHistory2h;
    std::optional<double> sell2hChangePercent;
    std::optional<double> buy2h;
    std::optional<double> buyHistory2h;
    std::optional<double> buy2hChangePercent;
    std::optional<double> v2h;
    std::optional<double> v2hUSD;
    std::optional<double> vHistory2h;
    std::optional<double> vHistory2hUSD;
    std::optional<double> v2hChangePercent;
    std::optional<double> vBuy2h;
    std::optional<double> vBuy2hUSD;
    std::optional<double> vBuyHistory2h;
    std::optional<double> vBuyHistory2hUSD;
    std::optional<double> vBuy2hChangePercent;
    std::optional<double> vSell2h;
    std::optional<double> vSell2hUSD;
    std::optional<double> vSellHistory2h;
    std::optional<double> vSellHistory2hUSD;
    std::optional<double> vSell2hChangePercent;
    std::optional<double> trade4h;
    std::optional<double> tradeHistory4h;
    std::optional<double> trade4hChangePercent;
    std::optional<double> sell4h;
    std::optional<double> sellHistory4h;
    std::optional<double> sell4hChangePercent;
    std::optional<double> buy4h;
    std::optional<double> buyHistory4h;
    std::optional<double> buy4hChangePercent;
    std::optional<double> v4h;
    std::optional<double> v4hUSD;
    std::optional<double> vHistory4h;
    std::optional<double> vHistory4hUSD;
    std::optional<double> v4hChangePercent;
    std::optional<double> vBuy4h;
    std::optional<double> vBuy4hUSD;
    std::optional<double> vBuyHistory4h;
    std::optional<double> vBuyHistory4hUSD;
    std::optional<double> vBuy4hChangePercent;
    std::optional<double> vSell4h;
    std::optional<double> vSell4hUSD;
    std::optional<double> vSellHistory4h;
    std::optional<double> vSellHistory4hUSD;
    std::optional<double> vSell4hChangePercent;
    std::optional<double> trade8h;
    std::optional<double> tradeHistory8h;
    std::optional<double> trade8hChangePercent;
    std::optional<double> sell8h;
    std::optional<double> sellHistory8h;
    std::optional<double> sell8hChangePercent;
    std::optional<double> buy8h;
    std::optional<double> buyHistory8h;
    std::optional<double> buy8hChangePercent;
    std::optional<double> v8h;
    std::optional<double> v8hUSD;
    std::optional<double> vHistory8h;
    std::optional<double> vHistory8hUSD;
    std::optional<double> v8hChangePercent;
    std::optional<double> vBuy8h;
    std::optional<double> vBuy8hUSD;
    std::optional<double> vBuyHistory8h;
    std::optional<double> vBuyHistory8hUSD;
    std::optional<double> vBuy8hChangePercent;
    std::optional<double> vSell8h;
    std::optional<double> vSell8hUSD;
    std::optional<double> vSellHistory8h;
    std::optional<double> vSellHistory8hUSD;
    std::optional<double> vSell8hChangePercent;
    std::optional<double> trade24h;
    std::optional<double> tradeHistory24h;
    std::optional<double> trade24hChangePercent;
    std::optional<double> sell24h;
    std::optional<double> sellHistory24h;
    std::optional<double> sell24hChangePercent;
    std::optional<double> buy24h;
    std::optional<double> buyHistory24h;
    std::optional<double> buy24hChangePercent;
    std::optional<double> v24h;
    std::optional<double> v24hUSD;
    std::optional<double> vHistory24h;
    std::optional<double> vHistory24hUSD;
    std::optional<double> v24hChangePercent;
    std::optional<double> vBuy24h;
    std::optional<double> vBuy24hUSD;
    std::optional<double> vBuyHistory24h;
    std::optional<double> vBuyHistory24hUSD;
    std::optional<double> vBuy24hChangePercent;
    std::optional<double> vSell24h;
    std::optional<double> vSell24hUSD;
    std::optional<double> vSellHistory24h;
    std::optional<double> vSellHistory24hUSD;
    std::optional<double> vSell24hChangePercent;
    std::optional<nullptr> watch;
    std::optional<double> numberMarkets;
};

// Token Creation Info Types
struct TokenCreationInfoParams {
    std::string address;
};

struct TokenCreationInfoResponse {
    bool success;
    std::optional<std::string> txHash;
    std::optional<double> slot;
    std::optional<std::string> tokenAddress;
    std::optional<double> decimals;
    std::optional<std::string> owner;
    std::optional<double> blockUnixTime;
    std::optional<std::string> blockHumanTime;
};

struct TokenTrendingParams {
    std::optional<std::string> sort_by;
    std::optional<std::string> sort_type;
    std::optional<double> offset;
    std::optional<double> limit;
};

// Token Trending Types
struct TokenTrendingResponse {
    bool success;
    std::optional<double> updateUnixTime;
    std::optional<std::string> updateTime;
    std::optional<std::string> address;
    std::optional<std::string> symbol;
    std::optional<std::string> name;
    std::optional<double> decimals;
    std::optional<double> liquidity;
    std::optional<std::string> logoURI;
    std::optional<double> volume24hUSD;
    std::optional<double> rank;
    std::optional<double> price;
    std::optional<double> total;
};

// Token List V2 Types
struct TokenListV2Params {
    std::optional<double> offset;
    std::optional<double> limit;
    std::optional<std::string> sortBy;
    std::optional<std::string> sortOrder;
};

// this endpoint is for enterprise only and the response is not documented
struct TokenListV2Response {
    bool success;
    std::any data;
};

struct TokenMetadataMultiParams {
    std::string list_addresses;
};

struct TokenMetadataMultiResponse {
    bool success;
    std::optional<std::string> address;
    std::optional<std::string> symbol;
    std::optional<std::string> name;
    std::optional<double> decimals;
    std::optional<std::string> coingecko_id;
    std::optional<std::string> website;
    std::optional<std::string> twitter;
    std::optional<std::string> discord;
    std::optional<std::string> medium;
    std::optional<std::string> logo_uri;
};

struct TokenTradeDataMultiParams {
    std::string list_addresses;
};

struct TokenTradeDataMultiResponse {
    bool success;
};

// Token Metadata Single Types
struct TokenMetadataSingleParams {
    std::string address;
};

struct TokenMetadataSingleResponse {
    bool success;
    std::optional<std::string> address;
    std::optional<std::string> symbol;
    std::optional<std::string> name;
    std::optional<double> decimals;
    std::optional<std::string> coingecko_id;
    std::optional<std::string> website;
    std::optional<std::string> twitter;
    std::optional<std::string> discord;
    std::optional<std::string> medium;
    std::optional<std::string> logo_uri;
};

// Token Market Data Types
struct TokenMarketDataParams {
    std::string address;
};

struct TokenMarketDataResponse {
    bool success;
    std::optional<std::string> address;
    std::optional<double> liquidity;
    std::optional<double> price;
    std::optional<double> supply;
    std::optional<double> marketcap;
    std::optional<double> circulating_supply;
    std::optional<double> circulating_marketcap;
};

// Token Trade Data Single Types
struct TokenTradeDataSingleParams {
    std::string address;
};

struct TokenTradeDataSingleResponse {
    bool success;
    TokenTradeData data;
};

// Token Market Stats Types
struct TokenMarketStatsResponse {
    bool success;
    std::string address;
    double liquidity;
    double price;
    double supply;
    double marketcap;
    double circulating_supply;
    double circulating_marketcap;
};

// Token Holders Types
struct TokenHoldersParams {
    std::string address;
    std::optional<double> offset;
    std::optional<double> limit;
};

struct TokenHoldersResponse {
    bool success;
    std::optional<std::string> amount;
    std::optional<double> decimals;
    std::optional<std::string> mint;
    std::optional<std::string> owner;
    std::optional<std::string> token_account;
    std::optional<double> ui_amount;
};

// Token Mint Burn Types
struct MintBurnParams {
    std::string address;
    "block_time" sort_by;
    std::string sort_type;
    std::string type;
    std::optional<double> after_time;
    std::optional<double> before_time;
    std::optional<double> offset;
    std::optional<double> limit;
};

struct MintBurnResponse {
    bool success;
    std::optional<std::string> amount;
    std::optional<std::string> block_human_time;
    std::optional<double> block_time;
    std::optional<std::string> common_type;
    std::optional<double> decimals;
    std::optional<std::string> mint;
    std::optional<std::string> program_id;
    std::optional<double> slot;
    std::optional<std::string> tx_hash;
    std::optional<double> ui_amount;
    std::optional<std::string> ui_amount_string;
};

// New Listing Types
struct NewListingParams {
    double time_to;
    bool meme_platform_enabled;
    std::optional<double> limit;
};

struct NewListingResponse {
    bool success;
    std::string address;
    std::string symbol;
    std::string name;
    double decimals;
    std::string source;
    std::string liquidityAddedAt;
    std::string logoURI;
    double liquidity;
};

// Top Traders Types
struct TopTradersParams {
    std::string address;
    std::optional<TimeInterval> time_frame;
    std::optional<std::string> sort_type;
    std::optional<std::string> sort_by;
    std::optional<double> offset;
    std::optional<double> limit;
};

struct TopTradersResponse {
    bool success;
    std::string trader;
    double volume24h;
    double trades24h;
    double profit24h;
    double total;
};

// All Markets Types
struct AllMarketsParams {
    std::string address;
    TimeInterval time_frame;
    std::string sort_type;
    std::string sort_by;
    std::optional<double> offset;
    std::optional<double> limit;
};

struct AllMarketsResponse {
    bool success;
    std::string address;
    std::string address;
    double decimals;
    std::string symbol;
    std::optional<std::string> icon;
    std::string address;
    double decimals;
    std::string symbol;
    std::optional<std::string> icon;
    std::string createdAt;
    double liquidity;
    std::string name;
    std::optional<double> price;
    std::string source;
    double trade24h;
    double trade24hChangePercent;
    double uniqueWallet24h;
    double uniqueWallet24hChangePercent;
    double volume24h;
    double total;
};

// Token Volume By Owner Types
struct TokenVolumeByOwnerResponse {
    bool success;
    std::string tokenAddress;
    std::string owner;
    std::vector<std::string> tags;
    std::string type;
    double volume;
    double trade;
    double tradeBuy;
    double tradeSell;
    double volumeBuy;
    double volumeSell;
};


} // namespace elizaos
