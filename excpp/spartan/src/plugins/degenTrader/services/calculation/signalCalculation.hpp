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
#include "base/BaseTradeService.hpp"
#include "elizaos/core.hpp"
#include "types/trading.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class SignalCalculationService {
public:
    void calculateTechnicalSignals(const std::any& marketData);
    std::future<std::vector<TokenSignal>> scoreTokenSignals(const std::vector<TokenSignal>& signals);
    std::future<double> calculateDrawdown(const std::any& portfolio);
    std::future<double> getHighWaterMark();
    std::future<void> updateHighWaterMark(double value);
};


} // namespace elizaos
