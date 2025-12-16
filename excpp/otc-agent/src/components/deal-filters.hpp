#include "chain-selector.hpp"
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



struct FiltersState {
    std::vector<Chain> chains;
    double minMarketCap;
    double maxMarketCap;
    std::vector<("negotiable" | "fixed")> negotiableTypes;
    std::string searchQuery;
};

struct DealFiltersProps {
    FiltersState filters;
    (filters: FiltersState) => void onFiltersChange;
};

using DealType = std::variant<"all", "negotiable", "fixed">;


} // namespace elizaos
