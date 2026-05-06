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
#include "constants.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



  //DEFAULT_RPC: 'https://api.mainnet-beta.solana.com',

class BirdeyeService {
public:
    BirdeyeService();
    void getBirdeyeFetchOptions(auto chain);
    void getTrending();
    void getToken(auto chain, auto ca);
    Promise< getTokenMarketData(const std::string& tokenAddress);
    std::future<std::any> getTokensMarketData(const std::vector<std::string>& tokenAddresses);
    void lookupToken(auto chain, auto ca);
    static std::future<BirdeyeService> start(IAgentRuntime runtime);
    static void stop(IAgentRuntime runtime);
    void getEmptyMarketData();
};


} // namespace elizaos
