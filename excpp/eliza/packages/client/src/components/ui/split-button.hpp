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



struct SplitButtonAction {
    ReactNode label;
    std::optional<ReactNode> icon;
    std::optional<bool> disabled;
    std::optional<std::string> variant;
};

struct SplitButtonProps {
    SplitButtonAction mainAction;
    std::vector<SplitButtonAction> actions;
    std::optional<std::string> variant;
    std::optional<std::string> size;
    std::optional<bool> disabled;
    std::optional<std::string> className;
};


} // namespace elizaos
