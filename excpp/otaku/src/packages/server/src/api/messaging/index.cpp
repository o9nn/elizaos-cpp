#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router messagingRouter(ElizaOS elizaOS, AgentServer serverInstance) {
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
        router.use('/', createChannelsRouter(elizaOS, serverInstance));

        // Mount unified sessions API for simplified messaging
        router.use('/', createSessionsRouter(elizaOS, serverInstance));

        // Mount jobs API for one-off messaging (similar to @bankr/sdk)
        router.use('/', createJobsRouter(elizaOS, serverInstance));

        return router;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
