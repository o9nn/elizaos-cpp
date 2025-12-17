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
#include "TagBadge.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct FilterBarProps {
    std::vector<std::string> selectedTags;
    std::vector<Project> projects;
};

void FilterBar(auto onTagSelect); 
} // namespace elizaos
