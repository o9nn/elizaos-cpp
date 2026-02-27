#include "logger.hpp"
#include <string>
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool parseBooleanFromText(const std::string& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!value) return false;
    const auto normalized = value.toLowerCase();
    return normalized == "true" || normalized == "1" || normalized == "yes" || normalized == "on";

}

} // namespace elizaos
