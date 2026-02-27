#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_COMMUNITYINVESTOR_CONFIG_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_COMMUNITYINVESTOR_CONFIG_H
#include "core.h"
#include "./types.h"

class BuyAmountConfig;
class TradingConfig;
class DatabaseConfig;
class MemoryConfig;
enum struct Conviction;
enum struct RecommendationType;
enum struct TransactionType;
class BuySignalMessage;
class SellSignalMessage;

class BuyAmountConfig : public object, public std::enable_shared_from_this<BuyAmountConfig> {
public:
    using std::enable_shared_from_this<BuyAmountConfig>::shared_from_this;
    double baseAmount;

    double minAmount;

    double maxAmount;

    double trustScoreMultiplier;

    double convictionMultiplier;
};

class TradingConfig : public object, public std::enable_shared_from_this<TradingConfig> {
public:
    using std::enable_shared_from_this<TradingConfig>::shared_from_this;
    double slippageBps;

    boolean forceSimulation;

    string defaultChain;

    double maxPositionsPerToken;

    double maxPositionsPerRecommender;

    double minLiquidityUsd;

    double maxMarketCapUsd;

    std::shared_ptr<BuyAmountConfig> buyAmountConfig;
};

class DatabaseConfig : public object, public std::enable_shared_from_this<DatabaseConfig> {
public:
    using std::enable_shared_from_this<DatabaseConfig>::shared_from_this;
    string schemaVersion;

    boolean enableCaching;

    double cacheTimeout;
};

class MemoryConfig : public object, public std::enable_shared_from_this<MemoryConfig> {
public:
    using std::enable_shared_from_this<MemoryConfig>::shared_from_this;
    string embeddingModel;

    double embeddingDimension;

    double similarityThreshold;

    double cacheTimeout;
};

extern std::shared_ptr<TradingConfig> DEFAULT_TRADING_CONFIG;
extern std::shared_ptr<DatabaseConfig> DEFAULT_DATABASE_CONFIG;
extern std::shared_ptr<MemoryConfig> DEFAULT_MEMORY_CONFIG;
enum struct Conviction {
    NONE = std::string("NONE"), LOW = std::string("LOW"), MEDIUM = std::string("MEDIUM"), HIGH = std::string("HIGH"), VERY_HIGH = std::string("VERY_HIGH")
};
enum struct RecommendationType {
    BUY = std::string("BUY"), DONT_BUY = std::string("DONT_BUY"), SELL = std::string("SELL"), DONT_SELL = std::string("DONT_SELL"), NONE = std::string("NONE"), HOLD = std::string("HOLD")
};
enum struct TransactionType {
    BUY = std::string("BUY"), SELL = std::string("SELL"), TRANSFER_IN = std::string("transfer_in"), TRANSFER_OUT = std::string("transfer_out")
};
class BuySignalMessage : public object, public std::enable_shared_from_this<BuySignalMessage> {
public:
    using std::enable_shared_from_this<BuySignalMessage>::shared_from_this;
    string tokenAddress;

    string chain;

    string walletAddress;

    boolean isSimulation;

    std::shared_ptr<UUID> entityId;

    std::shared_ptr<UUID> recommendationId;

    Conviction conviction;

    string price;

    string marketCap;

    string liquidity;

    RecommendationType type;
};

class SellSignalMessage : public object, public std::enable_shared_from_this<SellSignalMessage> {
public:
    using std::enable_shared_from_this<SellSignalMessage>::shared_from_this;
    std::shared_ptr<UUID> positionId;

    string tokenAddress;

    std::shared_ptr<UUID> sellRecommenderId;

    string walletAddress;

    boolean isSimulation;
};

double getConvictionMultiplier(Conviction conviction);

double getLiquidityMultiplier(double liquidity);

double getMarketCapMultiplier(double marketCap);

double getVolumeMultiplier(double volume);

#endif
