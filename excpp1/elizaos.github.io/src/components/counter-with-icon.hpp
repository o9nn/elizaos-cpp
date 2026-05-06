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



struct CounterWithIconProps {
    LucideIcon icon;
    std::string label;
    std::variant<double, std::string> value;
    std::optional<std::string> iconClassName;
    std::optional<std::string> className;
};


} // namespace elizaos
