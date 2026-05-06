#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * CDP wallet request
 */
struct WalletRequest {
    std::string name;
};

/**
 * CDP wallet response
 */
struct WalletResponse {
    std::string address;
    std::string accountName;
};

/**
 * Request for top and trending tokens
 */
struct TopAndTrendingTokensRequest {
    std::string chain;
    std::optional<double> limit;
};

/**
 * Response for top and trending tokens
 */
struct TopAndTrendingTokensResponse {
    std::vector<std::any> topTokens;
    std::vector<std::any> trendingTokens;
};


} // namespace elizaos
