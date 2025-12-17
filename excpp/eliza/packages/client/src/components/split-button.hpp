#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct SplitButtonOption {
    std::string label;
    std::optional<std::string> description;
    std::optional<std::string> value;
};

struct SplitButtonProps {
    std::vector<SplitButtonOption> options;
    std::optional<std::string> defaultValue;
    std::optional<std::string> value;
    std::optional<std::string> variant;
    std::optional<std::string> className;
    std::optional<std::string> buttonClassName;
    std::optional<std::string> dropdownClassName;
    std::optional<bool> disabled;
};


} // namespace elizaos
