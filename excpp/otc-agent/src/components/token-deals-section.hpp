#pragma once
#include <algorithm>
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



struct TokenDealsSectionProps {
    Token token;
    std::optional<TokenMarketData> marketData;
    std::vector<OTCConsignment> consignments;
};

void getDealTerms(OTCConsignment c);

void getDealScore(OTCConsignment c);

void TokenDealsSection(auto marketData, auto consignments);

} // namespace elizaos
