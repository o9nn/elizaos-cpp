#include ".types.hpp"
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



// FIXME: change runtime to just pass the dataService in
std::future<'bullish' | 'neutral' | 'bearish'> assessMarketCondition(IAgentRuntime runtime);

double calculateVolatility(const std::vector<double>& priceHistory);

// buy is different than sell
double calculateDynamicSlippage(const std::string& amount, const std::any& quoteData);

} // namespace elizaos
