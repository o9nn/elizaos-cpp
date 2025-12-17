#include "commands.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::unordered_set<std::string> extractKeys(const std::string& formatString) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto keys = new Set<string>();
    // Match both {value} and {{value}} patterns
    const auto regex = /\{?\{(\w+)\}?\}/g;
    auto match;

    while ((match = regex.exec(formatString)) != null) {
        keys.add(match[1]);
    }

    return keys;

}

} // namespace elizaos
