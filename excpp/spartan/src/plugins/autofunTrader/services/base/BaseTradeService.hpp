#include "..config/trading.hpp"
#include "..types/trading.hpp"
#include ".analyticsService.hpp"
#include ".dataService.hpp"
#include ".walletService.hpp"
#include "elizaos/core.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;
;
;
;

abstract class BaseTradeService {
  protected tradingConfig: TradingConfig;

  constructor(
    protected runtime: IAgentRuntime,
    protected walletService: WalletService,
    protected dataService: DataService,
    protected analyticsService: AnalyticsService
  ) {
    this.tradingConfig = DEFAULT_CONFIG;
  }

  public getWalletService() {
    return this.walletService;
  }

  public getDataService() {
    return this.dataService;
  }

  public getAnalyticsService() {
    return this.analyticsService;
  }
}

} // namespace elizaos
