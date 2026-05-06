#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "button.hpp"
#include "token-deals-section.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct DealsGridProps {
    std::vector<std::string> chains;
    double minMarketCap;
    double maxMarketCap;
    std::vector<std::string> negotiableTypes;
    std::optional<std::string> searchQuery;
};

struct TokenGroup {
    std::string tokenId;
    std::optional<Token> token;
    std::optional<TokenMarketData> marketData;
    std::vector<OTCConsignment> consignments;
};

// --- Helper: Filter valid consignments (active with remaining amount) ---
std::vector<OTCConsignment> filterValidConsignments(const std::vector<OTCConsignment>& consignments);

// --- Helper: Group consignments by tokenId ---
std::vector<TokenGroup> groupConsignmentsByToken(const std::vector<OTCConsignment>& consignments);

void DealsGrid(auto { filters, DealsGridProps searchQuery = "" });

} // namespace elizaos
