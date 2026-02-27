#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-WORLD_SRC_TYPES_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAS-WORLD_SRC_TYPES_INDEX_H
#include "core.h"
#include "alchemy-sdk.h"
using AlchemyTokenBalance = TokenBalance;

class TokenAmount;
class ParsedTokenAccount;
class TokenBalance;
class Social;
class TokenInfo;
class BaseToken;
class Liquidity;
class TokenPair;
class TokenHolding;
class TokenData;
class TimeSeriesPoint;
class TokenAnalysis;
class DexScreenerResponse;

class TokenAmount : public object, public std::enable_shared_from_this<TokenAmount> {
public:
    using std::enable_shared_from_this<TokenAmount>::shared_from_this;
    string amount;

    double decimals;

    double uiAmount;
};

class ParsedTokenAccount : public object, public std::enable_shared_from_this<ParsedTokenAccount> {
public:
    using std::enable_shared_from_this<ParsedTokenAccount>::shared_from_this;
    object data;
};

class TokenBalance : public object, public std::enable_shared_from_this<TokenBalance> {
public:
    using std::enable_shared_from_this<TokenBalance>::shared_from_this;
    string mint;

    object tokenAmount;
};

class Social : public object, public std::enable_shared_from_this<Social> {
public:
    using std::enable_shared_from_this<Social>::shared_from_this;
    string platform;

    string handle;
};

class TokenInfo : public object, public std::enable_shared_from_this<TokenInfo> {
public:
    using std::enable_shared_from_this<TokenInfo>::shared_from_this;
    string imageUrl;

    array<object> websites;

    array<object> socials;
};

class BaseToken : public object, public std::enable_shared_from_this<BaseToken> {
public:
    using std::enable_shared_from_this<BaseToken>::shared_from_this;
    string address;

    string name;

    string symbol;
};

class Liquidity : public object, public std::enable_shared_from_this<Liquidity> {
public:
    using std::enable_shared_from_this<Liquidity>::shared_from_this;
    double usd;

    double base;

    double quote;
};

class TokenPair : public object, public std::enable_shared_from_this<TokenPair> {
public:
    using std::enable_shared_from_this<TokenPair>::shared_from_this;
    string chainId;

    string dexId;

    string url;

    string pairAddress;

    std::shared_ptr<BaseToken> baseToken;

    std::shared_ptr<BaseToken> quoteToken;

    string priceUsd;

    std::shared_ptr<Liquidity> liquidity;

    double fdv;

    double marketCap;

    std::shared_ptr<TokenInfo> info;

    object volume;

    object txns;

    double pairCreatedAt;

    double score;
};

class TokenHolding : public object, public std::enable_shared_from_this<TokenHolding> {
public:
    using std::enable_shared_from_this<TokenHolding>::shared_from_this;
    string address;

    double balance;

    double decimals;

    double usdValue;

    double percentageOwned;

    double firstReceived;

    object marketData;
};

class TokenData : public object, public std::enable_shared_from_this<TokenData> {
public:
    using std::enable_shared_from_this<TokenData>::shared_from_this;
    array<std::shared_ptr<TokenHolding>> holdings;

    object summary;
};

class TimeSeriesPoint : public object, public std::enable_shared_from_this<TimeSeriesPoint> {
public:
    using std::enable_shared_from_this<TimeSeriesPoint>::shared_from_this;
    double timestamp;

    double price;

    double volume;
};

class TokenAnalysis : public object, public std::enable_shared_from_this<TokenAnalysis> {
public:
    using std::enable_shared_from_this<TokenAnalysis>::shared_from_this;
    double priceChange24h;

    double priceChange7d;

    double volumeChange24h;

    double volumeAvg24h;

    double txCount24h;

    double holders;

    array<std::shared_ptr<TimeSeriesPoint>> timeSeries;
};

class DexScreenerResponse : public object, public std::enable_shared_from_this<DexScreenerResponse> {
public:
    using std::enable_shared_from_this<DexScreenerResponse>::shared_from_this;
    array<object> pairs;
};

#endif
