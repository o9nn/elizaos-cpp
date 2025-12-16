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

struct ICard {
    string | number number;
    std::string title;
    std::string description;
    std::string button;
    std::optional<() => void> onClick;
    std::optional<bool> disabled;
};


} // namespace elizaos
