#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "..types/trading.hpp"
#include ".base/BaseTradeService.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class ScoringService {
public:
    std::future<std::vector<TokenSignal>> scoreTokenSignals(const std::vector<TokenSignal>& signals);
};


} // namespace elizaos
