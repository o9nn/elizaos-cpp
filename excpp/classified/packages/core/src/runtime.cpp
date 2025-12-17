#include "runtime.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void normalizeAction(const std::string& actionString) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return actionString.toLowerCase().replace(/_/g, "");

}

} // namespace elizaos
