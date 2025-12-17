#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".utils.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class TradeChainService {
public:
    TradeChainService();
    std::future<double> registerChain(const std::any& provider);
    void listActiveChains();
    void makeKeypair(auto regName);
    static void start(IAgentRuntime runtime);
    static void stop(IAgentRuntime runtime);
    bool isServiceRunning();
};


} // namespace elizaos
