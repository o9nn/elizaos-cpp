#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

struct ICard {
    std::variant<std::string, double> number;
    std::string title;
    std::string description;
    std::string button;
    std::optional<bool> disabled;
};


} // namespace elizaos
