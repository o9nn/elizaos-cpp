#include "taskService.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

TaskService createTaskService(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new TaskService(runtime);

}

} // namespace elizaos
