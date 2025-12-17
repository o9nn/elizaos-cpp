#include "debug.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createDebugRouter(AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // Require admin for all debug endpoints
    router.use(requireAuth, requireAdmin);

    // Debug endpoint to check message servers - ADMIN ONLY
    router.get("/servers", async (_req: AuthenticatedRequest, res) => {
        try {
            const auto servers = serverInstance.getServers();
            res.json({
                success: true,
                servers: servers || [],
                count: servers.size() || 0,
                });
                } catch (error) {
                    res.status(500).json({
                        success: false,
                        error: true /* instanceof check */ ? error.message : "Unknown error",
                        });
                    }
                    });

                    return router;

}

} // namespace elizaos
