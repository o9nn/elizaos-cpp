#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class TradeDataProviderService {
public:
    TradeDataProviderService(IAgentRuntime public runtime);
    void registerDataProvder(const std::any& dataProvider);
    void interested_trending(auto handler);
    void checkPositions();
    void updateTrending();
    void interested_priceDelta(auto chain, auto token, auto handler);
    void getTokenInfo(auto chain, auto address);
    void start(IAgentRuntime runtime);
    void stop(IAgentRuntime runtime);
    std::future<void> start();
    std::future<void> stop();
    bool isServiceRunning();
};


} // namespace elizaos
