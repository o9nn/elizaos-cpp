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
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class JupiterService {
public:
    JupiterService();
    void registerProvider(const std::any& provider);
    Promise< getQuote(auto outputMint, auto amount, auto slippageBps, auto platformFeeBps);
    std::future<SwapResponse> executeSwap(auto userPublicKey);
    std::future<bool> confirmTransaction(Connection connection, const std::string& signature);
    std::future<double> getTokenPrice(const std::string& tokenMint, const std::string& quoteMint, double inputDecimals = 6);
    void getBestRoute(auto outputMint, auto amount);
    std::future<double> getPriceImpact(auto outputMint, auto amount);
    std::future<double> getMinimumReceived(auto outputMint, auto amount, auto slippageBps);
    Promise< estimateGasFees(auto outputMint, auto amount);
    std::future<double> findBestSlippage(auto outputMint, auto amount);
    Promise< getTokenPair(auto outputMint);
    Promise<Array< getHistoricalPrices(auto outputMint, auto timeframe);
    Array< findArbitragePaths(auto amount, auto maxHops);
    static void start(IAgentRuntime runtime);
    static void stop(IAgentRuntime runtime);
    bool isServiceRunning();
};

} // namespace elizaos
