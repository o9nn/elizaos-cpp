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
#include "ProfileHoldings.module.css.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct TokenHolding {
    std::string name;
    double amount;
    double allocation;
    double price;
    double value;
};

        // Filter for only ai16z and degenai tokens


} // namespace elizaos
