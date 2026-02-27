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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct DashboardStatProps {
    std::string label;
    std::string value;
    std::optional<std::string> description;
    std::optional<std::string> tag;
    ElementType icon;
    std::optional<std::string> intent;
    std::optional<std::string> direction;
};

struct ArrowProps {
    std::string direction;
    double index;
};


} // namespace elizaos
