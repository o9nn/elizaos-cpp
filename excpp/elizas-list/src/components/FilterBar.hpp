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
    std::vector<std::string> tags;
    (search: string, tags: string[]) => void onFilterChange;
};

void FilterBar(auto { tags, FilterBarProps onFilterChange }); 
} // namespace elizaos
