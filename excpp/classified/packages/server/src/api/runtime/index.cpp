#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router runtimeRouter(const std::unordered_map<UUID, IAgentRuntime>& agents, AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // Mount health endpoints at root level
    router.use('/', createHealthRouter(agents, serverInstance));

    // Mount logging endpoints
    router.use('/', createLoggingRouter(agents, serverInstance));

    // Mount debug endpoints under /debug
    router.use('/debug', createDebugRouter(serverInstance));

    return router;

}

} // namespace elizaos
