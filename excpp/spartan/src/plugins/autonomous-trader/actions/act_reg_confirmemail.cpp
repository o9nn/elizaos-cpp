#include "act_reg_confirmemail.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void findGeneratedCode(auto message, auto length) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto pattern = "new RegExp(" + "\\b[A-Za-z0-9]{" + length + "}\\b";
    const auto match = message.match(pattern);
    return match ? match[0] : nullptr;

}

} // namespace elizaos
