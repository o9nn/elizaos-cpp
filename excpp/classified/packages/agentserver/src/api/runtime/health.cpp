#include "health.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router createHealthRouter(const std::unordered_map<UUID, IAgentRuntime>& agents, AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto router = express.Router();

    // Health check
    router.get("/ping", (_req, res) => {
        res.json({ pong: true, timestamp: Date.now() });
        });

        // Hello world endpoint
        router.get("/hello", (_req, res) => {
            std::cout << "Hello endpoint hit" << std::endl;
            res.setHeader("Content-Type", "application/json");
            res.send(/* JSON.stringify */ std::string({ message: "Hello World!" }));
            });

            // System status endpoint
            router.get("/status", (_req, res) => {
                std::cout << "Status endpoint hit" << std::endl;
                res.setHeader("Content-Type", "application/json");
                res.send(;
                JSON.stringify({
                    status: "ok",
                    agentCount: agents.size,
                    timestamp: new Date().toISOString(),
                    });
                    );
                    });

                    // Comprehensive health check
                    router.get("/health", (_req, res) => {
                        std::cout << { apiRoute = "/health" } << "Health check route hit" << std::endl;
                        const auto healthcheck = {;
                            status: "OK",
                            version: process.env.APP_VERSION || "unknown",
                            timestamp: new Date().toISOString(),
                            dependencies: {
                                // Server is healthy even with no agents - it's ready to accept agent registrations
                                agents: agents.size > 0 ? "healthy" : "ready",
                                },
                                };

                                // Always return 200 if server is running - agent count doesn't affect server health
                                res.status(200).json(healthcheck);
                                });

                                // Server stop endpoint
                                router.post("/stop", (_req, res) => {
                                    std::cout << { apiRoute = "/stop" } << "Server stopping..." << std::endl;
                                    serverInstance.stop(); // Use optional chaining in case server is std::nullopt;
                                    res.json({ message: "Server stopping..." });
                                    });

                                    return router;

}

} // namespace elizaos
