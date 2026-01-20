#include "userSessionStorage.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

UserSessionStorageService getUserSessionStorageService(IAgentRuntime runtime) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new UserSessionStorageService(runtime);

}

} // namespace elizaos
