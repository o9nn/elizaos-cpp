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



class TradeStrategyService {
public:
    TradeStrategyService();
    void register_strategy(const std::any& strategy);
    void listActiveStrategies();
    void interested(auto chain, auto token, auto callback);
    void open_position(auto stratHndl, auto pos);
    void update_position(auto stratHndl, auto posHndl, auto pos);
    void close_position(auto stratHndl, auto posHndl, auto closeInfo);
    static void start(IAgentRuntime runtime);
    static void stop(IAgentRuntime runtime);
    bool isServiceRunning();
};


} // namespace elizaos
