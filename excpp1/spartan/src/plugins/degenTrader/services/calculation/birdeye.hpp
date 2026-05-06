#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "..config.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class BirdeyeService {
public:
    BirdeyeService(const std::string& private apiKey);
    void getBirdeyeFetchOptions();
    Promise< getTokenMarketData(const std::string& tokenAddress);
    void if(auto !response.ok || !volResponse.ok || !priceHistoryResponse.ok);
    void if(auto !data.data);
    void catch(auto error);
    std::future<std::any> getTokensMarketData(const std::vector<std::string>& tokenAddresses);
    void getEmptyMarketData();
};


} // namespace elizaos
