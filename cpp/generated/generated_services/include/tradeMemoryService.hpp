#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_TRADEMEMORYSERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENTRADER_SERVICES_TRADEMEMORYSERVICE_H
#include "core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;
#include "./base/BaseTradeService.h"
#include "./walletService.h"
#include "./dataService.h"
#include "./analyticsService.h"

class TradeMemory;
class TradeMemoryService;

class TradeMemory : public object, public std::enable_shared_from_this<TradeMemory> {
public:
    using std::enable_shared_from_this<TradeMemory>::shared_from_this;
    std::shared_ptr<UUID> id;

    std::string tokenAddress;

    std::string chain;

    std::any type;

    std::string amount;

    std::string price;

    std::shared_ptr<Date> timestamp;

    std::string txHash;

    object metadata;
};

class TradeMemoryService : public BaseTradeService, public std::enable_shared_from_this<TradeMemoryService> {
public:
    using std::enable_shared_from_this<TradeMemoryService>::shared_from_this;
    TradeMemoryService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService);
    virtual std::shared_ptr<Promise<void>> initialize();
    virtual std::shared_ptr<Promise<void>> storeTrade(std::shared_ptr<TradeMemory> trade);
    virtual std::shared_ptr<Promise<array<std::shared_ptr<TradeMemory>>>> getTradesForToken(std::string tokenAddress, std::string chain);
    virtual std::shared_ptr<Promise<std::shared_ptr<TradeMemory>>> createTrade(object params);
    virtual std::shared_ptr<Promise<array<std::shared_ptr<TradeMemory>>>> getRecentTrades(double limit = 10);
    virtual std::shared_ptr<Promise<array<std::shared_ptr<TradeMemory>>>> searchTrades(std::string query);
    virtual std::shared_ptr<Promise<void>> deleteTrade(std::shared_ptr<UUID> tradeId);
};

#endif
