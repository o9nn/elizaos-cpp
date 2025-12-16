#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "database.hpp"
#include "marketDataService.hpp"

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
public:
    PriceProtectionService();
    std::future<ValidationResult> validateQuotePrice(const std::string& tokenId, const std::string& tokenAddress, Chain chain, double priceAtQuote, double maxDeviationBps);

private:
    MarketDataService marketDataService_;
};


} // namespace elizaos
