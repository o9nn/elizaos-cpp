#include "componentService.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

ComponentService createComponentService(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new ComponentService(runtime);

}

} // namespace elizaos
