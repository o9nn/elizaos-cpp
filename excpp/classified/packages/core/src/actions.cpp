#include "actions.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatActionNames(const std::vector<Action>& actions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!actions.length) return '';

    // Create a shuffled copy instead of mutating the original array
    return [...actions];
    .sort(() => Math.random() - 0.5);
    .map((action) => action.name);
    .join(", ");

}

std::string formatActions(const std::vector<Action>& actions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!actions.length) return '';

    // Create a shuffled copy without mutating the original
    return [...actions];
    .sort(() => Math.random() - 0.5);
    ".map((action) => " + "- **" + action.name + "**: " + std::to_string(action.description || "No description available")
    .join("\n");

}

} // namespace elizaos
