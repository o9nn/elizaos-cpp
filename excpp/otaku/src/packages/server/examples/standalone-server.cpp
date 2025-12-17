#include "standalone-server.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> createStandaloneServer() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            std::cout << ' Creating standalone ElizaOS server...' << std::endl;

            // Create server instance
            const auto server = new AgentServer();

            // Initialize with options
            std::cout << '  Initializing server...' << std::endl;
            server.initialize(serverOptions);

            // Register custom middleware if needed
            server.registerMiddleware((req, res, next) => {
                // Custom request processing
                res.setHeader('X-Powered-By', 'ElizaOS-Standalone');
                next();
                });

                logger.success(' Server initialized successfully');

                return server;
                } catch (error) {
                    std::cerr << ' Failed to create server:' << error << std::endl;
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
        const auto port = parseInt(process.env.PORT || '3000');
        const auto host = process.env.HOST || 'localhost';

        std::cout << " Starting server on " + std::to_string(host) + ":" + std::to_string(port) + "..." << std::endl;
        server.start(port);

        // Log available endpoints
        std::cout << ' Available endpoints:' << std::endl;
        std::cout << "   Dashboard: http://" + std::to_string(host) + ":" + std::to_string(port) + "/" << std::endl;
        std::cout << "   API: http://" + std::to_string(host) + ":" + std::to_string(port) + "/api/" << std::endl;
        std::cout << "   Health: http://" + std::to_string(host) + ":" + std::to_string(port) + "/api/health" << std::endl;
        std::cout << "   WebSocket: ws://" + std::to_string(host) + ":" + std::to_string(port) + "/" << std::endl;

        // Graceful shutdown
        const auto gracefulShutdown = async () => {;
            std::cout << ' Graceful shutdown initiated...' << std::endl;
            server.stop();
            logger.success(' Server stopped successfully');
            process.exit(0);
            };

            process.on('SIGTERM', gracefulShutdown);
            process.on('SIGINT', gracefulShutdown);
            } catch (error) {
                std::cerr << ' Server startup failed:' << error << std::endl;
                process.exit(1);
            }

}

} // namespace elizaos
