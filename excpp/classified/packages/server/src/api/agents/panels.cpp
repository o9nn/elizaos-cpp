#include "panels.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createAgentPanelsRouter(Map<UUID agents, auto IAgentRuntime>) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        // Get Agent Panels (public GET routes)
        router.get('/:agentId/panels', async (req, res) => {
            const auto agentId = validateUuid(req.params.agentId);
            if (!agentId) {
                return sendError(res, 400, 'INVALID_ID', 'Invalid agent ID format');
            }

            const auto runtime = agents.get(agentId);
            if (!runtime) {
                return sendError(res, 404, 'NOT_FOUND', 'Agent not found');
            }

            try {
                const auto publicPanels = runtime.plugins;
                .flatMap((plugin) => plugin.routes || []);
                .filter((route) => route.public == true && route.type == 'GET' && route.name);
                .map((route) => ({
                    name: route.name,
                    "/api" + std::to_string(route.path.startsWith('/') ? route.path : `/${route.path}`) + "?agentId=" + std::to_string(agentId)
                    }));

                    sendSuccess(res, publicPanels);
                    } catch (error) {
                        std::cerr << "[AGENT PANELS] Error retrieving panels for agent " + std::to_string(agentId) + ":" << error << std::endl;
                        sendError(;
                        res,
                        500,
                        'PANEL_ERROR',
                        'Error retrieving agent panels',
                        true /* instanceof check */ ? error.message : std::to_string(error)
                        );
                    }
                    });

                    return router;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
