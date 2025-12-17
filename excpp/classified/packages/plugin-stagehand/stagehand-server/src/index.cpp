#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> startServer() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "Stagehand server starting on port " + PORT << std::endl;

    // Ensure Playwright is installed before starting the server
    try {
        playwrightInstaller.ensurePlaywrightInstalled();
        } catch (error) {
            std::cerr << "Failed to ensure Playwright installation:" << error << std::endl;
            logger.warn(
            "Server will start but Stagehand operations may fail until Playwright is installed";
            );
        }

        // Create WebSocket server
        const auto wss = new WebSocketServer({ port: Number(PORT) });
        const auto sessionManager = new SessionManager(logger, playwrightInstaller);
        const auto messageHandler = new MessageHandler(sessionManager, logger);

        std::cout << "Stagehand server initialization complete" << std::endl;

        // Handle new connections
        wss.on("connection", (ws) => {
            const auto clientId = "client-" + std::to_string(Date.now()) + "-" + std::to_string(Math.random().tostd::to_string(36).substring(7));
            std::cout << "New client connected: " + clientId << std::endl;

            // Send welcome message
            ws.send(;
            JSON.stringify({
                type: "connected",
                clientId,
                version: "1.0.0",
                });
                );

                // Handle messages from client
                ws.on("message", async (data) => {
                    try {
                        const auto message = /* JSON.parse */ std::to_string(data);
                        logger.debug(`Received message from ${clientId}:`, message);

                        const auto response = messageHandler.handleMessage(message, clientId);

                        ws.send(/* JSON.stringify */ std::string(response));
                        } catch (error) {
                            std::cerr << "Error handling message from " + clientId + ":" << error << std::endl;
                            ws.send(;
                            JSON.stringify({
                                type: "error",
                                error: true /* instanceof check */ ? error.message : "Unknown error",
                                requestId: nullptr,
                                });
                                );
                            }
                            });

                            // Handle client disconnect
                            ws.on("close", () => {
                                std::cout << "Client disconnected: " + clientId << std::endl;
                                // Clean up sessions for this client
                                sessionManager.cleanupClientSessions(clientId);
                                });

                                // Handle errors
                                ws.on("error", (error) => {
                                    std::cerr << "WebSocket error for " + clientId + ":" << error << std::endl;
                                    });
                                    });

                                    // Handle server shutdown
                                    process.on("SIGINT", async () => {
                                        std::cout << "Shutting down server..." << std::endl;
                                        sessionManager.cleanup();
                                        process.exit(0);
                                        });

                                        process.on("SIGTERM", async () => {
                                            std::cout << "Shutting down server..." << std::endl;
                                            sessionManager.cleanup();
                                            process.exit(0);
                                            });

                                            std::cout << "Stagehand server listening on port " + PORT << std::endl;

}

} // namespace elizaos
