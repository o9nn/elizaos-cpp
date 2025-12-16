#include "..types.hpp"
#include "..utils/bignumber.hpp"
#include "..utils/wallet.hpp"
#include ".analyticsService.hpp"
#include ".base/BaseTradeService.hpp"
#include ".calculation/tradeCalculation.hpp"
#include ".dataService.hpp"
#include ".tradeMemoryService.hpp"
#include ".validation/TokenValidationService.hpp"
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



class SellService extends BaseTradeService {
  private pendingSells: { [tokenAddress: string]: BN } = {};

        // Add validation for slippage with warning and enforce stricter limits
        /*

        */

        // why are we getting this after the trade execution?
        // for the price? shouldn't we already have it?
        //console.log('sell marketData', marketData)


} // namespace elizaos
