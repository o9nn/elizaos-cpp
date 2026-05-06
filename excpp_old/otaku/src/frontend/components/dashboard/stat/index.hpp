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



struct DashboardStatProps {
    std::string label;
    std::string value;
    std::optional<std::string> description;
    std::optional<std::string> tag;
    ElementType icon;
    std::optional<std::variant<"positive", "negative", "neutral">> intent;
    std::optional<std::variant<"up", "down">> direction;
};

struct ArrowProps {
    std::variant<"up", "down"> direction;
    double index;
};


} // namespace elizaos
