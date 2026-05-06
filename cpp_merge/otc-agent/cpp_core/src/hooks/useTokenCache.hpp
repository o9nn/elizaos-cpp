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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct TokenCacheEntry {
    Token token;
    std::optional<TokenMarketData> marketData;
    double fetchedAt;
};

// Fallback: fetch token metadata from blockchain

void useTokenCache(const std::string& tokenId);

  // Set up refresh interval only once per tokenId
    std::future<void> refreshMarketData();

      // Clean up interval when no more subscribers

void useMarketDataRefresh(const std::string& tokenId, const std::optional<Token>& token);

} // namespace elizaos
