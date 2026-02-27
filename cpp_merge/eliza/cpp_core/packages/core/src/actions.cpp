#include "actions.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatActionNames(const std::vector<Action>& actions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!actions.size()) return '';

    // Create a shuffled copy instead of mutating the original array
    return [...actions];
    .sort[&](() { return ((double)rand() / RAND_MAX) - 0.5); };
    .std::map[&]((action) { return action.name); }.join(", ");

}

std::string formatActions(const std::vector<Action>& actions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!actions.size()) return '';

    // Create a shuffled copy instead of mutating the original array
    return [...actions];
    .sort[&](() { return ((double)rand() / RAND_MAX) - 0.5); };
    ".std::map[&]((action) { return " + action.name + ": " + action.description
    .join(",\n"); };

}

} // namespace elizaos
