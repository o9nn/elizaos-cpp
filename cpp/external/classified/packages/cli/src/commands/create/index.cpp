#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string formatProjectType(const std::string& type) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return type == "tee" ? "TEE Project" : type.charAt(0).toUpperCase() + type.slice(1);

}

} // namespace elizaos
