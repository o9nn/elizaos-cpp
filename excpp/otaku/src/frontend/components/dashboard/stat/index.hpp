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



struct DashboardStatProps {
    std::string label;
    std::string value;
    std::optional<std::string> description;
    std::optional<std::string> tag;
    React.ElementType icon;
    std::optional<"positive" | "negative" | "neutral"> intent;
    std::optional<"up" | "down"> direction;
};

struct ArrowProps {
    "up" | "down" direction;
    double index;
};


} // namespace elizaos
