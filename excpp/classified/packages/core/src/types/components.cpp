#include "components.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

ActionResult createActionResult(const std::optional<ActionResult>& partial) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return {
        success: true, // Default to success
        ...partial,
        };

}

} // namespace elizaos
