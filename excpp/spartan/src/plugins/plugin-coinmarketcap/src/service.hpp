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



class CoinmarketcapService {
public:
    CoinmarketcapService();
    void getBirdeyeFetchOptions();
    static std::future<BirdeyeService> start(IAgentRuntime runtime);
    static void stop(IAgentRuntime runtime);
    Promise< getTokenMarketData(const std::string& tokenAddress);
    std::future<std::any> getTokensMarketData(const std::vector<std::string>& tokenAddresses);
    void lookupToken(auto chain, auto ca);
};


} // namespace elizaos
