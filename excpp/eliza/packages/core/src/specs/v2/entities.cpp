#include "entities.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::optional<Entity>> findEntityByName(IAgentRuntime runtime, Memory message, State state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::any rt = runtime;
    return coreFindEntityByName(rt, message, state);

}

std::future<void> getEntityDetails(auto roomId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreGetEntityDetails({ runtime: runtime, roomId });

}

void formatEntities() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return coreFormatEntities({ entities: entities });

}

} // namespace elizaos
