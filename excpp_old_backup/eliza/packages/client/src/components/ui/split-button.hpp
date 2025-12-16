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



struct SplitButtonAction {
    React.ReactNode label;
    () => void onClick;
    std::optional<React.ReactNode> icon;
    std::optional<bool> disabled;
    std::optional<'default' | 'destructive'> variant;
};

struct SplitButtonProps {
    SplitButtonAction mainAction;
    std::vector<SplitButtonAction> actions;
    std::optional<'default' | 'outline' | 'secondary' | 'ghost' | 'link' | 'destructive'> variant;
    std::optional<'default' | 'sm' | 'lg' | 'icon'> size;
    std::optional<bool> disabled;
    std::optional<std::string> className;
};


} // namespace elizaos
