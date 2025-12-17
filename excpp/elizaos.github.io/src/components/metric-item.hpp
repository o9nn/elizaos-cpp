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



struct MetricItemProps {
    LucideIcon icon;
    std::variant<std::string, double> value;
    std::string label;
    std::optional<std::string> valueClassName;
    std::optional<std::string> iconClassName;
    std::optional<std::string> iconBgClassName;
};

void MetricItem(auto value, auto label, auto valueClassName, auto iconClassName, auto iconBgClassName);

} // namespace elizaos
