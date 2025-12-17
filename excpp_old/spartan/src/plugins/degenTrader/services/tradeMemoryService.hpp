#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "analyticsService.hpp"
#include "base/BaseTradeService.hpp"
#include "dataService.hpp"
#include "elizaos/core.hpp"
#include "walletService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct TradeMemory {
    UUID id;
    std::string tokenAddress;
    std::string chain;
    std::variant<'BUY', 'SELL'> type;
    std::string amount;
    std::string price;
    Date timestamp;
    std::optional<std::string> txHash;
    std::optional<double> slippage;
    std::optional<std::string> expectedAmount;
    std::optional<std::string> receivedAmount;
    std::optional<std::string> valueUsd;
};

class TradeMemoryService {
public:
    TradeMemoryService(IAgentRuntime runtime, WalletService walletService, DataService dataService, AnalyticsService analyticsService);
    std::future<void> initialize();
    std::future<void> storeTrade(TradeMemory trade);
    std::future<std::vector<TradeMemory>> getTradesForToken(const std::string& tokenAddress, const std::string& chain);
    std::future<TradeMemory> createTrade(std::optional<std::any> params);
    std::future<std::vector<TradeMemory>> getRecentTrades(number = 10 limit);
    std::future<std::vector<TradeMemory>> searchTrades(const std::string& query);
    std::future<void> deleteTrade(UUID tradeId);
};


} // namespace elizaos
