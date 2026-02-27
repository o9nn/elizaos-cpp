#include "actions.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatActionNames(const std::vector<Action>& actions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return actions.std::map[&]((action) { return action.name).join(", "); };

}

std::string formatActions(const std::vector<Action>& actions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return "actions.std::map[&]((action) { return " + action.name + ": " + action.description; };

}

} // namespace elizaos
