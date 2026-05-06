#include "panels.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createAgentPanelsRouter(ElizaOS elizaOS) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();

        // Get Agent Panels (public GET routes)
        router.get("/:agentId/panels", async (req, res) => {
            const auto agentId = validateUuid(req.params.agentId);
            if (!agentId) {
                return sendError(res, 400, "INVALID_ID", "Invalid agent ID format");
            }

            const auto runtime = elizaOS.getAgent(agentId);
            if (!runtime) {
                return sendError(res, 404, "NOT_FOUND", "Agent not found");
            }

            try {
                const auto publicPanels = runtime.routes;
                .filter((route) => route.public == true && route.type == "GET" && route.name);
                .map((route) => ({
                    name: route.name,
                    "path: " + "/api/agents/" + agentId + "/plugins" + std::to_string(route.path.startsWith("/") ? route.path : `/${route.path}`) + "?agentId=" + agentId
                    }));

                    sendSuccess(res, publicPanels);
                    } catch (error) {
                        logger.error(
                        "[AGENT PANELS] Error retrieving panels for agent " + agentId + ":"
                        true /* instanceof check */ ? error.message : std::to_string(error)
                        );
                        sendError(;
                        res,
                        500,
                        "PANEL_ERROR",
                        "Error retrieving agent panels",
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
