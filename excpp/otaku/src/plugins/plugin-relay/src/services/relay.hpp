#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "utils/multichain-wallet.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class RelayService {
public:
    RelayService(IAgentRuntime runtime);
    std::string capabilityDescription() const;
    static std::future<RelayService> start(IAgentRuntime runtime);
    std::future<void> stop();
    std::future<void> initialize(IAgentRuntime runtime);
    void getQuote(QuoteRequest request);
    std::future<std::vector<RelayStatus>> getStatus(StatusRequest request);
    std::future<std::vector<RelayChain>> getChains();
    std::future<std::vector<RelayCurrencyInfo>> getCurrencies(double chainId);
    std::future<void> indexTransaction(const std::string& txHash, double chainId);
};


} // namespace elizaos
