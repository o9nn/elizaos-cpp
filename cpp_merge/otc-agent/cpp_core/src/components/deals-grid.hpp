#include "button.hpp"
#include "token-deals-section.hpp"
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



struct DealsGridProps {
    { filters;
    std::vector<std::string> chains;
    double minMarketCap;
    double maxMarketCap;
    std::vector<std::string> negotiableTypes;
    std::optional<std::string> searchQuery;
};

struct TokenGroup {
    std::string tokenId;
    Token | null token;
    TokenMarketData | null marketData;
    std::vector<OTCConsignment> consignments;
};

// --- Helper: Filter valid consignments (active with remaining amount) ---
std::vector<OTCConsignment> filterValidConsignments(const std::vector<OTCConsignment>& consignments);

// --- Helper: Group consignments by tokenId ---
std::vector<TokenGroup> groupConsignmentsByToken(const std::vector<OTCConsignment>& consignments);

void DealsGrid(auto { filters, DealsGridProps searchQuery = "" });

} // namespace elizaos
