#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router messagingRouter(const std::unordered_map<UUID, IAgentRuntime>& agents, AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        if (!serverInstance) {
            throw std::runtime_error('ServerInstance is required for messaging router');
        }

        // Mount core messaging functionality at root level
        router.use('/', createMessagingCoreRouter(serverInstance));

        // Mount server management functionality
        router.use('/', createServersRouter(serverInstance));

        // Mount channel management functionality
        router.use('/', createChannelsRouter(agents, serverInstance));

        return router;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
