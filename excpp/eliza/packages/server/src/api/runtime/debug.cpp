#include "debug.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createDebugRouter(AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // Debug endpoint to check message servers
    router.get('/servers', async (_req, res) => {
        try {
            const auto servers = serverInstance.getServers();
            res.json({
                success: true,
                servers: servers || [],
                count: servers.length || 0,
                });
                } catch (error) {
                    res.status(500).json({
                        success: false,
                        error: true /* instanceof check */ ? error.message : 'Unknown error',
                        });
                    }
                    });

                    return router;

}

} // namespace elizaos
