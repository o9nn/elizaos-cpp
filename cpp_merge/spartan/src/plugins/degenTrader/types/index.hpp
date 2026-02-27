#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_TYPES_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_TYPES_INDEX_H
#include "core.h"
#include "@elizaos/core.h"

enum struct ServiceTypes;
class TokenSignal;
class BuySignalMessage;
class SellSignalMessage;
class PriceSignalMessage;
class PortfolioStatus;
class TradePerformanceData;

enum struct ServiceTypes {
    DEGEN_TRADING = std:("DEGEN_TRADING")
};
class TokenSignal : public object, public std::enable_shared_from_this<TokenSignal> {
public:
    using std::enable_shared_from_this<TokenSignal>::shared_from_this;
    string address;

    string symbol;

    double marketCap;

    double volume24h;

    double price;

    double liquidity;

    double score;

    array<string> reasons;

    object technicalSignals;

    object socialMetrics;

    object cmcMetrics;
};

class BuySignalMessage : public object, public std::enable_shared_from_this<BuySignalMessage> {
public:
    using std::enable_shared_from_this<BuySignalMessage>::shared_from_this;
    std::shared_ptr<UUID> positionId;

    string tokenAddress;

    string entityId;

    string expectedOutAmount;
};

class SellSignalMessage : public object, public std::enable_shared_from_this<SellSignalMessage> {
public:
    using std::enable_shared_from_this<SellSignalMessage>::shared_from_this;
    std::shared_ptr<UUID> positionId;

    string tokenAddress;

    string amount;

    string entityId;

    string expectedOutAmount;

    string currentBalance;

    string walletAddress;

    boolean isSimulation;

    string sellRecommenderId;

    string reason;
};

class PriceSignalMessage : public object, public std::enable_shared_from_this<PriceSignalMessage> {
public:
    using std::enable_shared_from_this<PriceSignalMessage>::shared_from_this;
    string tokenAddress;

    double initialPrice;

    double currentPrice;

    double priceChange;
};

class PortfolioStatus : public object, public std::enable_shared_from_this<PortfolioStatus> {
public:
    using std::enable_shared_from_this<PortfolioStatus>::shared_from_this;
    double totalValue;

    object positions;

    double solBalance;

    double drawdown;
};

class TradePerformanceData : public object, public std::enable_shared_from_this<TradePerformanceData> {
public:
    using std::enable_shared_from_this<TradePerformanceData>::shared_from_this;
    string token_address;

    double buy_price;

    string buy_timeStamp;

    double buy_amount;

    double buy_value_usd;

    double buy_market_cap;

    double buy_liquidity;

    double profit_usd;

    double profit_percent;

    boolean rapidDump;
};

#endif
