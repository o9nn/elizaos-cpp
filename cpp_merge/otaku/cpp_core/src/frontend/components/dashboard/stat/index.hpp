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
    std: label;
    std: value;
    std::optional<std:> description;
    std::optional<std:> tag;
    ElementType icon;
    std::optional<std:> intent;
    std::optional<std:> direction;
};

struct ArrowProps {
    std: direction;
    double index;
};


} // namespace elizaos
