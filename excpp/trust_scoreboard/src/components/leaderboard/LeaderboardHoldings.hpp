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
#include "LeaderboardHoldings.module.css.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct DAOHolding {
    double rank;
    std::string name;
    std::string holdings;
    std::string value;
    std::string percentage;
    std::string imageUrl;
};


} // namespace elizaos
