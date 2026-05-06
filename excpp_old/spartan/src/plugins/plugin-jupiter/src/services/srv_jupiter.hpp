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



class JupiterService {
public:
    JupiterService(IAgentRuntime public runtime);
    void registerProvider(const std::any& provider);
    Promise< getQuote(auto {
    inputMint, auto outputMint, auto amount, auto slippageBps, auto platformFeeBps = 200, std::optional<std::any> });
    void if(auto !this.swapApi);
    void if(auto !quoteResponseData);
    void catch(auto error);
    std::future<bool> confirmTransaction(Connection connection, const std::string& signature);
    std::future<double> getTokenPrice(const std::string& tokenMint, const std::string& quoteMint, number = 6 inputDecimals);
    void catch(auto error);
    void if(auto !response.ok);
    void catch(auto error);
    void if(auto !response.ok);
    void catch(auto error);
    void for(auto const token1 of commonTokens);
    void catch(auto error);
    void start(IAgentRuntime runtime);
    void stop(IAgentRuntime runtime);
    std::future<void> start();
    std::future<void> stop();
    bool isServiceRunning();

} // namespace elizaos
