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
ction works

    // Click enabled item

    // Open dropdown

    // Select item

    // Dropdown should close

    // Both parts should have consistent styling


} // namespace elizaos
