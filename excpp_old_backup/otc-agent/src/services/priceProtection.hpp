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


} // namespace elizaos
