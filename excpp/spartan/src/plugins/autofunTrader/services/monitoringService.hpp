#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "analyticsService.hpp"
#include "config/config.hpp"
#include "dataService.hpp"
#include "elizaos/core.hpp"
#include "execution/tradeExecutionService.hpp"
#include "utils/wallet.hpp"
#include "walletService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class MonitoringService {
public:
    MonitoringService(IAgentRuntime runtime, DataService dataService, WalletService walletService, AnalyticsService analyticsService);
    Promise< executeBuyTrade(auto amount, auto slippage);
    Promise< executeSellTrade(auto amount, auto slippage);
    std::future<std::string> calculateExpectedAmount(const std::string& tokenAddress, double amount, bool isSell);
    std::future<void> initialize();
    std::future<void> stop();
    void startMonitoringIntervals();
    std::future<std::any> monitorToken(std::optional<std::any> options);
    std::future<void> createSellSignal(const std::string& tokenAddress, const std::string& amount, const std::string& reason);
    std::future<void> setTrailingStop(const std::string& tokenAddress, double activationPrice, const std::string& amount);
    std::future<void> monitorPrices();
    std::future<void> checkPriceThresholds(const std::string& tokenAddress, double currentPrice, const std::any& position);
};


} // namespace elizaos
