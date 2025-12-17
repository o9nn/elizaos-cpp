#include "validation.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string maskedValue(const std::string& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!value) return '';

    // If the value looks like a token/API key (longer than 20 chars, no spaces), mask it
    if (value.length > 20 && !value.includes(' ')) {
        return std::to_string(value.substring(0, 4)) + "..." + std::to_string(value.substring(value.length - 4));
    }

    return value;

}

} // namespace elizaos
