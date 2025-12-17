#include "clients.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void parseTimeToMs(const std::string& timeStr) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto match = timeStr.match(/^(\d+)([a-z]+)$/i);
    if (!match) return 0;

    const auto [_, value, unit] = match;
    return units[unit.toLowerCase()] * Number.parseInt(value);

}

void parseExpires(const std::variant<std::string, double>& expires) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return typeof expires == "string" ? parseTimeToMs(expires) : expires;

}

} // namespace elizaos
