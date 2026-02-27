#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_TYPES_H
#include "core.hpp"
// External dependency removed

typedef object MarketData;

typedef object Position;

typedef object TokenAnalysis;

class TokenSecurityData;
class TokenTradeData;
class DexScreenerPair;
class ProcessedTokenData;
class TokenAnalysisState;
class BuySignalMessage;
class SellSignalMessage;
class QuoteParams;
class StartProcessParams;
class AddTransactionParams;
class PriceSignalMessage;
class StartDegenProcessParams;
class ITradeService;

class TokenSecurityData : public object, public std::enable_shared_from_this<TokenSecurityData> {
public:
    using std::enable_shared_from_this<TokenSecurityData>::shared_from_this;
    string ownerBalance;

    string creatorBalance;

    double ownerPercentage;

    double creatorPercentage;

    string top10HolderBalance;

    double top10HolderPercent;
};

class TokenTradeData : public object, public std::enable_shared_from_this<TokenTradeData> {
public:
    using std::enable_shared_from_this<TokenTradeData>::shared_from_this;
    double price;

    double priceChange24h;

    double volume24h;

    string volume24hUsd;

    double uniqueWallets24h;

    double uniqueWallets24hChange;
};

class DexScreenerPair : public object, public std::enable_shared_from_this<DexScreenerPair> {
public:
    using std::enable_shared_from_this<DexScreenerPair>::shared_from_this;
    double priceUsd;

    object volume;

    double marketCap;

    object liquidity;

    object priceChange;

    object txns;
};

class ProcessedTokenData : public object, public std::enable_shared_from_this<ProcessedTokenData> {
public:
    using std::enable_shared_from_this<ProcessedTokenData>::shared_from_this;
    std::shared_ptr<TokenSecurityData> security;

    std::shared_ptr<TokenTradeData> tradeData;

    object dexScreenerData;

    string holderDistributionTrend;

    array<any> highValueHolders;

    boolean recentTrades;

    double highSupplyHoldersCount;
};

class TokenAnalysisState : public object, public std::enable_shared_from_this<TokenAnalysisState> {
public:
    using std::enable_shared_from_this<TokenAnalysisState>::shared_from_this;
    double lastAnalyzedIndex;

    std::shared_ptr<Set<string>> analyzedTokens;
};

class BuySignalMessage : public object, public std::enable_shared_from_this<BuySignalMessage> {
public:
    using std::enable_shared_from_this<BuySignalMessage>::shared_from_this;
    string positionId;

    string tokenAddress;

    string tradeAmount;

    string expectedOutAmount;

    string entityId;
};

class SellSignalMessage : public object, public std::enable_shared_from_this<SellSignalMessage> {
public:
    using std::enable_shared_from_this<SellSignalMessage>::shared_from_this;
    string positionId;

    string tokenAddress;

    string pairId;

    string amount;

    string currentBalance;

    string sellRecommenderId;

    string walletAddress;

    boolean isSimulation;

    string reason;

    string entityId;

    double slippage;
};

class QuoteParams : public object, public std::enable_shared_from_this<QuoteParams> {
public:
    using std::enable_shared_from_this<QuoteParams>::shared_from_this;
    string inputMint;

    string outputMint;

    string amount;

    string walletAddress;

    double slippageBps;
};

class StartProcessParams : public object, public std::enable_shared_from_this<StartProcessParams> {
public:
    using std::enable_shared_from_this<StartProcessParams>::shared_from_this;
    string id;

    string tokenAddress;

    string balance;

    boolean isSimulation;

    string initialMarketCap;

    string entityId;

    string walletAddress;

    string txHash;
};

class AddTransactionParams : public object, public std::enable_shared_from_this<AddTransactionParams> {
public:
    using std::enable_shared_from_this<AddTransactionParams>::shared_from_this;
    string id;

    string address;

    string amount;

    string walletAddress;

    boolean isSimulation;

    double marketCap;

    string entityId;

    string txHash;
};

class PriceSignalMessage : public object, public std::enable_shared_from_this<PriceSignalMessage> {
public:
    using std::enable_shared_from_this<PriceSignalMessage>::shared_from_this;
    string initialPrice;

    string currentPrice;

    double priceChange;

    string tokenAddress;
};

class StartDegenProcessParams : public StartProcessParams, public std::enable_shared_from_this<StartDegenProcessParams> {
public:
    using std::enable_shared_from_this<StartDegenProcessParams>::shared_from_this;
    string initialPrice;
};

class ITradeService : public object, public std::enable_shared_from_this<ITradeService> {
public:
    using std::enable_shared_from_this<ITradeService>::shared_from_this;
    object dataService;
};

extern object ServiceTypes;
#endif
