#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-BIRDEYE_SRC_TYPES_API_SEARCH_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_PLUGIN-BIRDEYE_SRC_TYPES_API_SEARCH_H
#include "core.h"
#include "../shared.h"

class TokenMarketSearchParams;
class TokenMarketSearchResponse;
class TokenResult;
class MarketResult;

class TokenMarketSearchParams : public object, public std::enable_shared_from_this<TokenMarketSearchParams> {
public:
    using std::enable_shared_from_this<TokenMarketSearchParams>::shared_from_this;
    any chain;

    string keyword;

    any target;

    any sort_by;

    any sort_type;

    boolean verify_token;

    string markets;

    double offset;

    double limit;
};

class TokenMarketSearchResponse : public object, public std::enable_shared_from_this<TokenMarketSearchResponse> {
public:
    using std::enable_shared_from_this<TokenMarketSearchResponse>::shared_from_this;
    boolean success;

    object data;
};

class TokenResult : public object, public std::enable_shared_from_this<TokenResult> {
public:
    using std::enable_shared_from_this<TokenResult>::shared_from_this;
    string name;

    string symbol;

    string address;

    string network;

    double fdv;

    double market_cap;

    double liquidity;

    double volume_24h_change_percent;

    double price;

    double price_change_24h_percent;

    double buy_24h;

    double buy_24h_change_percent;

    double sell_24h;

    double sell_24h_change_percent;

    double trade_24h;

    double trade_24h_change_percent;

    double unique_wallet_24h;

    double unique_view_24h_change_percent;

    string last_trade_human_time;

    double last_trade_unix_time;

    string creation_time;

    double volume_24h_usd;

    string logo_uri;
};

class MarketResult : public object, public std::enable_shared_from_this<MarketResult> {
public:
    using std::enable_shared_from_this<MarketResult>::shared_from_this;
    string name;

    string address;

    double liquidity;

    string source;

    double trade_24h;

    double trade_24h_change_percent;

    double unique_wallet_24h;

    double unique_wallet_24h_change_percent;

    string last_trade_human_time;

    double last_trade_unix_time;

    string base_mint;

    string quote_mint;

    double amount_base;

    double amout_quote;

    string creation_time;

    double volume_24h_usd;
};

#endif
