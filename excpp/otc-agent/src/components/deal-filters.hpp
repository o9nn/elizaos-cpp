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
#include "chain-selector.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct FiltersState {
    std::vector<Chain> chains;
    double minMarketCap;
    double maxMarketCap;
    std::string searchQuery;
};

struct DealFiltersProps {
    FiltersState filters;
};

using DealType = std::string;


} // namespace elizaos
