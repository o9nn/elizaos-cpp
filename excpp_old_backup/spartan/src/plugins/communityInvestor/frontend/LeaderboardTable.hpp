#include "ui/badge.hpp"
#include "ui/button.hpp"
#include "ui/card.hpp"
#include "ui/table.hpp"
#include "utils.hpp"
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



struct RecommendationDetailsProps {
    std::vector<Recommendation> recommendations;
    std::string username;
};

struct LeaderboardTableProps {
    std::vector<LeaderboardEntry> data;
};


} // namespace elizaos
