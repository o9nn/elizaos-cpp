#include "goalService.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

GoalDataManager createGoalDataService(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new GoalDataManager(runtime);

}

} // namespace elizaos
