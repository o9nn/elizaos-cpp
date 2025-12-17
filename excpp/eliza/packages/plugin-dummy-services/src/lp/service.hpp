#pragma once
#include <any>
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



class DummyLpService {
public:
    std::string getDexName();
    static std::future<DummyLpService> start(IAgentRuntime runtime);
    static std::future<void> stop(IAgentRuntime runtime);
    std::future<std::vector<PoolInfo>> getPools(std::optional<std::string> tokenAMint, std::optional<std::string> tokenBMint);
    Promise<TransactionResult & addLiquidity(std::optional<std::any> params);
    Promise<TransactionResult & removeLiquidity(const std::any& params);
    std::variant<Promise<LpPositionDetails, null>> getLpPositionDetails(const std::string& userAccountPublicKey, const std::string& poolOrPositionIdentifier);
    std::future<std::unordered_map<std::string, std::optional<PoolInfo>>> getMarketDataForPools(const std::vector<std::string>& poolIds);
};


} // namespace elizaos
