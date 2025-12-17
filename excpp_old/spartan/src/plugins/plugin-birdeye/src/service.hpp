#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "constants.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



  //DEFAULT_RPC: 'https://api.mainnet-beta.solana.com',

class BirdeyeService {
public:
    BirdeyeService(IAgentRuntime protected runtime);
    void getBirdeyeFetchOptions(auto chain = 'solana');
    void getTrending();
    void getToken(auto chain, auto ca);
    Promise< getTokenMarketData(const std::string& tokenAddress);
    void if(auto !response.ok || !volResponse.ok || !priceHistoryResponse.ok);
    void if(auto !data.data);
    void catch(auto error);
    std::future<std::any> getTokensMarketData(const std::vector<std::string>& tokenAddresses);
    void lookupToken(auto chain, auto ca);
    std::future<BirdeyeService> start(IAgentRuntime runtime);
    void stop(IAgentRuntime runtime);
    std::future<void> stop();
    void getEmptyMarketData();
};


} // namespace elizaos
