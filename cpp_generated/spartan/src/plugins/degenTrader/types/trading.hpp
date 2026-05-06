#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_TYPES_TRADING_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_TYPES_TRADING_H
#include "core.h"
#include "@elizaos/core.h"

typedef object WalletPortfolioItem;

typedef object WalletPortfolio;

class TokenSignal;
class RiskLimits;
class TradingConfig;
class PortfolioStatus;
class SellSignalMessage;

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

class RiskLimits : public object, public std::enable_shared_from_this<RiskLimits> {
public:
    using std::enable_shared_from_this<RiskLimits>::shared_from_this;
    double maxPositionSize;

    double maxDrawdown;

    double stopLossPercentage;

    double takeProfitPercentage;
};

class TradingConfig : public object, public std::enable_shared_from_this<TradingConfig> {
public:
    using std::enable_shared_from_this<TradingConfig>::shared_from_this;
    object intervals;

    object thresholds;

    std::shared_ptr<RiskLimits> riskLimits;

    object slippageSettings;
};

class PortfolioStatus : public object, public std::enable_shared_from_this<PortfolioStatus> {
public:
    using std::enable_shared_from_this<PortfolioStatus>::shared_from_this;
    double totalValue;

    object positions;

    double solBalance;

    double drawdown;
};

class SellSignalMessage : public object, public std::enable_shared_from_this<SellSignalMessage> {
public:
    using std::enable_shared_from_this<SellSignalMessage>::shared_from_this;
    std::shared_ptr<UUID> positionId;

    string tokenAddress;

    string amount;

    string entityId;

    string expectedOutAmount;
};

#endif
