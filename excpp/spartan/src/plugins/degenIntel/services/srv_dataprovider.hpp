#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class TradeDataProviderService {
public:
    TradeDataProviderService();
    void registerDataProvder(const std::any& dataProvider);
    void interested_trending(auto handler);
    void checkPositions();
    void updateTrending();
    void interested_priceDelta(auto chain, auto token, auto handler);
    void getTokenInfo(auto chain, auto address);
    static void start(IAgentRuntime runtime);
    static void stop(IAgentRuntime runtime);
    bool isServiceRunning();
};


} // namespace elizaos
