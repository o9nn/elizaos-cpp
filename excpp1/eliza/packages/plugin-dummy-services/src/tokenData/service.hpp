#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class DummyTokenDataService {
public:
    DummyTokenDataService(std::optional<IAgentRuntime> runtime);
    TokenData generateDummyToken(const std::string& chain, std::optional<std::string> address, std::optional<std::string> query);
    std::variant<Promise<TokenData, null>> getTokenDetails(const std::string& address, const std::string& chain);
    std::future<std::vector<TokenData>> getTrendingTokens(auto chain = 'solana', auto limit = 10, auto _timePeriod = '24h');
    std::future<std::vector<TokenData>> searchTokens(const std::string& query, auto chain = 'solana', auto limit = 5);
    std::future<std::vector<TokenData>> getTokensByAddresses(const std::vector<std::string>& addresses, const std::string& chain);
    std::future<DummyTokenDataService> start(IAgentRuntime runtime);
    std::future<void> stop(IAgentRuntime runtime);
    std::future<void> start();
    std::future<void> stop();
};


} // namespace elizaos
