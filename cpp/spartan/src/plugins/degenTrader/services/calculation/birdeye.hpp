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
#include "config.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class BirdeyeService {
public:
    BirdeyeService();
    void getBirdeyeFetchOptions();
    Promise< getTokenMarketData(const std::string& tokenAddress);
    std::future<std::any> getTokensMarketData(const std::vector<std::string>& tokenAddresses);
    void getEmptyMarketData();
};


} // namespace elizaos
