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



struct Option {
    std: icon;
    std: label;
    std::optional<std:> id;
};

struct MultiSelectComboboxProps {
    std::vector<Option> options;
    std::optional<std:> className;
    std::optional<std::vector<Option>> initialSelected;
};


} // namespace elizaos
