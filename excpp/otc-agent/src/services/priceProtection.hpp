#include "database.hpp"
#include "marketDataService.hpp"
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
import type { Chain } from "@/config/chains";
;

struct ValidationResult {
    bool isValid;
    double currentPrice;
    double priceAtQuote;
    double deviation;
    double deviationBps;
    double maxAllowedDeviationBps;
    std::optional<std::string> reason;
};


class PriceProtectionService {
  private marketDataService: MarketDataService;

  constructor() {
    this.marketDataService = new MarketDataService();
  }

  async validateQuotePrice(
    tokenId: string,
    tokenAddress: string,
    chain: Chain,
    priceAtQuote: number,
    maxDeviationBps: number,
  ): Promise<ValidationResult> {
    const marketData = await MarketDataDB.getMarketData(tokenId);

    let currentPrice: number;
    if (!marketData || Date.now() - marketData.lastUpdated > 300000) {
      currentPrice = await this.marketDataService.fetchTokenPrice(
        tokenAddress,
        chain,
      );
    } else {
      currentPrice = marketData.priceUsd;
    }

    const deviation = Math.abs(currentPrice - priceAtQuote);
    const deviationBps = Math.floor((deviation / priceAtQuote) * 10000);

    const isValid = deviationBps <= maxDeviationBps;

    return {
      isValid,
      currentPrice,
      priceAtQuote,
      deviation,
      deviationBps,
      maxAllowedDeviationBps: maxDeviationBps,
      reason: isValid
        ? undefined
        : `Price moved ${deviationBps / 100}%, exceeding maximum allowed ${maxDeviationBps / 100}%`,
    };
  }
}

} // namespace elizaos
