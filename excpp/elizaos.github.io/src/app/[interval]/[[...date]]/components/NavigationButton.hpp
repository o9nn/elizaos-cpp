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



struct NavigationButtonProps {
    std::string href;
    std::variant<"prev", "next"> direction;
    bool isVisible;
};

void NavigationButton(auto direction, auto isVisible);

} // namespace elizaos
