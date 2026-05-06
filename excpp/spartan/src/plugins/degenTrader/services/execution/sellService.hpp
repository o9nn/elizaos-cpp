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
#include "base/BaseTradeService.hpp"
#include "calculation/tradeCalculation.hpp"
#include "dataService.hpp"
#include "elizaos/core.hpp"
#include "tradeMemoryService.hpp"
#include "types.hpp"
#include "utils/bignumber.hpp"
#include "utils/wallet.hpp"
#include "validation/TokenValidationService.hpp"
#include "walletService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class SellService {
public:
    SellService(IAgentRuntime runtime, WalletService walletService, DataService dataService, AnalyticsService analyticsService, TradeMemoryService tradeMemoryService);
    std::future<void> initialize();
    std::future<void> stop();
    std::future<void> handleSellSignal(const std::any& params);
    std::future<void> updateExpectedOutAmount(std::optional<SellSignalMessage> signal);
    Promise< executeSell(std::optional<SellSignalMessage> signal);

private:
    TokenValidationService validationService_;
    TradeCalculationService calculationService_;
    TradeMemoryService tradeMemoryService_;
};


} // namespace elizaos
