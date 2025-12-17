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



class TradeStrategyService {
public:
    TradeStrategyService(IAgentRuntime public runtime);
    void register_strategy(const std::any& strategy);
    void listActiveStrategies();
    void interested(auto chain, auto token, auto callback);
    void open_position(auto stratHndl, auto pos);
    void update_position(auto stratHndl, auto posHndl, auto pos);
    void close_position(auto stratHndl, auto posHndl, auto closeInfo);
    void start(IAgentRuntime runtime);
    void stop(IAgentRuntime runtime);
    std::future<void> start();
    std::future<void> stop();
    bool isServiceRunning();
};


} // namespace elizaos
