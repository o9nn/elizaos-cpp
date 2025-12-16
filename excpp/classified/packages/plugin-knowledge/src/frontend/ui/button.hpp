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



struct ButtonProps {
    ReactNode children;
    std::optional<std::variant<'default', 'outline', 'ghost', 'destructive'>> variant;
    std::optional<std::variant<'default', 'sm', 'lg', 'icon'>> size;
    std::optional<std::string> className;
    std::optional<bool> disabled;
    std::optional<std::string> title;
    std::optional<std::variant<'button', 'submit', 'reset'>> type;
};


} // namespace elizaos
