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



struct TokenDealsSectionProps {
    Token token;
    TokenMarketData | null marketData;
    std::vector<OTCConsignment> consignments;
};

void getDealTerms(OTCConsignment c);

void getDealScore(OTCConsignment c);


} // namespace elizaos
