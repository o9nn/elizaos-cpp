#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "..utils/wallet.hpp"
#include ".analyticsService.hpp"
#include ".dataService.hpp"
#include ".walletService.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class TradeExecutionService {
public:
    TradeExecutionService(IAgentRuntime protected runtime, WalletService protected walletService, DataService protected dataService, AnalyticsService protected analyticsService);
    std::future<void> initialize();
    std::future<void> stop();
    Promise< executeBuyTrade(auto {
    tokenAddress, auto amount, auto slippage, const std::any& });
    void if(auto result.success);
    void catch(auto error);
    void if(auto result.success);
    void catch(auto error);
    std::future<std::string> calculateExpectedAmount(const std::string& tokenAddress, double amount, bool isSell);


} // namespace elizaos
