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



struct SplitButtonAction {
    ReactNode label;
    std::optional<ReactNode> icon;
    std::optional<bool> disabled;
    std::optional<std::variant<'default', 'destructive'>> variant;
};

struct SplitButtonProps {
    SplitButtonAction mainAction;
    std::vector<SplitButtonAction> actions;
    std::optional<std::variant<'default', 'outline', 'secondary', 'ghost', 'link', 'destructive'>> variant;
    std::optional<std::variant<'default', 'sm', 'lg', 'icon'>> size;
    std::optional<bool> disabled;
    std::optional<std::string> className;
};


} // namespace elizaos
