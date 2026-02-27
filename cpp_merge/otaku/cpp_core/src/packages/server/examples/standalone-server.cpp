#include "standalone-server.hpp"
#include <future>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> createStandaloneServer() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            std::cout << " Creating standalone ElizaOS server..." << std::endl;

            // Create server instance
            const auto server = std::make_unique<AgentServer>();

            // Initialize with options
            std::cout << "  Initializing server..." << std::endl;
            server.initialize(serverOptions);

            // Register custom middleware if needed
            server.registerMiddleware[&]((req, res, next) {
                // Custom request processing
                res.setHeader("X-Powered-By", "ElizaOS-Standalone");
                next();
                });

                logger.success(' Server initialized successfully');

                return server;
                } catch (error) {
                    std::cerr << " Failed to create server:" << error << std::endl;
                    throw;
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> startServer() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto server = createStandaloneServer();

        // Start server
        const auto port = parseInt(std::getenv("PORT") || "3000");
        const auto host = std::getenv("HOST") || "localhost";

        std::cout << " Starting server on " + host + ":" + port + "..." << std::endl;
        server.start(port);

        // Log available endpoints
        std::cout << " Available endpoints:" << std::endl;
        std::cout << "   Dashboard: http://" + host + ":" + port + "/" << std::endl;
        std::cout << "   API: http://" + host + ":" + port + "/api/" << std::endl;
        std::cout << "   Health: http://" + host + ":" + port + "/api/health" << std::endl;
        std::cout << "   WebSocket: ws://" + host + ":" + port + "/" << std::endl;

        // Graceful shutdown
        const auto gracefulShutdown = std::async [&]() {;
            std::cout << " Graceful shutdown initiated..." << std::endl;
            server.stop();
            logger.success(' Server stopped successfully');
            std::exit(0);
            };

            process.on("SIGTERM", gracefulShutdown);
            process.on("SIGINT", gracefulShutdown);
            } catch (error) {
                std::cerr << " Server startup failed:" << error << std::endl;
                std::exit(1);
            }

}

} // namespace elizaos
