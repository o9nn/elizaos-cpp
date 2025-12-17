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
#include "stat-badge.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ContributorItemProps {
    std::string username;
    std::string href;
    ReactNode stats;
    std::optional<std::string> className;
    std::optional<ReactNode> children;
    double score;
};

void ContributorItem(auto href, auto stats, auto score, auto className, auto children);

} // namespace elizaos
