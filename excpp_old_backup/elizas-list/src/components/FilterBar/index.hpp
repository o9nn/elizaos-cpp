#include "TagBadge.hpp"
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



struct FilterBarProps {
    std::vector<std::string> selectedTags;
    (tag: string) => void onTagSelect;
    std::vector<Project> projects;
};

void FilterBar(auto { selectedTags, auto onTagSelect, FilterBarProps projects }); 
} // namespace elizaos
