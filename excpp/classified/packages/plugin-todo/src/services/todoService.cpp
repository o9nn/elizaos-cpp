#include "todoService.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

TodoDataManager createTodoDataService(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new TodoDataManager(runtime);

}

} // namespace elizaos
