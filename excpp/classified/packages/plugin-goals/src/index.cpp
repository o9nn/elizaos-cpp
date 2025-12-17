#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<GoalService> getGoalsService(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (runtime.getService('goals') || runtime.getService('goals'));

}

} // namespace elizaos
