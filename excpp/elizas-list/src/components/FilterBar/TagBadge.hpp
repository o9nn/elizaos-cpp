#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct TagBadgeProps {
    std::string tag;
    bool selected;
    std::optional<bool> showIcon;
    std::optional<bool> showTrending;
};

void TagBadge(auto selected, auto onClick, auto showIcon, auto showTrending); 
} // namespace elizaos
