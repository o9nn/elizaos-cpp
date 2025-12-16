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



struct Option {
    std::string icon;
    std::string label;
    std::optional<std::string> id;
};

struct MultiSelectComboboxProps {
    std::vector<Option> options;
    std::optional<std::string> className;
    std::optional<std::vector<Option>> initialSelected;
};


} // namespace elizaos
