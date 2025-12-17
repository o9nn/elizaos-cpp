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



struct BadgeProps {
    ReactNode children;
    std::optional<std::string> variant;
    std::optional<std::string> className;
};

void Badge(auto variant, auto className);

} // namespace elizaos
