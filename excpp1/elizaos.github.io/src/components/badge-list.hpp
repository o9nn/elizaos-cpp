#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct BadgeItem {
    std::variant<std::string, double> id;
    std::string label;
    std::optional<std::string> className;
};

struct BadgeListProps {
    std::vector<BadgeItem> items;
    std::optional<std::string> className;
    std::optional<std::string> badgeClassName;
};


} // namespace elizaos
