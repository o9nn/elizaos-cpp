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
    std: label;
    std::optional<std:> description;
    std::optional<std:> value;
};

struct SplitButtonProps {
    std::vector<SplitButtonOption> options;
    std::optional<std:> defaultValue;
    std::optional<std:> value;
    std::optional<std:> std::variant;
    std::optional<std:> className;
    std::optional<std:> buttonClassName;
    std::optional<std:> dropdownClassName;
    std::optional<bool> disabled;
};


} // namespace elizaos
