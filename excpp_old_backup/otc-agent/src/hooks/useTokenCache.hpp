#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct TokenCacheEntry {
    Token token;
    TokenMarketData | null marketData;
    double fetchedAt;
};

// Fallback: fetch token metadata from blockchain
std::future<Token | null> fetchTokenFromChain(const std::string& tokenId);

void useTokenCache(string | null tokenId);

  // Set up refresh interval only once per tokenId
    std::future<void> refreshMarketData();

      // Clean up interval when no more subscribers

void useMarketDataRefresh(string | null tokenId, Token | null token);

} // namespace elizaos
