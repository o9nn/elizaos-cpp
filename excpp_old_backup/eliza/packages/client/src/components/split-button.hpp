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



struct SplitButtonOption {
    std::string label;
    std::optional<std::string> description;
    std::optional<std::string> value;
    std::optional<() => void> onClick;
};

struct SplitButtonProps {
    std::vector<SplitButtonOption> options;
    std::optional<std::string> defaultValue;
    std::optional<std::string> value;
    std::optional<(value: string) => void> onValueChange;
    std::optional<(option: SplitButtonOption, value: string) => void> onClick;
    std::optional<'default' | 'destructive'> variant;
    std::optional<std::string> className;
    std::optional<std::string> buttonClassName;
    std::optional<std::string> dropdownClassName;
    std::optional<bool> disabled;
};


} // namespace elizaos
