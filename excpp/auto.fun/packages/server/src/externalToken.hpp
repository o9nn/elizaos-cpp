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
#include "db.hpp"
#include "mcap.hpp"
#include "redis.hpp"
#include "util.hpp"
#include "websocket-client.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Define max swaps to keep in Redis list (consistent with other files)

// Time in seconds before refreshing market/holder data

// Define a type for the expected structure of a processed swap
// This should match the schema of your 'swaps' table
using ProcessedSwap = {

// Type for combined market and holder data
using TokenDetails = {

/**
 * Use to fetch/update token, holder and swap data for an external token (either post-bond or imported).
 */
class ExternalToken {
public:
    ExternalToken(const std::string& mint, RedisCache redisClient);
    static std::future<std::future<ExternalToken>> create(const std::string& mint, std::optional<RedisCache> redisClient);
    void registerWebhook();
    void updateAllData();
    std::variant<Promise<TokenDetails, null>> updateMarketAndHolders(auto forceUpdate);
    Promise< fetchMarketData();
    std::future<std::vector<std::any>> _fetchHolderData(double tokenSupply);
    std::future<std::vector<ProcessedSwap>> updateLatestSwapData(auto BATCH_LIMIT);
    std::future<void> fetchHistoricalSwapData();
    std::future<void> insertProcessedSwaps(const std::vector<ProcessedSwap>& processedSwaps);

private:
    Codex sdk_;
    std::string mint_;
    WebSocketClient wsClient_;
    RedisCache redisCache_;
};


} // namespace elizaos
