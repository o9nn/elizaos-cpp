#include "actions.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatActionNames(const std::vector<Action>& actions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return actions.std::map((action) => action.name).join(", ");

}

std::string formatActions(const std::vector<Action>& actions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return "actions.std::map((action) => " + action.name + ": " + action.description;

}

} // namespace elizaos
